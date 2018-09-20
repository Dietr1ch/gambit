//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header for the DirectDM backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Sanjay Bloor
///          (sanjay.bloor12@imperial.ac.uk)
///  \date 2018 Sep
///
///  *********************************************

#define BACKENDNAME DirectDM
#define BACKENDLANG Python
#define VERSION 2.0.1
#define SAFE_VERSION 2_0_1

LOAD_LIBRARY

//#include "gambit/Backends/python_variable.hpp"

BE_CONV_FUNCTION(initialise_WC_dict, pybind11::dict, (std::vector<std::pair<std::string, double> >), "initialise_WC_dict")
BE_CONV_FUNCTION(get_NR_WCs_flav, pybind11::dict, (pybind11::dict, int, std::string), "get_NR_WCs_flav")
BE_CONV_FUNCTION(get_NR_WCs_EW, pybind11::dict, (pybind11::dict, double, double, double, std::string), "get_NR_WCs_EW")

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
