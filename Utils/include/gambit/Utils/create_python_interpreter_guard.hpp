//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Header that declares a pointer that ensures
///  the python interpreter has been started.
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

#include "gambit/Utils/python_interpreter.hpp"

namespace Gambit
{

  namespace Utils
  {

    const python_interpreter* guard = &(python_interpreter::get());
  
  }
    
}

#endif
