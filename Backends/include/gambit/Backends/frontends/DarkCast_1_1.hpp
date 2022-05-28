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

#ifdef HAVE_PYBIND11
  // //** Function handle for version 1 and 3 of the source code file
  
  BE_CONV_FUNCTION(dark_photon_decay_width, double, (double, std::vector<std::string>, double), "dark_photon_decay_width");
  BE_CONV_FUNCTION(dark_photon_bfrac, double, (double, std::vector<std::string>), "dark_photon_branching_fraction");

  // //** Function handle for version 2 of the source code file

  // BE_CONV_FUNCTION(get_dark_photon_instance, pybind11::object, (), "get_dark_photon_instance");
  // BE_CONV_FUNCTION(dark_photon_decay_width, double, (pybind11::object&, double, std::vector<std::string>, double), "dark_photon_decay_width");
  // BE_CONV_FUNCTION(dark_photon_bfrac, double, (pybind11::object, double, std::vector<std::string>), "dark_photon_branching_fraction");
#endif

#include "gambit/Backends/backend_undefs.hpp"