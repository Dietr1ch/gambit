//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  HepMC event file reader module function
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Andy Buckley
///          (andy.buckley@cern.ch)
///  \date 2019 June
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2019 June
///
///  \author Anders Kvellestad
///          (a.kvellestad@imperial.ac.uk)
///  \date 2019 June
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 Sep, Oct
///  \date 2020 Apr
///
///  \author Tomek Procter
///           (t.procter.1@research.gla.ac.uk)
///  \date 2019 October
///  \date 2021 November
///
///  \author Yang Zhang
///           (tsp116@ic.ac.uk)
///  \date 2020 June
///
///  *********************************************

#include "gambit/cmake/cmake_variables.hpp"

#ifndef EXCLUDE_HEPMC

#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"
#include "gambit/Utils/util_functions.hpp"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "gambit/ColliderBit/colliders/Pythia8/Py8EventConversions.hpp"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/ReaderAscii.h"

#define DEBUG_PREFIX "DEBUG: OMP thread " << omp_get_thread_num() << ":  "
//#define COLLIDERBIT_DEBUG

namespace Gambit
{

  namespace ColliderBit
  {

    /// Storage of different FastJet methods
    FJNS::JetAlgorithm FJalgorithm_map(str algorithm)
    {
      FJNS::JetAlgorithm result;
      if (algorithm == "antikt") {result = FJNS::antikt_algorithm;}
      else if (algorithm == "cambridge") {result = FJNS::cambridge_algorithm;}
      else if (algorithm == "kt") {result = FJNS::kt_algorithm;}
      else if (algorithm == "genkt") {result = FJNS::genkt_algorithm;}
      else if (algorithm == "cambridge_for_passive") {result = FJNS::cambridge_for_passive_algorithm;}
      else
      {
        ColliderBit_error().raise(LOCAL_INFO, "Could not find jet algorithm in list available. Please add the missing option to the FJalgorithm_map function in Py8EventConversions.hpp.");
      }
      return result;
    }

    FJNS::Strategy FJstrategy_map(str strategy)
    {
      FJNS::Strategy result;
      if (strategy == "Best") {result = FJNS::Best;}
      else if (strategy == "NlnN") {result = FJNS::NlnN;}
      else
      {
        ColliderBit_error().raise(LOCAL_INFO, "Could not find jet strategy in list available. Please add the missing option to the FJstrategy_map function in Py8EventConversions.hpp.");
      }
      return result;
    }

    FJNS::RecombinationScheme FJRecomScheme_map(str reco_scheme)
    {
      FJNS::RecombinationScheme result;
      if (reco_scheme == "E_scheme") {result = FJNS::E_scheme;}
      else if (reco_scheme == "pt_scheme") {result = FJNS::pt_scheme;}
      else if (reco_scheme == "pt2_scheme") {result = FJNS::pt2_scheme;}
      else
      {
        ColliderBit_error().raise(LOCAL_INFO, "Could not find jet recombination scheme in list available. Please add the missing option to the FJRecomScheme_map function in Py8EventConversions.hpp.");
      }
      return result;
    }

