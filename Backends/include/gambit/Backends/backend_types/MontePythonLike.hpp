//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Declarations of container classes
///  for the MontePythonLike backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Janina Renk
///          (janina.renk@fysik.su.se)
///  \date 2019 Jun
///
///  \author Pat Scott
///          (pat.scott@uq.edu.au)
///  \date 2020 Apr
///
///  *********************************************

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

namespace Gambit
{

  /// Shorthand for a string to pybind object map map to avoid commas in macros
  typedef std::map<std::string,pybind11::object> map_str_pyobj;

  /// Class holding MPLike data structure & map with initialised Likelihoods objects; this is
  /// separated form the Classy_cosmo_container since it needs to be initialised as 'static const'
  /// such that the initialisation and reading in of data only happens once.
  /// This is essential since the parsing of the data at initialisation of a Likelihood object can take
  /// much longer than the actual Likelihood calculation.
  class MPLike_data_container
  {
    public:

      MPLike_data_container() {}
      MPLike_data_container(pybind11::object &data_in, map_str_pyobj likelihoods_in): data(data_in), likelihoods(likelihoods_in){}

      /// MPLike data structure
      pybind11::object data;

      /// Map likelihood name to initialised MPLike likelihood object
      map_str_pyobj likelihoods;
  };

}
