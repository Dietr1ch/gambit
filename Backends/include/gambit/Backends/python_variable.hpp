//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Class python_variable, needed to overload
///  constructor and assignment operators to send
///  messages through pybind11.
///
///  ***********************************************
///
///  Authors
///  =======
///
///  (add name and date if you modify)
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2017 Dec
///
///  ***********************************************

#ifndef __python_variable_hpp__
#define __python_variable_hpp__

#include "gambit/Elements/ini_catch.hpp"
#include "gambit/Backends/backend_singleton.hpp"
#include "gambit/cmake/cmake_variables.hpp"

#ifdef HAVE_PYBIND11
  #include <pybind11/pybind11.h>
#endif

namespace Gambit
{

  /// Holds the info about a python backend variable, and defines conversion functions.
  template <typename TYPE>
  class python_variable
  {

    private:

      #ifdef HAVE_PYBIND11

        /// Wrapper for the Python dictionary of internal module variables
        pybind11::dict _dict;

        /// Name of the variable inside the Python module
        str _symbol;

        /// Indication of whether or not the function has been successfully loaded
        bool handle_works;

      #endif

    public:

      /// Constructor
      python_variable(const str& be, const str& ver, const str& symbol) : _symbol(symbol), handle_works(false)
      {
        #ifdef HAVE_PYBIND11
          using namespace Backends;
          try
          {
            // Extract the backend module pointer from the backendInfo object
            pybind11::module* mod;
            if (backendInfo().works.at(be+ver))
            {
              mod = backendInfo().loaded_python_backends.at(be+ver);
            }
            else
            {
              mod = (pybind11::module*)0;
              return;
            }

            // Extract the wrapper to the module's internal dictionary
            try
            {
              _dict = mod->attr("__dict__");
              handle_works = true;
            }
            catch (std::exception& e)
            {
              std::ostringstream err;
              err << "Failed to retrieve handle to dictionary (containing variable " << _symbol << ") from Python module for " << be+ver << endl
                  << "The backend variable associated with this symbol will be disabled (i.e. get status = -2)." << endl
                  << "Python error was: " << e.what() << endl;
              backend_warning().raise(LOCAL_INFO, err.str());
              backendInfo().dlerrors[be+ver] = _symbol;
            }
          }
          catch (std::exception& e) { ini_catch(e); }
        #endif
      }

      /// Assignment operator for python_variable from equivalent C++ type
      python_variable& operator=(const TYPE& val)
      {
        #ifdef HAVE_PYBIND11
          if (not handle_works) backend_error().raise(LOCAL_INFO, "Attempted to use a Python backend variable that was not successfully loaded.");
          _dict[_symbol.c_str()] = val;
          return *this;
        #else
          backend_error().raise(LOCAL_INFO, "Attempted to assign a C++ type to a python_variable without pybind11.");
        #endif
      }

      /// Cast operator from python_variable to equivalent C++ type
      operator TYPE const()
      {
        #ifdef HAVE_PYBIND11
          if (not handle_works) backend_error().raise(LOCAL_INFO, "Attempted to use a Python backend variable that was not successfully loaded.");
          pybind11::object result = _dict[_symbol.c_str()];
          return result.cast<TYPE>();
        #else
          backend_error().raise(LOCAL_INFO, "Attempted to cast a python_variable to a C++ type without pybind11.");
        #endif
      }

  };

}

#endif /* __python_variable_hpp__ */