    /// A nested function that reads in HepMC event files
    void readHepMCEvent(HepMC3::GenEvent& result, const str HepMC_filename,
                        const MCLoopInfo& RunMC, const int iteration,
                        void(*halt)())
    {
      result.clear();

      // Initialise the HepMC reader
      static int HepMC_file_version = -1;

      static bool first = true;
      if (first)
      {
        if (not Utils::file_exists(HepMC_filename)) throw std::runtime_error("HepMC event file "+HepMC_filename+" not found. Quitting...");

        // Figure out if the file is HepMC2 or HepMC3
        std::ifstream infile(HepMC_filename);
        if (infile.good())
        {
          std::string line;
          while(std::getline(infile, line))
          {
            // Skip blank lines
            if(line == "") continue;

            // We look for "HepMC::Version 2" or "HepMC::Version 3",
            // so we only need the first 16 characters of the line
            std::string short_line = line.substr(0,16);

            if (short_line == "HepMC::Version 2")
            {
              HepMC_file_version = 2;
              break;
            }
            else if (short_line == "HepMC::Version 3")
            {
              // Check the text format
              std::getline(infile, line);
              std::string text_format = line.substr(0,14);
              if (text_format == "HepMC::Asciiv3")
              {
                HepMC_file_version = 3;
                break;
              }
              else if (text_format == "HepMC::IO_GenE")
              {
                HepMC_file_version = 2;
                break;
              }
              else
              {
                std::stringstream msg;
                msg <<  "Could not determine HepMC version from the string '" << text_format << "' extracted from the line '" << line << "'. Quitting...";
                ColliderBit_error().raise(LOCAL_INFO, msg.str());
              }
            }
            else
            {
              std::stringstream msg;
              msg << "Could not determine HepMC version from the string '" << short_line << "' extracted from the line '" << line << "'. Quitting...";
              ColliderBit_error().raise(LOCAL_INFO, msg.str());
            }
          }
        }
        first = false;
      }

      if(HepMC_file_version != 2 and HepMC_file_version != 3)
      {
        std::stringstream msg;
        msg << "Failed to determine HepMC version for input file " << HepMC_filename << ". Quitting...";
        ColliderBit_error().raise(LOCAL_INFO, msg.str());
      }

      static HepMC3::Reader *HepMCio;

      // Initialize the reader on the first iteration
      if (iteration == BASE_INIT)
      {
        if (HepMC_file_version == 2)
        {
          HepMCio = new HepMC3::ReaderAsciiHepMC2(HepMC_filename);
        }
        else
        {
          HepMCio = new HepMC3::ReaderAscii(HepMC_filename);
        }
      }

      // Delete the reader in the last iteration
      if (iteration == BASE_FINALIZE)
        delete HepMCio;

      // Don't do anything else during special iterations
      if (iteration < 0) return;

      #ifdef COLLIDERBIT_DEBUG
        cout << DEBUG_PREFIX << "Event number: " << iteration << endl;
      #endif

      // Attempt to read the next HepMC event. If there are no more events, wrap up the loop and skip the rest of this iteration.
      bool event_retrieved = true;
      #pragma omp critical (reading_HepMCEvent)
      {
        event_retrieved = HepMCio->read_event(result);

        // FIXME This is a temp solution to ensure that the event reading
        //       stops when there are no more events in the HepMC file.
        //       Remove this once bugfix is implemented in HepMC.
        if ((result.particles().size() == 0) && (result.vertices().size() == 0)) event_retrieved = false;
      }
      if (not event_retrieved)
      {
        // Tell the MCLoopInfo instance that we have reached the end of the file
        RunMC.report_end_of_event_file();
        halt();
      }
      if (not event_retrieved) halt();

   }
    /// A nested function that reads in HepMC event files
    void getHepMCEvent(HepMC3::GenEvent& result)
    {
      using namespace Pipes::getHepMCEvent;

      // Get yaml options
      const static str HepMC_filename = runOptions->getValueOrDef<str>("", "hepmc_filename");

      // Get the HepMC event
      readHepMCEvent(result, HepMC_filename, *Dep::RunMC, *Loop::iteration, Loop::halt);

    }


