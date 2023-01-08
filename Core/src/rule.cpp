//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Function implementations for rule classes.
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

#include "gambit/Core/rule.hpp"
#include "gambit/Core/error_handlers.hpp"
#include "gambit/Core/resolution_utilities.hpp"
#include "gambit/Elements/functors.hpp"

namespace Gambit
{

  namespace DRes
  {

    /// True if and only if the passed functor matches the 'if' part of a rule
    bool Rule::antecedent_matches(functor* f, const Utils::type_equivalency& te) const
    {
      bool match = true;
      if (match and if_capability) match = stringComp(capability, f->capability()   ); 
      if (match and if_type)       match = typeComp  (type,       f->type(),      te); 
      if (match and if_function)   match = stringComp(function,   f->name()         ); 
      if (match and if_version)    match = stringComp(version,    f->version()      ); 
      return match;
    }

    /// True if and only if the passed functor matches the 'then' part of a rule
    bool Rule::consequent_matches(functor* f, const Utils::type_equivalency& te) const
    {
      bool match = true;
      if (match and then_capability) match = stringComp(capability, f->capability()   ); 
      if (match and then_type)       match = typeComp  (type,       f->type(),      te); 
      if (match and then_function)   match = stringComp(function,   f->name()         ); 
      if (match and then_version)    match = stringComp(version,    f->version()      ); 
      return match;
    }

    /// Whether a rule allows a given functor or not.  
    /// Must be true for a module functor to be used to resolve a dependency, or for a backend functor to be used to resolve a backend requirement.   
    /// True unless the functor passes the antecedent ('if' part of the rule) but fails the consequent ('then' part of the rule). 
    bool Rule::allows(functor* f, const Utils::type_equivalency& te, bool ignore_if_weak) const
    {
      if (ignore_if_weak and weakrule) return true;
      if (not antecedent_matches(f, te)) return true;
      bool result = consequent_matches(f, te);
      return result;
    }

    /// Check if a given string is a permitted field of the BackendRule class
    bool BackendRule::permits_field(const str& field)
    {
      return field == "backend";
    }

    /// True if and only if the passed backend functor matches the 'if' part of a rule
    bool BackendRule::antecedent_matches(functor* f, const Utils::type_equivalency& te) const
    {
      // Allow matching only if the antecedent has been properly specified.
      bool match = if_capability or if_type or if_function or if_version or if_backend;
      // Check if the base class part of the antecedent was matched.
      match = match and Rule::antecedent_matches(f, te);
      // Check if the derived class part of the antecedent was matched.
      if (match and if_backend) match = stringComp(backend, f->origin()); 
      return match;
    }

    /// True if and only if the passed backend functor matches the 'then' part of a rule
    bool BackendRule::consequent_matches(functor* f, const Utils::type_equivalency& te) const
    {
      // Allow matching only if the consequent has been properly specified.
      bool match = then_capability or then_type or then_function or then_version or then_backend;
      // Check if the base class part of the consequent was matched.
      match = match and Rule::consequent_matches(f, te);
      // Check if the derived class part of the consequent was matched.
      if (match and then_backend) match = stringComp(backend, f->origin()); 
      // Log match
      if (match and log_matches) f->addMatchedBackendRule(this);
      return match;
    }

    /// Check if a given string is a permitted field of the ModuleRule class
    bool ModuleRule::permits_field(const str& field)
    {
      static const std::set<str> fields =
      {
        "module",
        "options",
        "dependencies",
        "backends",
        "functionChain"
      };
      return (fields.find(field) != fields.end());
    }

    /// True if and only if the passed module functor matches the 'if' part of a rule
    bool ModuleRule::antecedent_matches(functor* f, const Utils::type_equivalency& te) const
    {
      // Allow matching only if the antecedent has been properly specified.
      bool match = if_capability or if_type or if_function or if_version or if_module;
      // Check if the base class part of the antecedent was matched.
      match = match and Rule::antecedent_matches(f, te);
      // Check if the derived class part of the antecedent was matched.
      if (match and if_module) match = stringComp(module, f->origin()); 
      return match;
    }

    /// True if and only if the passed module functor matches the 'then' part of a rule
    bool ModuleRule::consequent_matches(functor* f, const Utils::type_equivalency& te) const
    {
      // Allow matching only if the consequent has been properly specified.
      bool match = then_capability or then_type or then_function or then_version or
                   then_module or then_options or then_dependencies or then_backends or then_functionChain;
      // Check if the base class part of the consequent was matched.
      match = match and Rule::consequent_matches(f, te);
      // Check if the derived class part of the consequent was matched.
      if (match and then_module) match = stringComp(module, f->origin()); 
      // Log match
      if (match and log_matches) f->addMatchedModuleRule(this);
      return match;
    }

    /// Whether the set of dependency rules subjugate to this rule allow a given module functor or not. 
    bool ModuleRule::dependencies_allow(functor* f, const Utils::type_equivalency& te, bool ignore_if_weak) const
    {
      if (ignore_if_weak and weakrule) return true;
      bool allow = true;
      for (const ModuleRule& rule : dependencies) allow = allow and rule.allows(f, te, ignore_if_weak);
      return allow;      
    }

