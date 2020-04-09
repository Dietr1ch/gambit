//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header for the MontePython backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Janina Renk
///          (janina.renk@fysik.su.se)
///  \date 2019 June
///
///  \author Sanjay Bloor
///          (sanjay.bloor12@imperial.ac.uk)
///  \date 2019 June
///
///  \author Pat Scott
///          (pat.scott@uq.edu.au)
///  \date 2020 Apr
///
///  *********************************************

#define BACKENDNAME MontePythonLike
#define BACKENDLANG Python2
#define VERSION 3.1.0
#define SAFE_VERSION 3_1_0

LOAD_LIBRARY

BE_CONV_FUNCTION(get_MP_available_likelihoods,  std::vector<str>, (), "get_MP_available_likelihoods")
BE_CONV_FUNCTION(get_MP_loglike, double, (const MPLike_data_container&, pybind11::object&, std::string&), "get_MP_loglike")
BE_CONV_FUNCTION(create_MP_data_object, pybind11::object, (map_str_str&), "create_MP_data_object")
BE_CONV_FUNCTION(create_MP_likelihood_objects,  map_str_pyobj, (pybind11::object&, map_str_str&), "create_MP_likelihood_objects")

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
