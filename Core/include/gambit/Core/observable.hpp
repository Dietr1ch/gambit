//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Observable class for holding ObsLike entries.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (patrickcolinscott@gmail.com)
///  \date 2022 Nov
///
///  *********************************************

#pragma once

#include "gambit/Core/rule.hpp"

#include "yaml-cpp/yaml.h"


namespace Gambit
{

  namespace DRes
  {

    struct Observable
    {
      std::string purpose;
      std::string capability;
      std::string type;
      std::string function;
      std::string module;

      /// Instruction to printer as to whether to write result to disk
      bool printme;

      YAML::Node subcaps;

      ///Default constructor, to ensure the default values are not gibberish
      Observable():
        purpose(),
        capability(),
        type(),
        function(),
        module(),
        printme(true),
        subcaps()
      {}
    };

  }

}
