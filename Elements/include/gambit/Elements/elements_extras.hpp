//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions that require printing capabilities
///  that can be run from within module functions.
///  TODO: Perhaps move postprocessor retrieve back
///        into DarkBit, and change this file to be
///        purely for suspicious points
///
///  Classes added:
///  - Suspicious point exception class.
///
///  Authors:
///
///  \author Chris Chang          
///  \date Dec 2023
///
///  *********************************************

#ifndef __elements_extras_hpp__
#define __elements_extras_hpp__

#include <map>
#include <set>
#include <string>
#include <exception>
#include <vector>
#include <utility>

#include "gambit/Utils/util_macros.hpp"
#include "gambit/Logs/log_tags.hpp"
#include "gambit/Printers/baseprinter.hpp"
#include "gambit/Printers/printermanager.hpp"

namespace Gambit
{

  /// Gambit suspicious point exception class.
  class Suspicious_point_exception
  {

    public:

      /// Constructor
      Suspicious_point_exception() {}

      /// Raise the suspicious point exception. Print it with a message and a code. The default code is 1.
      void raise(const std::string &msg, int code=1, bool debug=false)
      {
        // get the printer pointer
        Printers::BasePrinter& printer = *(get_global_printer_manager()->printerptr);
        printer.print(code, "Suspicious Point Code", Printers::get_main_param_id("Suspicious Point Code"), printer.getRank(), Printers::get_point_id());
      
        if (debug) std::cout << "Point Suspicious (" << code << "): " << msg << std::endl;
      }

  };

  /// Postprocessor reader retrieve function
  template<typename T>
  bool pp_reader_retrieve(T& result, str dataset)
  {
    return get_pp_reader().retrieve(result, dataset);
  }
}

#endif
