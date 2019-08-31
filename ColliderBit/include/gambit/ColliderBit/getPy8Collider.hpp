//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  ColliderBit event loop functions returning
///  collider Monte Carlo event simulators.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Aldo Saavedra
///
///  \author Andy Buckley
///
///  \author Chris Rogan
///          (crogan@cern.ch)
///  \date 2014 Aug
///  \date 2015 May
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jul
///  \date 2018 Jan
///  \date 2019 Jan, Feb, May
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date   2017 March
///  \date   2018 Jan
///  \date   2018 May
///
///  *********************************************


#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"

// #define COLLIDERBIT_DEBUG
#define DEBUG_PREFIX "DEBUG: OMP thread " << omp_get_thread_num() << ":  "

namespace Gambit
{

  namespace ColliderBit
  {

    /// Retrieve a Pythia hard-scattering Monte Carlo simulation
    template<typename PythiaT, typename EventT>
    void getPy8Collider(Py8Collider<PythiaT, EventT>& result,
                        const MCLoopInfo& RunMC,
                        const SLHAstruct& slha,
                        const str model_suffix,
                        const int iteration,
                        void(*wrapup)(),
                        const Options& runOptions)
    {
      static bool first = true;
      static str pythia_doc_path;
      static double xsec_veto_fb;

      if (iteration == BASE_INIT)
      {
        // Setup the Pythia documentation path and print the banner once
        if (first)
        {
          const str be = "Pythia" + model_suffix;
          const str ver = Backends::backendInfo().default_version(be);
          pythia_doc_path = Backends::backendInfo().path_dir(be, ver) + "/../share/Pythia8/xmldoc/";
          result.banner(pythia_doc_path);
          first = false;
        }
      }

      // To make sure that the Pythia instance on each OMP thread gets all the 
      // options it should, all the options parsing and initialisation happens in
      // START_SUBPROCESS (OMP parallel) rather than COLLIDER_INIT (only thread 0).
      // We may want to split this up, so that all the yaml options are parsed in 
      // COLLIDER_INIT (by thread 0), and used to initialize the 'result' instance
      // of each thread within START_SUBPROCESS.
      // 
      // else if (iteration == COLLIDER_INIT)
      // {
      //   // Do the option parsing here?
      // }

      else if (iteration == START_SUBPROCESS)
      {

        std::vector<str> pythiaOptions;

        // By default we tell Pythia to be quiet. (Can be overridden from yaml settings)
        pythiaOptions.push_back("Print:quiet = on");
        pythiaOptions.push_back("SLHA:verbose = 0");

        // Get options from yaml file.
        const double xsec_veto_default = 0.0;
        const bool partonOnly_default = false;
        const double antiktR_default = 0.4;
        if (runOptions.hasKey(RunMC.current_collider()))
        {
          YAML::Node colNode = runOptions.getValue<YAML::Node>(RunMC.current_collider());
          Options colOptions(colNode);
          xsec_veto_fb = colOptions.getValueOrDef<double>(xsec_veto_default, "xsec_veto");
          result.partonOnly = colOptions.getValueOrDef<bool>(partonOnly_default, "partonOnly");
          result.antiktR = colOptions.getValueOrDef<double>(antiktR_default, "antiktR");
          if (colOptions.hasKey("pythia_settings"))
          {
            std::vector<str> addPythiaOptions = colNode["pythia_settings"].as<std::vector<str> >();
            pythiaOptions.insert(pythiaOptions.end(), addPythiaOptions.begin(), addPythiaOptions.end());
          }
        }
        else
        {
          xsec_veto_fb = xsec_veto_default;
          result.partonOnly = partonOnly_default;
          result.antiktR = antiktR_default;
        }

        // We need showProcesses for the xsec veto.
        pythiaOptions.push_back("Init:showProcesses = on");

        // We need "SLHA:file = slhaea" for the SLHAea interface.
        pythiaOptions.push_back("SLHA:file = slhaea");

        // Variables needed for the xsec veto
        std::stringstream processLevelOutput;
        str _junk, readline;
        int code, nxsec;
        double xsec, totalxsec;

        // Each thread needs an independent Pythia instance at the start
        // of each event generation loop.
        // Thus, the actual Pythia initialization is
        // *after* COLLIDER_INIT, within omp parallel.

        result.clear();

        // Add the thread-specific seed to the Pythia options
        str seed = std::to_string(int(Random::draw() * 899990000.));
        pythiaOptions.push_back("Random:seed = " + seed);

        #ifdef COLLIDERBIT_DEBUG
          cout << DEBUG_PREFIX << "getPythia"+model_suffix+": My Pythia seed is: " << seed << endl;
        #endif

        try
        {
            result.init(pythia_doc_path, pythiaOptions, &slha, processLevelOutput);
        }
        catch (typename Py8Collider<PythiaT,EventT>::InitializationError& e)
        {
          // Append new seed to override the previous one
          int newSeedBase = int(Random::draw() * 899990000.);
          pythiaOptions.push_back("Random:seed = " + std::to_string(newSeedBase));
          try
          {
            result.init(pythia_doc_path, pythiaOptions, &slha, processLevelOutput);
          }
          catch (typename Py8Collider<PythiaT,EventT>::InitializationError& e)
          {
            #ifdef COLLIDERBIT_DEBUG
              cout << DEBUG_PREFIX << "Py8Collider::InitializationError caught in getPy8Collider. Will discard this point." << endl;
            #endif
            piped_invalid_point.request("Bad point: Pythia can't initialize");
            wrapup();
            return;
          }
        }

        // Should we apply the xsec veto and skip event generation?

        // - Get the upper limt xsec as estimated by Pythia
        code = -1;
        nxsec = 0;
        totalxsec = 0.;
        while(true)
        {
          std::getline(processLevelOutput, readline);
          std::istringstream issPtr(readline);
          issPtr.seekg(47, issPtr.beg);
          issPtr >> code;
          if (!issPtr.good() && nxsec > 0) break;
          issPtr >> _junk >> xsec;
          if (issPtr.good())
          {
            totalxsec += xsec;
            nxsec++;
          }
        }

        #ifdef COLLIDERBIT_DEBUG
          cout << DEBUG_PREFIX << "totalxsec [fb] = " << totalxsec * 1e12 << ", veto limit [fb] = " << xsec_veto_fb << endl;
        #endif

        // - Check for NaN xsec
        if (Utils::isnan(totalxsec))
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << DEBUG_PREFIX << "Got NaN cross-section estimate from Pythia." << endl;
          #endif
          piped_invalid_point.request("Got NaN cross-section estimate from Pythia.");
          wrapup();
          return;
        }

        // - Wrap up loop if veto applies
        if (totalxsec * 1e12 < xsec_veto_fb)
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << DEBUG_PREFIX << "Cross-section veto applies. Will now call Loop::wrapup() to skip event generation for this collider." << endl;
          #endif
          wrapup();
        }

      }

    }



    /// Retrieve a specific Pythia hard-scattering Monte Carlo simulation
    #define IS_SUSY true
    #define NOT_SUSY false
    #define GET_SPECIFIC_PYTHIA(NAME, PYTHIA_NS, SPECTRUM, MODEL_EXTENSION, SUSY_FLAG)\
    void NAME(Py8Collider<PYTHIA_NS::Pythia8::Pythia,                                 \
                          PYTHIA_NS::Pythia8::Event> &result)                         \
    {                                                                                 \
      using namespace Pipes::NAME;                                                    \
                                                                                      \
      static SLHAstruct slha;                                                         \
      static SLHAstruct slha_spectrum;                                                \
                                                                                      \
      if (*Loop::iteration == BASE_INIT)                                              \
      {                                                                               \
        /* SLHAea object constructed from dependencies on the spectrum and decays. */ \
        slha.clear();                                                                 \
        slha_spectrum.clear();                                                        \
        slha = Dep::decay_rates->getSLHAea(2);                                        \
        /* SLHAea in SLHA2 format, please. */                                         \
        slha_spectrum = Dep::SPECTRUM->getSLHAea(2);                                  \
        slha.insert(slha.begin(), slha_spectrum.begin(), slha_spectrum.end());        \
        if (SUSY_FLAG)                                                                \
        {                                                                             \
          if(slha.find("MODSEL") == slha.end())                                       \
          {                                                                           \
            SLHAea::Block block("MODSEL");                                            \
            block.push_back("BLOCK MODSEL              # Model selection");           \
            SLHAea::Line line;                                                        \
            line << 1 << 0 << "# Tell Pythia that this is a SUSY model.";             \
            block.push_back(line);                                                    \
            slha.push_front(block);                                                   \
          }                                                                           \
        }                                                                             \
      }                                                                               \
                                                                                      \
      getPy8Collider(result, *Dep::RunMC, slha, #MODEL_EXTENSION,                     \
        *Loop::iteration, Loop::wrapup, *runOptions);                                 \
    }


    /// Retrieve a specific Pythia hard-scattering Monte Carlo simulation
    /// from reading a SLHA file rather than getting a Spectrum + DecayTable
    #define GET_SPECIFIC_PYTHIA_SLHA(NAME, PYTHIA_NS, MODEL_EXTENSION)                \
    void NAME(Py8Collider<PYTHIA_NS::Pythia8::Pythia,                                 \
                          PYTHIA_NS::Pythia8::Event> &result)                         \
    {                                                                                 \
      using namespace Pipes::NAME;                                                    \
      static SLHAstruct slha;                                                         \
                                                                                      \
      if (*Loop::iteration == COLLIDER_INIT)                                          \
      {                                                                               \
        const pair_str_SLHAstruct& filename_content_pair = *Dep::SLHAFileNameAndContent; \
        if (filename_content_pair.first.empty())                                      \
        {                                                                             \
          piped_invalid_point.request("Got empty SLHA filename. Will invalidate point."); \
        }                                                                             \
      }                                                                               \
                                                                                      \
      getPy8Collider(result, *Dep::RunMC, Dep::SLHAFileNameAndContent->second,        \
        #MODEL_EXTENSION, *Loop::iteration, Loop::wrapup, *runOptions);               \
    }


    /// Get a specific Pythia hard-scattering sim as a generator-independent pointer-to-BaseCollider
    #define GET_PYTHIA_AS_BASE_COLLIDER(NAME)           \
    void NAME(const BaseCollider* &result)              \
    {                                                   \
      result = &(*Pipes::NAME::Dep::HardScatteringSim); \
    }                                                   \

  }

}
