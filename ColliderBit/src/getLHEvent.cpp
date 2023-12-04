//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Les Houches event file reader module function
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2019 May
///
///  *********************************************

#include "gambit/cmake/cmake_variables.hpp"

#include <iostream>

using namespace std;

#ifndef EXCLUDE_HEPMC

#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"
#include "gambit/ColliderBit/lhef2heputils.hpp"
#include "gambit/Utils/util_functions.hpp"

#include "gambit/Utils/begin_ignore_warnings_hepmc.hpp"
#include "HepMC3/LHEF.h"
#include "gambit/Utils/end_ignore_warnings.hpp"


namespace Gambit
{

  namespace ColliderBit
  {

    /// A nested function that reads in Les Houches Event files and converts them to HEPUtils::Event format
    void getLHEvent_HEPUtils(HEPUtils::Event& result)
    {
      using namespace Pipes::getLHEvent_HEPUtils;

      result.clear();

      // Get yaml options and initialise the LHEF reader
      const static double jet_pt_min = runOptions->getValueOrDef<double>(10.0, "jet_pt_min");
      const static str lhef_filename = runOptions->getValue<str>("lhef_filename");
      static bool first = true;
      if (first)
      {
        if (not Utils::file_exists(lhef_filename)) throw std::runtime_error("LHE file "+lhef_filename+" not found.  Quitting...");
        first = false;
      }
      static LHEF::Reader lhe(lhef_filename);

      // Get all jet collection settings
      str jetcollection_taus;
      std::vector<jet_collection_settings> all_jet_collection_settings = {}; // Initialise with no collections, if no jet collection setting specified, then should use the basecollider's initialised jet collection
      if (runOptions->hasKey((*Dep::RunMC).current_collider()))
      {
        YAML::Node colNode = runOptions->getValue<YAML::Node>((*Dep::RunMC).current_collider());
        Options colOptions(colNode);

        // Fill the jet collection settings
        if (colOptions.hasKey("jet_collections"))
        {
          YAML::Node all_jetcollections_node = colOptions.getValue<YAML::Node>("jet_collections");
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

          jetcollection_taus = colOptions.getValueOrDef<str>("antikt_R04", "jet_collection_taus");
          // Throw an error if the jetcollection_taus setting is not given and not using the antikt_R04 collection
          if (std::find(jetcollection_names.begin(), jetcollection_names.end(), jetcollection_taus) == jetcollection_names.end())
          {
            ColliderBit_error().raise(LOCAL_INFO,"Please provide the jet_collection_taus setting for jet collections if not using antikt_R04.");
          }
        }
      }
      else
      {
        all_jet_collection_settings = {{"antikt_R04", "antikt", 0.4, "E_scheme", "Best"}};
        jetcollection_taus = "antikt_R04";
      }

      // Don't do anything during special iterations
      if (*Loop::iteration < 0) return;

      // Attempt to read the next LHE event as a HEPUtils event. If there are no more events, wrap up the loop and skip the rest of this iteration.
      bool event_retrieved = true;
      #pragma omp critical (reading_LHEvent)
      {
        if (lhe.readEvent()) get_HEPUtils_event(lhe, result, jet_pt_min, all_jet_collection_settings);
        else event_retrieved = false;
      }
      if (not event_retrieved)
      {
        // Tell the MCLoopInfo instance that we have reached the end of the file
        Dep::RunMC->report_end_of_event_file();
        Loop::halt();
      }

    }

  }

}

#endif
