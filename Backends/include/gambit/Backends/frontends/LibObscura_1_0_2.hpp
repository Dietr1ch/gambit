//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Fronted header for the obscura backend
///
///  Compile-time registration of available
///  functions and variables from this backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Timon Emken
///          (timon.emken@fysik.su.se)
///  \date 2022 Nov

#define BACKENDNAME LibObscura
#define BACKENDLANG CXX
#define VERSION 1.0.2
#define SAFE_VERSION 1_0_2
#define REFERENCE Emken:2021uzb

LOAD_LIBRARY

// Syntax for BE_FUNCTION:
//  BE_FUNCTION([choose function name], [type], [arguement types], "[exact symbol name]", "[choose capability name]")

 BE_FUNCTION(FractionalDays, double, (int,int,int,double,double, double), "_ZN7obscura27Fractional_Days_since_J2000Eiiiddd", "FractionalDays")

/* Syntax for BE_VARIABLE:
 * BE_VARIABLE([name], [type], "[exact symbol name]", "[choose capability name]")
 * */

 BE_VARIABLE(ObscuraMeter, double, "_ZN10libphysica13natural_units5meterE", "ObscuraMeter")

// Initialisation function (definition)
BE_INI_FUNCTION{} END_BE_INI_FUNCTION

 // Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"