//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Singleton class for holding the python 
///  interpreter instance.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (patrickcolinscott@gmail.com)
///  \date 2023 Jun
///
///  *********************************************

#pragma once

#ifdef HAVE_PYBIND11

#include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
#include <pybind11/pybind11.h>
#include "gambit/Utils/end_ignore_warnings.hpp"


namespace pybind11
{
  class scoped_interpreter;
}

namespace Gambit
{

  namespace Utils
  {

    /// A singleton class that holds a pointer to the python interpreter. 
    class EXPORT_SYMBOLS python_interpreter
    {
    
      public:
    
        /// Singleton accessor function 
        static python_interpreter& get();
  
        /// sys module loaded at construction time
        pybind11::module* sys;
    
        /// os module loaded at construction time
        pybind11::module* os;
  
      private:
    
        /// Regular constructor. No access as it is private.
        python_interpreter();
        
        /// Regular destructor. No access as it is private.
        ~python_interpreter();
  
        /// Shut off access to copy constructor by making it private
        python_interpreter (const python_interpreter&);
  
        /// Shut off access to assignment operator by making it private 
        const python_interpreter& operator= (const python_interpreter&);
    
        /// Pointer to the Python interpreter
        pybind11::scoped_interpreter* iptr;
    
    };
  
    /// Just some syntactic sugar 
    void python_interpreter_guard();
    
  }

}

#endif
