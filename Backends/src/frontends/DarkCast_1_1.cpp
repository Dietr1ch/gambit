//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend for DarkCast 1.1 backend
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Quan Huynh
///          (qhuy0003@student.monash.edu)
///  \date 2022 Apr
///
///  *********************************************


#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/DarkCast_1_1.hpp"

//** VERSION 1: THE ZOOMIE ONE
#ifdef HAVE_PYBIND11

  #include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
  #include <pybind11/stl.h>
  #include <pybind11/stl_bind.h>
  #include <pybind11/functional.h>
  #include "gambit/Utils/end_ignore_warnings.hpp"
  #include <pybind11/embed.h>
  #include <cstring>
  
  BE_NAMESPACE
  {
    namespace py = pybind11;

    // py::scoped_interpreter guard{};
    static py::object dark_photon;

    //I think using BE_INI_FUNCTION would be better but I don't know how it works
    void dark_photon_ini()
    {
      std::string model_name = "dark_photon";
      // I hope this give me the darkcast module
      // std::string module_name = backendDir.append(".model");
      // py::module dark_cast_module = py::module::import(module_name.c_str()); 

      dark_photon = DarkCast.attr("Model")(model_name);
    }

    double dark_photon_decay_width(double mass, std::vector<std::string> states, double k)
    {
      if (dark_photon == NULL) 
      {
        dark_photon_ini();
      }

      double width = dark_photon.attr("width")(states, mass, k).cast<double>();
      return width;
    }

    double dark_photon_bfrac(double mass, std::vector<std::string> states)
    {
      if (dark_photon == NULL)
      {
        dark_photon_ini();
      }

      double bfrac = dark_photon.attr("bfrac")(states, mass).cast<double>();
      return bfrac;
    }
  }
  END_BE_NAMESPACE

#endif

BE_INI_FUNCTION{}
END_BE_INI_FUNCTION

// //** Version 2: Also very fast but require to run pybind11:scoped_interpreter guard{}; before
// // calling the functions
// #ifdef HAVE_PYBIND11

//   #include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
//   #include <pybind11/stl.h>
//   #include <pybind11/stl_bind.h>
//   #include <pybind11/functional.h>
//   #include "gambit/Utils/end_ignore_warnings.hpp"
//   #include <pybind11/embed.h>
  
//   BE_NAMESPACE
//   {
//     namespace py = pybind11;
//     //I think using BE_INI_FUNCTION would be better but I don't know how it works
//     py::object get_dark_photon_instance()
//     {
//       std::string model_name = "dark_photon";
//       // I hope this give me the darkcast module
//       py::module_ dark_cast_module = py::module_::import(backendDir); 
//       py::object dark_photon = dark_cast_module.attr("Model")(model_name);

//       return dark_photon;
//     }

//     double dark_photon_decay_width(py::object& dark_photon, double mass, std::vector<std::string> states, double k)
//     {
//       double width = dark_photon.attr("width")(states, mass, k).cast<double>();
//       return width;
//     }

//     double dark_photon_bfrac(py::object& dark_photon, double mass, std::vector<std::string> states)
//     {
//       double bfrac = dark_photon.attr("bfrac")(states, mass).cast<double>();
//       return bfrac;
//     }
//   }
// #endif

// //** Version 3: The slow one
// #ifdef HAVE_PYBIND11

//   #include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
//   #include <pybind11/stl.h>
//   #include <pybind11/stl_bind.h>
//   #include <pybind11/functional.h>
//   #include "gambit/Utils/end_ignore_warnings.hpp"
//   #include <pybind11/embed.h>
  
//   BE_NAMESPACE
//   {
//     namespace py = pybind11;

//     double dark_photon_decay_width(double mass, std::vector<std::string> states, double k)
//     {
//       py::scoped_interpreter guard{};
//       std::string model_name = "dark_photon";
//       // I hope this give me the darkcast module
//       py::module_ dark_cast_module = py::module_::import(backendDir); 
//       dark_photon = dark_cast_module.attr("Model")(model_name);

//       double width = dark_photon.attr("width")(states, mass, k).cast<double>();
//       return width;
//     }

//     double dark_photon_bfrac(double mass, std::vector<std::string> states)
//     {
//       py::scoped_interpreter guard{};
//       std::string model_name = "dark_photon";
//       // I hope this give me the darkcast module
//       py::module_ dark_cast_module = py::module_::import(backendDir); 
//       dark_photon = dark_cast_module.attr("Model")(model_name);

//       double bfrac = dark_photon.attr("bfrac")(states, mass).cast<double>();
//       return bfrac;
//     }
//   }
// #endif