    /// A helper function for collecting the jet_collections yaml settings. 
    /// Used by functions getHepMCEvent_HEPUtils and convertHepMCEvent_HEPUtils.
    void read_jet_collections_settings(const Options& runOptions, std::vector<jet_collection_settings>& all_jet_collection_settings, str& jetcollection_taus)
    {
      if (runOptions.hasKey("jet_collections"))
      {
        YAML::Node all_jetcollections_node = runOptions.getValue<YAML::Node>("jet_collections");
        Options all_jetcollection_options(all_jetcollections_node);

        str algorithm;
        double R;
        str recombination_scheme;
        str strategy;
        std::vector<str> jetcollection_names = all_jetcollection_options.getNames();

        for (str key : jetcollection_names)
        {
          YAML::Node current_jc_node = all_jetcollection_options.getValue<YAML::Node>(key);
          Options current_jc_options(current_jc_node);

          algorithm = current_jc_options.getValueOrDef<str>("antikt", "algorithm");
          R = current_jc_options.getValueOrDef<double>(0.4, "R");
          recombination_scheme = current_jc_options.getValueOrDef<str>("E_scheme", "recombination_scheme");
          strategy = current_jc_options.getValueOrDef<str>("Best", "strategy");

          all_jet_collection_settings.push_back({key, algorithm, R, recombination_scheme, strategy});
        }

        jetcollection_taus = runOptions.getValueOrDef<str>("antikt_R04", "jet_collection_taus");
        // Throw an error if the jetcollection_taus setting is not given and not using the antikt_R04 collection
        if (std::find(jetcollection_names.begin(), jetcollection_names.end(), jetcollection_taus) == jetcollection_names.end())
        {
          ColliderBit_error().raise(LOCAL_INFO,"Please provide the jet_collection_taus setting for jet collections if not using antikt_R04.");
        }
      }
      else
      {
        all_jet_collection_settings = {{"antikt_R04", "antikt", 0.4, "E_scheme", "Best"}};
        jetcollection_taus = "antikt_R04";
      }
    }


    /// A nested function that reads in HepMC event files and converts them to HEPUtils::Event format
    void getHepMCEvent_HEPUtils(HEPUtils::Event &result)
    {
      using namespace Pipes::getHepMCEvent_HEPUtils;

      // Get yaml options
      const static str HepMC_filename = runOptions->getValueOrDef<str>("", "hepmc_filename");
      const static double jet_pt_min = runOptions->getValueOrDef<double>(10.0, "jet_pt_min");
      std::vector<jet_collection_settings> all_jet_collection_settings = {};
      str jetcollection_taus;
      read_jet_collections_settings(*runOptions, all_jet_collection_settings, jetcollection_taus);

      // Get the HepMC event
      //HepMC3::GenEvent ge = *Dep::HardScatteringEvent;
      HepMC3::GenEvent ge;
      readHepMCEvent(ge, HepMC_filename, *Dep::RunMC, *Loop::iteration, Loop::halt);

      //We need to not do anything else on special iterations, where an event has not actually been extracted:
      if (*Loop::iteration < 0) return;

      //Set the weight
      result.set_weight(ge.weight());

      //Translate to HEPUtils event by calling the unified HEPMC/Pythia event converter:
      Gambit::ColliderBit::convertParticleEvent(ge.particles(), result, all_jet_collection_settings, jetcollection_taus, jet_pt_min);

    }

    void convertHepMCEvent_HEPUtils(HEPUtils::Event &result)
    {
      using namespace Pipes::convertHepMCEvent_HEPUtils;

      //Don't do anything on special iterations: you'll just end up dereferencing a nullptr
      if (*Loop::iteration < 0) return;

      //HepMC Event should just be sitting waiting for us.
      HepMC3::GenEvent ge = *Dep::HardScatteringEvent;

      //Get yaml options required for conversion
      const static double jet_pt_min = runOptions->getValueOrDef<double>(10.0, "jet_pt_min");
      std::vector<jet_collection_settings> all_jet_collection_settings = {};
      str jetcollection_taus;
      read_jet_collections_settings(*runOptions, all_jet_collection_settings, jetcollection_taus);

      //Set the weight
      result.set_weight(ge.weight());

      //Translate to HEPUtils event by calling the unified HEPMC/Pythia event converter:
      Gambit::ColliderBit::convertParticleEvent(ge.particles(), result, all_jet_collection_settings, jetcollection_taus, jet_pt_min);
    }

  }
}

#endif