    /// Whether the functionChain of this rule allows a given module functor to be used to resolve the dependency of another. 
    bool ModuleRule::function_chain_allows(functor* candidate, functor* dependee, const Utils::type_equivalency& te, bool ignore_if_weak) const
    {
      // Scenarios in which the functionChain is irrelevent.
      if ((ignore_if_weak and weakrule) or
          (not then_functionChain) or 
          (not antecedent_matches(candidate, te))) return true;
      // Iterate over the entries in the functionChain
      for (auto it = functionChain.begin(); it != functionChain.end() - 1; ++it)
      {
        // Function is allowed if somewhere in the chain it is directly preceeded by the dependent function.
        if ((*it) == dependee->name()) return (*(it+1) == candidate->name());
      }
      return true;
    }

    /// Whether the set of backend rules subjugate to this rule allow a given backend functor or not. 
    bool ModuleRule::backend_reqs_allow(functor* f, const Utils::type_equivalency& te, bool ignore_if_weak) const
    {
      if (ignore_if_weak and weakrule) return true;
      bool allow = true;
      for (const BackendRule& rule : backends) allow = allow and rule.allows(f, te, ignore_if_weak);
      return allow;      
    }


/*
    /// Check whether quantity matches observableType
    /// Matches capability
    bool capabilityMatchesIniEntry(const sspair & quantity, const Observable & observable)
    {
      // Compares dependency specifications of rules entries or observable
      // entries with capability (capabilities have to be unique for these
      // lists)
      return ( stringComp( observable.capability, quantity.first ) );
    }

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and module name
    bool moduleFuncMatchesIniEntry(functor *f, const ModuleRule &e, const Utils::type_equivalency & eq)
    {
      return (e.capability != "" ? stringComp(e.capability, f->capability()) : true)
         and (e.type       != "" ? typeComp  (e.type,       f->type(), eq)   : true)
         and (e.function   != "" ? stringComp(e.function,   f->name())       : true)
         and (e.module     != "" ? stringComp(e.module,     f->origin())     : true);
    }

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and backend name
    bool backendFuncMatchesIniEntry(functor *f, const BackendRule &e, const Utils::type_equivalency & eq)
    {
      return (e.capability != "" ? stringComp(e.capability, f->capability()) : true)
         and (e.type       != "" ? typeComp  (e.type,       f->type(), eq)   : true)
         and (e.function   != "" ? stringComp(e.function,   f->name())       : true)
         and (e.backend    != "" ? stringComp(e.backend,    f->origin())     : true)
         and (e.version    != "" ? stringComp(e.version,    f->version())    : true);
    }

    /// Check whether functor matches rules
    /// Matches function name and type
    bool matchesRules( functor *f, const Rule & rule)
    {
      #ifdef DEPRES_DEBUG
        cout << (*f).name() << " vs " << rule.function << endl;
        cout << (*f).origin() << " vs " << rule.module << endl;
      #endif
      // Rule matches functor if the capability or module function
      // is present and they (and the module if given) match
      bool matches = false;
      if (not rule.capability.empty())
        matches = stringComp( rule.capability, f->capability() );
      if (not rule.function.empty())
        matches = stringComp( rule.function, f->name() );
      if (not rule.module.empty())
        matches = matches and stringComp( rule.module, f->origin());
      return matches;
    }

    /// Find rules entry that matches vertex
    const Observable * findIniEntry(functor* f,
     const std::set<Observable> &entries, const str & errtag, const Utils::type_equivalency & eq)
    {
      std::vector<const Observable*> auxEntryCandidates;
      for (std::set<Observable>::const_iterator it =
          entries.begin(); it != entries.end(); ++it)
      {
        if ( moduleFuncMatchesIniEntry(f, *it, eq) and it->capability != "" )
        {
          auxEntryCandidates.push_back(&(*it));
        }
      }
      if ( auxEntryCandidates.size() == 0 ) return NULL;
      else if ( auxEntryCandidates.size() != 1 )
      {
        str errmsg = "Found multiple " + errtag + " entries for ";
        errmsg += f->capability() +" (" +
            f->type() + ") [" +
            f->name() + ", " +
            f->origin() + "]";
        dependency_resolver_error().raise(LOCAL_INFO, errmsg);
      }
      return auxEntryCandidates[0]; // auxEntryCandidates.size() == 1
    }

    /// Find observable entry that matches capability/type
    const Observable* findIniEntry(
            sspair quantity, const std::set<Observable> & entries, const str & errtag)
    {
      std::vector<const Observable*> obsEntryCandidates;
      for (std::set<Observable>::const_iterator it =
          entries.begin(); it != entries.end(); ++it)
      {
        if ( capabilityMatchesIniEntry(quantity, *it) ) // use same criteria than for normal dependencies
        {
          obsEntryCandidates.push_back(&(*it));
        }
      }
      if ( obsEntryCandidates.size() == 0 ) return NULL;
      else if ( obsEntryCandidates.size() != 1 )
      {
        str errmsg = "Found multiple " + errtag + " entries for ";
        errmsg += quantity.first + " (" + quantity.second + ")";
        dependency_resolver_error().raise(LOCAL_INFO,errmsg);
      }
      return obsEntryCandidates[0]; // obsEntryCandidates.size() == 1
    }
*/







  }

}
