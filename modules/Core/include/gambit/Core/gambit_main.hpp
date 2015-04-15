//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Header for main GAMBIT executable.
///
///  *********************************************
///
///  Authors:
///   
///  \author The GAMBIT Collaboration
///  \date 2012 Oct --> 
///
///  *********************************************

#ifndef __gambit_main_hpp__
#define __gambit_main_hpp__

#include "gambit/Utils/static_members.hpp"
#include "gambit/Utils/stream_overloads.hpp"
#include "gambit/Utils/equivalency_singleton.hpp"
#include "gambit/Backends/backend_rollcall.hpp"
#include "gambit/Core/depresolver.hpp"
#include "gambit/Core/yaml_parser.hpp"
#include "gambit/Core/likelihood_container.hpp"
#include "gambit/Core/module_rollcall.hpp"
#include "gambit/Logs/log.hpp"
#include "gambit/Printers/printermanager.hpp"
#include "gambit/Models/model_rollcall.hpp"
#include "gambit/ScannerBit/scannerbit.hpp"
#include "gambit/ScannerBit/priors_rollcall.hpp"

//#include "gambit/Printers/printer_rollcall.hpp" // Load up all the available printers


#endif //__gambit_main_hpp__

