//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Class for holding a rule to be applied during
///  dependency resolution.
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

#include "gambit/Core/yaml_parser.hpp"
#include "gambit/Core/resolution_utilities.hpp"
#include "gambit/Utils/util_types.hpp"


namespace Gambit
{

  namespace DRes
  {

    /// A simple rule for dependency resolution (aka constraints on module and
    /// function name).
    struct Rule
    {
      Rule(std::string function, std::string module) : function(function), module(module) {}
      Rule(IniParser::ObservableType);
      std::string capability;
      std::string type;
      std::string function;
      std::string module;
      std::string backend;
      std::string version;
      Options options;
    };

    /// Check whether quantity matches observableType
    /// Matches capability and type
    bool quantityMatchesIniEntry(const sspair & quantity, const IniParser::ObservableType & observable, const Utils::type_equivalency & eq);

    /// Check whether quantity matches observableType
    /// Matches capability
    bool capabilityMatchesIniEntry(const sspair & quantity, const IniParser::ObservableType & observable);

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and module name
    bool moduleFuncMatchesIniEntry(functor *f, const IniParser::ObservableType &e, const Utils::type_equivalency & eq);

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and backend name
    bool backendFuncMatchesIniEntry(functor *f, const IniParser::ObservableType &e, const Utils::type_equivalency & eq);

    /// Get entry level relevant for options
    int getEntryLevelForOptions(const IniParser::ObservableType &e);

    /// Check whether functor matches rules
    /// Matches function name and type
    bool matchesRules( functor *f, const Rule & rule);

    /// Find entries (comparison of inifile entry with quantity or functor)
    /// @{
    const IniParser::ObservableType * findIniEntry(
        sspair quantity, const IniParser::ObservablesType &, const str &);
    const IniParser::ObservableType * findIniEntry(
        functor* f, const IniParser::ObservablesType &, const str &, const Utils::type_equivalency &);
    /// @}

  }
}
