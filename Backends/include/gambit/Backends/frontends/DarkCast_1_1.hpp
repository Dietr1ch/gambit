//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header for the DarkCast backend
///
///  Compile-time registration of available
///  functions and variables from this backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Quan Huynh
///          (qhuy0003@student.monash.edu)
///  \date 2022 Apr
///
///  *********************************************


#define BACKENDNAME DarkCast
#define BACKENDLANG Python
#define VERSION 1.1
#define SAFE_VERSION 1_1
#define REFERENCE Ilten:2018crw

LOAD_LIBRARY

//Input: coupling parameter kappa, (list of) decay states, mass [GeV] => Output: decay width [GeV]
BE_FUNCTION(dark_photon_decay_width, double, (double, std::vector<std::string>, double), "dark_photon_decay_width", "dark_photon_decay_width_capability")

//Input: (list of) decay states, mass [GeV] => Output: branching fraction [1]
BE_FUNCTION(dark_photon_branching_fraction, double, (std::vector<std::string>, double), "dark_photon_branching_fraction", "dark_photon_branching_fraction_capability")


#include "gambit/Backends/backend_undefs.hpp"