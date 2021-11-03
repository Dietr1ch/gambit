//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header for the Acropolis backend
///
///  Compile-time registration of available
///  functions and variables from this backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Patrick Stoecker
///          (patrick.stoecker@kit.edu)
///  \date 2021 Oct
///
///  *********************************************

#define BACKENDNAME Acropolis
#define BACKENDLANG Python3
#define VERSION 1.2.0
#define SAFE_VERSION 1_2_0
#define REFERENCE Depta:2020mhj,Depta:2020zbh,Hufnagel:2018bjp

LOAD_LIBRARY

#ifdef HAVE_PYBIND11

  BE_CONV_FUNCTION(abundance_photodissociation_decay, void,(double*,double*,double,double,double,double,double), "abundance_photodissociation_decay")

#endif

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
