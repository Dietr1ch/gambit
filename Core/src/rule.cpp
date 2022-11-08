//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Function implementations for rules class.
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
#include "gambit/Elements/functors.hpp"

namespace Gambit
{

  namespace DRes
  {

    /// Constructor
    Rule::Rule(IniParser::ObservableType t)
    {
      capability = t.capability;
      type  = t.type;
      function = t.function;
      module = t.module;
      backend = t.backend;
      version = t.version;
      options = t.options;
    }


    /// Check whether quantity matches observableType
    /// Matches capability and type
    bool quantityMatchesIniEntry(const sspair & quantity, const IniParser::ObservableType & observable, const Utils::type_equivalency & eq)
    {
      // Compares dependency specifications of rules entries or observable
      // entries with capability (capabilities have to be unique for these
      // lists)
      return ( stringComp( observable.capability, quantity.first ) and
               typeComp  ( observable.type,       quantity.second, eq ));
    }

    /// Check whether quantity matches observableType
    /// Matches capability
    bool capabilityMatchesIniEntry(const sspair & quantity, const IniParser::ObservableType & observable)
    {
      // Compares dependency specifications of rules entries or observable
      // entries with capability (capabilities have to be unique for these
      // lists)
      return ( stringComp( observable.capability, quantity.first ) );
    }

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and module name
    bool moduleFuncMatchesIniEntry(functor *f, const IniParser::ObservableType &e, const Utils::type_equivalency & eq)
    {
      return (e.capability != "" ? stringComp(e.capability, f->capability()) : true)
         and (e.type       != "" ? typeComp  (e.type,       f->type(), eq)   : true)
         and (e.function   != "" ? stringComp(e.function,   f->name())       : true)
         and (e.module     != "" ? stringComp(e.module,     f->origin())     : true);
    }

    /// Check whether functor matches ObservableType
    /// Matches capability, type, function and backend name
    bool backendFuncMatchesIniEntry(functor *f, const IniParser::ObservableType &e, const Utils::type_equivalency & eq)
    {
      return (e.capability != "" ? stringComp(e.capability, f->capability()) : true)
         and (e.type       != "" ? typeComp  (e.type,       f->type(), eq)   : true)
         and (e.function   != "" ? stringComp(e.function,   f->name())       : true)
         and (e.backend    != "" ? stringComp(e.backend,    f->origin())     : true)
         and (e.version    != "" ? stringComp(e.version,    f->version())    : true);
    }

    /// Get entry level relevant for options
    int getEntryLevelForOptions(const IniParser::ObservableType &e)
    {
      int z = 0;
      if ( e.module != "" ) z = 1;
      if ( e.capability != "" ) z = 2;
      if ( e.type != "" ) z = 3;
      if ( e.function != "" ) z = 4;
      return z;
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
    const IniParser::ObservableType * findIniEntry(functor* f,
     const IniParser::ObservablesType &entries, const str & errtag, const Utils::type_equivalency & eq)
    {
      std::vector<const IniParser::ObservableType*> auxEntryCandidates;
      for (IniParser::ObservablesType::const_iterator it =
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
    const IniParser::ObservableType* findIniEntry(
            sspair quantity, const IniParser::ObservablesType & entries, const str & errtag)
    {
      std::vector<const IniParser::ObservableType*> obsEntryCandidates;
      for (IniParser::ObservablesType::const_iterator it =
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








  }

}
