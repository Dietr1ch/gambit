//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Function implementations for observable class.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (patrickcolinscott@gmail.com)
///  \date 2022 Dec
///
///  *********************************************

#include "gambit/Core/observable.hpp"
#include "gambit/Core/resolution_utilities.hpp"
#include "gambit/Elements/type_equivalency.hpp"

namespace Gambit
{

  namespace DRes
  {

    /// True if and only if the passed functor matches all matchable non-empty fields of the observable (i.e. everything except purpose, subcaps, dependencies and backend_reqs).
    bool Observable::matches(functor* f, const Utils::type_equivalency& te) const
    {
      return ( stringComp ( capability, f->capability()     ) and
               typeComp   ( type,       f->type()      , te ) and
               stringComp ( function,   f->name()           ) and
               stringComp ( version,    f->version()        ) and
               stringComp ( module,     f->origin()         ) );
    }

    /// Whether the set of dependency rules subjugate to this observable allow a given module functor or not. 
    bool Observable::dependencies_allow(functor* f, const Utils::type_equivalency& te) const
    {
      bool allow = true;
      for (const ModuleRule& rule : dependencies) allow = allow and rule.allows(f, te);
      return allow;      
    }

    /// Whether the set of backend rules subjugate to this observable allow a given backend functor or not. 
    bool Observable::backend_reqs_allow(functor* f, const Utils::type_equivalency& te) const
    {
      bool allow = true;
      for (const BackendRule& rule : backends) allow = allow and rule.allows(f, te);
      return allow;      
    }

  }

}
