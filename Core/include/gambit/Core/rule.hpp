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

#include "gambit/Core/resolution_utilities.hpp"
#include "gambit/Core/observable.hpp"
#include "gambit/Utils/util_types.hpp"
#include "gambit/Utils/yaml_options.hpp"
#include "gambit/Elements/type_equivalency.hpp"


namespace Gambit
{

  namespace DRes
  {

    /// Base class rule for resolution of dependencies or backend requirements
    struct Rule
    {
      bool has_if;
      bool has_then;

      std::string capability;
      bool if_capability;
      bool then_capability;

      std::string type;
      bool if_type;
      bool then_type;

      std::string function;
      bool if_function;
      bool then_function;

      std::string version;
      bool if_version;
      bool then_version;

      /// Indicates that rule can be broken
      bool weakrule;

      ///Default constructor, to ensure the default values are not gibberish
      Rule():
        capability(),
        type(),
        function(),
        version(),
        weakrule(false)
      {}
    };

    /// Derived class rule for resolution of backend requirements
    struct BackendRule : public Rule
    {
      std::string backend;
      bool if_backend;
      bool then_backend;

      //bool matches(backend_functor*, const Utils::type_equivalency&) // whether backend functor properties match all of the non-empty fields of the rule
      //bool allows(backend_functor*, const Utils::type_equivalency&) // must be true for backend functor to be used to resolve a backend req.  check if backend functor either matches the rule or has no non-empty fields captured by the rule
      //bool backend_reqs_allow(backend_functor*, const Utils::type_equivalency&) // must be true for backend functor to be used to resolve a backend req of another module functor that matches this rule. check if backend functor is allowed by all backend req subjugate rules.

      /// Default constructor, to ensure the default values are not gibberish
      BackendRule():
        Rule(),
        backend(),
        if_backend(false),
        then_backend(false)
      {}

      /// Check if a given string is a permitted field of this class
      static bool permits_field(const str&);
    };

    /// Derived class rule for resolution of dependencies
    struct ModuleRule : public Rule
    {
      std::string module;
      bool if_module;
      bool then_module;

      Options options;
      bool then_options;

      std::vector<ModuleRule> dependencies;
      bool then_dependencies;

      std::vector<BackendRule> backends;
      bool then_backends;

      std::vector<std::string> functionChain;
      bool then_functionChain;

      //bool matches(module_functor*, const Utils::type_equivalency&) // whether module functor properties match all of the non-empty fields of the rule
      //bool allows(module_functor*, const Utils::type_equivalency&) // must be true for module functor to be used to resolve a dependency.  check if module functor either matches the rule or has no non-empty fields captured by the rule
      //bool dependencies_allow(module_functor*, const Utils::type_equivalency&) // must be true for module functor to be used to resolve a dependency of another module functor that matches this rule. check if functor is allowed by *all* the dependency subjugate rules

      ///Default constructor, to ensure the default values are not gibberish
      ModuleRule():
        Rule(),
        module(),
        if_module(false),
        then_module(false),
        options(),
        then_options(false),
        dependencies(),
        then_dependencies(false),
        backends(),
        then_backends(false),
        functionChain(),
        then_functionChain(false)
      {}

      /// Check if a given string is a permitted field of this class
      static bool permits_field(const str&);
    };


    /*
    /// Check whether quantity matches observableType
    /// Matches capability and type
    bool quantityMatchesIniEntry(const sspair & quantity, const Observable & observable, const Utils::type_equivalency & eq);

    /// Check whether quantity matches observableType
    /// Matches capability
    bool capabilityMatchesIniEntry(const sspair & quantity, const Observable & observable);

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and module name
    bool moduleFuncMatchesIniEntry(functor *f, const ModuleRule &e, const Utils::type_equivalency & eq);

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and backend name
    bool backendFuncMatchesIniEntry(functor *f, const BackendRule &e, const Utils::type_equivalency & eq);

    /// Get entry level relevant for options
    int getEntryLevelForOptions(const Observable &e);

    /// Check whether functor matches rules
    /// Matches function name and type
    bool matchesRules( functor *f, const Rule & rule);

    /// Find entries (comparison of inifile entry with quantity or functor)
    /// @{
    const Observable * findIniEntry(
        sspair quantity, const std::vector<Observable> &, const str &);
    const Observable * findIniEntry(
        functor* f, const std::vector<Observable> &, const str &, const Utils::type_equivalency &);
    /// @}
    */


  }
}
