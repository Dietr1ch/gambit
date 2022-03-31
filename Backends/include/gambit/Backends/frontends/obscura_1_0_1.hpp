//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header for the obscura backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Timon Emken
///          (timon.emken@fysik.su.se)
///  \date 2022 Mar
///
///  *********************************************

// Identify backend
#define BACKENDNAME obscura_1_0_2
#define BACKENDLANGUAGE CXX
#define VERSION 1.0.2
#define SAFE_VERSION 1_0_2
#define REFERENCE Emken:2021uzb

// Begin
LOAD_LIBRARY

// Allow for models
BE_ALLOW_MODELS()

// BE_VARIABLE(MyVar, int, ("myvar_symbol"),"MyVar_Cap")
// BE_FUNCTION(MyFunc, void, (double&), ("myfunc_symbol"),"MyFunc_Cap")

// END
#include "gambit/Backends/backend_undefs.hpp"