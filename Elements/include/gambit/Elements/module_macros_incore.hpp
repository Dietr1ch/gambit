//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Generic observable and likelihood function 
///  macro definitions, for inclusion from the
///  Core.
///
///
///  Note here that \link FUNCTION() FUNCTION 
///  \endlink is the actual module function name,
///  whereas both \link CAPABILITY() CAPABILITY 
///  \endlink and all \em DEPs refer to the 
///  abstract physical quantities that functions 
///  may provide or require.  Thus, the provides()
///  methods expect a quantity input (i.e. 
///  corresponding to a \link CAPABILITY() 
///  CAPABILITY\endlink), the requires() methods  
///  expect a quantity input for the dependency but a 
///  function name input (i.e. corresponding to a 
///  \link FUNCTION() FUNCTION\endlink) for 
///  the actual dependent function, and all other 
///  things operate on the basis of the function 
///  name, not the quantity that is calculated.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott
///          (patscott@physics.mcgill.ca)
///  \date 2012 Nov  
///  \date 2013,14 Foreverrrrr
///
///  \author Abram Krislock
///          (abram.krislock@fysik.su.se)
///  \date 2013 Jan, Feb
///
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2013 Jan, Feb, 2014 Jan
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2013 Nov
///
///  \author Tomas Gonzalo
///          (t.e.gonzalo@fys.uio.no)
///  \date 2016 June
///
///  *********************************************

#ifndef __module_macros_incore_hpp__
#define __module_macros_incore_hpp__

/// Change this to 1 if you really don't care about parameter clashes.
#define ALLOW_DUPLICATES_IN_PARAMS_MAP 0

/// Suppress unused variable warnings in GCC (and do nothing for other compilers)
#ifndef VARIABLE_IS_NOT_USED
#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif
#endif

#include <map>

#include "gambit/Elements/functors.hpp"
#include "gambit/Elements/types_rollcall.hpp"
#include "gambit/Elements/module_macros_common.hpp"
#include "gambit/Elements/safety_bucket.hpp"
#include "gambit/Elements/ini_functions.hpp"
#include "gambit/Elements/ini_code_struct.hpp"
#include "gambit/Utils/static_members.hpp"
#include "gambit/Utils/exceptions.hpp"
#include "gambit/Backends/backend_singleton.hpp"
#include "gambit/Models/claw_singleton.hpp"
#include "gambit/Models/safe_param_map.hpp"
#ifndef STANDALONE
  #include "gambit/Core/ini_functions.hpp"
#endif

#include <boost/preprocessor/logical/bitand.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/seq/cat.hpp>



/// \name Tag-registration macros
/// @{
/// Add a regular tag to the current namespace
#define ADD_TAG_IN_CURRENT_NAMESPACE(TAG) namespace Tags { struct TAG; }
/// Add a backend tag to the current namespace
#define ADD_BETAG_IN_CURRENT_NAMESPACE(TAG) namespace BETags { struct TAG; }
/// Add a backend tag to the current namespace
#define ADD_MODEL_TAG_IN_CURRENT_NAMESPACE(TAG) namespace ModelTags { struct TAG; }
/// @}


/// \name Rollcall macros (redirection within the Core).
/// These are called from within rollcall headers in each module to 
/// register module functions, their capabilities, return types, dependencies,
/// and backend requirements.
/// @{

/// Registers the current \link MODULE() MODULE\endlink.
#define START_MODULE                                      CORE_START_MODULE

/// Registers the current \link CAPABILITY() CAPABILITY\endlink of the current 
/// \link MODULE() MODULE\endlink.
#define START_CAPABILITY                                  CORE_START_CAPABILITY(MODULE, CAPABILITY)
/// Long (all argument) version of \link START_CAPABILITY() START_CAPABILITY\endlink.
#define LONG_START_CAPABILITY(MODULE, CAPABILITY)         CORE_START_CAPABILITY(MODULE, CAPABILITY)

/// Registers the current \link FUNCTION() FUNCTION\endlink of the current 
/// \link MODULE() MODULE\endlink as a provider
/// of the current \link CAPABILITY() CAPABILITY\endlink, returning a result of 
/// type \em TYPE.
#define DECLARE_FUNCTION(TYPE, FLAG)                      CORE_DECLARE_FUNCTION(MODULE, CAPABILITY, FUNCTION, TYPE, FLAG)
/// Long (all argument) version of \link DECLARE_FUNCTION() DECLARE_FUNCTION\endlink.
#define LONG_DECLARE_FUNCTION(MODULE, CAPABILITY, FUNCTION, TYPE, FLAG) CORE_DECLARE_FUNCTION(MODULE, CAPABILITY, FUNCTION, TYPE, FLAG)

/// Indicates that the current \link FUNCTION() FUNCTION\endlink of the current 
/// \link MODULE() MODULE\endlink must be managed by another function (in the same
/// module or another) that calls it from within a loop.  That other function must
/// provide capability \em LOOPMAN. 
#define NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN)            CORE_NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN)                                  

/// Indicate that the current \link FUNCTION() FUNCTION\endlink depends on the 
/// presence of another module function that can supply capability \em DEP, with
/// return type \em TYPE.
#define DEPENDENCY(DEP, TYPE)                             CORE_DEPENDENCY(DEP, TYPE, MODULE, FUNCTION, NOT_MODEL)
/// Long (all argument) version of \link DEPENDENCY() DEPENDENCY\endlink.
#define LONG_DEPENDENCY(MODULE, FUNCTION, DEP, TYPE)      CORE_DEPENDENCY(DEP, TYPE, MODULE, FUNCTION, NOT_MODEL)

/// Indicate that the current \link FUNCTION() FUNCTION\endlink may only be used with
/// specific model \em MODEL, or combinations given via ALLOW_MODEL_COMBINATION.
/// If both this and ALLOW_MODEL_COMBINATION are absent, all models are allowed, but 
/// model parameters will not generally be accessible from within the module funtion.
#define ALLOWED_MODEL(MODULE,FUNCTION,MODEL)              CORE_ALLOWED_MODEL(MODULE,FUNCTION,MODEL)

/// Indicate that the current \link FUNCTION() FUNCTION\endlink may be used with a
/// specific model \em MODEL, but only in combination with others given via ALLOW_MODEL_COMBINATION.
#define ALLOWED_MODEL_DEPENDENCE(MODULE,FUNCTION,MODEL)   CORE_ALLOW_MODEL_DEPENDENCE(MODULE,FUNCTION,MODEL)

/// Indicate that the current \link FUNCTION() FUNCTION\endlink may only be used with
/// the specific model combination given, with other combinations passed in the same 
/// way, or with individual models speficied via ALLOW_MODEL(S).  If both this and 
/// ALLOW_MODEL(s) are absent, all models are allowed but no model parameters will be
/// accessible from within the module funtion.
#define ALLOW_MODEL_COMBINATION(...)                      CORE_ALLOW_MODEL_COMBINATION(MODULE,FUNCTION,(__VA_ARGS__))

/// Define a model GROUP of name GROUPNAME for use with ALLOW_MODEL_COMBINATION.
#define MODEL_GROUP(GROUPNAME,GROUP)                      CORE_MODEL_GROUP(MODULE,FUNCTION,GROUPNAME,GROUP)

/// BACKEND_REQ indicates that the current \link FUNCTION() FUNCTION\endlink requires one
/// backend variable or function to be available from a capability group \em GROUP,
/// and then declares a viable member of that group, with capability \em REQUIREMENT, 
/// type \em TYPE and (in the case of functions) arguments \em ARGS.  BACKEND_REQ also 
/// allows the user to specify a list of \em TAGS that apply to this specific group member,
/// which can then be used for easily implementing rules for choosing between different 
/// members of the same \em GROUP.  Note that \em GROUPs are automatically declared the 
/// first time that they are mentioned in a BACKEND_REQ statement.
#define DECLARE_BACKEND_REQ(GROUP, REQUIREMENT, TAGS, TYPE, ARGS, IS_VARIABLE) \
                                                          CORE_BACKEND_REQ(MODULE, CAPABILITY, FUNCTION, GROUP, REQUIREMENT, TAGS, TYPE, ARGS, IS_VARIABLE) 
#define LONG_DECLARE_BACKEND_REQ(MODULE, CAPABILITY, FUNCTION, GROUP, REQUIREMENT, TAGS, TYPE, ARGS, IS_VARIABLE) \
                                                          CORE_BACKEND_REQ(MODULE, CAPABILITY, FUNCTION, GROUP, REQUIREMENT, TAGS, TYPE, ARGS, IS_VARIABLE) 

/// Declare a backend group, from which one backend requirement must be activated.
#define BE_GROUP(GROUP)                                   CORE_BE_GROUP(GROUP)

/// Define a rule that uses TAGS to determine which backend requirements of the current
/// \link FUNCTION() FUNCTION\endlink are explicitly activated when one or more models 
/// from the set MODELS are being scanned.  Declaring this rule makes backend requirements
/// that match one or more TAGS conditional on the model being scanned.  Backend 
/// requirements that do not match any such rule are considered unconditional, and are
/// activated regardless of the model(s) being scanned.  Note that all rules have
/// _immediate_ effect, so only apply to BACKEND_REQs of the current FUNCTION that have 
/// already been declared!
#define ACTIVATE_BACKEND_REQ_FOR_MODELS(MODELS,TAGS)      CORE_BE_MODEL_RULE(MODELS,TAGS)                   

/// Define a rule that uses TAGS to determine a set of backend requirements of the current
/// \link FUNCTION() FUNCTION\endlink that are permitted to be fulfilled by the indicated
/// BACKEND_AND_VERSIONS.  If no versions are given, all versions of the stated backend are
/// considered allowed.  Declaring this rule makes all backend requirements that match
/// the rule resolvable _only_ by the backend-version pairs passed into 
/// \link BACKEND_OPTION() BACKEND_OPTION\endlink. Additional options provided by subsequent
/// calls to \link BACKEND_OPTION() BACKEND_OPTION\endlink are added to the options provided
/// by each previous declaration. In the case of multiple contradictory calls to 
/// \link BACKEND_OPTION() BACKEND_OPTION\endlink, the rule defined by the latest call takes
/// precedence.  Note that all rules have _immediate_ effect, so only apply to BACKEND_REQs
/// of the current FUNCTION that have already been declared!
#define BACKEND_OPTION(BACKEND_AND_VERSIONS,TAGS)         CORE_BACKEND_OPTION(BACKEND_AND_VERSIONS,TAGS)                   

/// Define a rule that certain sets of backend requirements need to be resolved by the same backend.
/// The sets are identified by tags, any number of which can be passed to FORCE_SAME_BACKEND.
/// All backend requirements with a given tag passed into FORCE_SAME_BACKEND will be forced
/// by the dependency resolver to use functions from the same backend.  Note that all rules have
/// _immediate_ effect, so only apply to BACKEND_REQs of the current FUNCTION that have 
/// already been declared!
#define FORCE_SAME_BACKEND(...)                           CORE_FORCE_SAME_BACKEND(__VA_ARGS__)

/// Indicate that the current \link FUNCTION() FUNCTION\endlink may depend on the 
/// presence of another module function that can supply capability 
/// \link CONDITIONAL_DEPENDENCY() CONDITIONAL_DEPENDENCY\endlink, with return type
/// \em TYPE.
#define START_CONDITIONAL_DEPENDENCY(TYPE)                CORE_START_CONDITIONAL_DEPENDENCY(MODULE, CAPABILITY, \
                                                           FUNCTION, CONDITIONAL_DEPENDENCY, TYPE)

/// Indicate that the current \link CONDITIONAL_DEPENDENCY() CONDITIONAL_DEPENDENCY\endlink
/// should be activated if the backend requirement \em BACKEND_REQ of the current 
/// \link FUNCTION() FUNCTION\endlink is filled by a backend function from \em BACKEND.
/// The versions of \em BACKEND that this applies to are passed in \em VERSTRING.
#define ACTIVATE_DEP_BE(BACKEND_REQ, BACKEND, VERSTRING)  CORE_ACTIVATE_DEP_BE(BACKEND_REQ, BACKEND, VERSTRING)

/// Indicate that the current \link CONDITIONAL_DEPENDENCY() CONDITIONAL_DEPENDENCY\endlink
/// should be activated if the model being scanned matches one of the models passed as an argument.
#define ACTIVATE_FOR_MODELS(...)                          ACTIVATE_DEP_MODEL(MODULE, CAPABILITY, FUNCTION, CONDITIONAL_DEPENDENCY, #__VA_ARGS__)

/// Quick, one-line declaration of model-conditional dependencies
#define MODEL_CONDITIONAL_DEPENDENCY(DEP, TYPE, ...)      CORE_START_CONDITIONAL_DEPENDENCY(MODULE, CAPABILITY, FUNCTION, DEP, TYPE) \
                                                          ACTIVATE_DEP_MODEL(MODULE, CAPABILITY, FUNCTION, DEP, #__VA_ARGS__)                        

/// Indicate that the current \link FUNCTION() FUNCTION\endlink requires classes that
/// must be loaded from \em BACKEND, version \em VERSION.  
#define CLASSLOAD_NEEDED(BACKEND, VERSION)               CORE_CLASSLOAD_NEEDED(BACKEND, VERSION)
/// @}


//  *******************************************************************************
/// \name Actual in-core rollcall macros
/// These macros do the actual heavy lifting within the rollcall system.
/// @{

// Determine whether to make registration calls to the Core in the START_MODULE
// macro, depending on STANDALONE flag 
#ifdef STANDALONE
  #define CORE_START_MODULE_COMMON(MODULE)                                     \
          CORE_START_MODULE_COMMON_MAIN(MODULE)
#else
  #define CORE_START_MODULE_COMMON(MODULE)                                     \
          CORE_START_MODULE_COMMON_MAIN(MODULE)                                \
          const int module_registered = register_module(STRINGIFY(MODULE));    
#endif

/// Redirection of \link START_MODULE() START_MODULE\endlink when invoked from 
/// within the core.
#define CORE_START_MODULE                                                      \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "START_MODULE."))                                                           \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Module errors */                                                      \
      error& CAT(MODULE,_error)()                                              \
      {                                                                        \
        static error local("A problem has been raised by " STRINGIFY(MODULE)   \
                           ".", STRINGIFY(MODULE) "_error");                   \
        return local;                                                          \
      }                                                                        \
                                                                               \
      /* Module warnings */                                                    \
      warning& CAT(MODULE,_warning)()                                          \
      {                                                                        \
        static warning local("A problem has been raised by " STRINGIFY(MODULE) \
                           ".", STRINGIFY(MODULE) "_warning");                 \
        return local;                                                          \
      }                                                                        \
                                                                               \
      /* Register the module with the log system.  Not done for models. */     \
      const int log_registered = register_module_with_log(STRINGIFY(MODULE));  \
                                                                               \
      CORE_START_MODULE_COMMON(MODULE)                                         \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Central module definition macro, used by modules and models.
#define CORE_START_MODULE_COMMON_MAIN(MODULE)                                  \
                                                                               \
      namespace Accessors                                                      \
      {                                                                        \
                                                                               \
        /* Module name */                                                      \
        str name() { return STRINGIFY(MODULE); }                               \
                                                                               \
        /* Maps from tag strings to tag-specialisted functions */              \
        std::map<str, bool(*)()> map_bools;                                    \
        std::map<str, bool(*)(str)> condit_bools;                              \
        std::map<str, std::map<str, bool(*)()> >model_bools;                   \
                                                                               \
        /* All module observables/likelihoods, their dependencies, required    \
        quantities from backends, and their types, as strings */               \
        static std::map<str,str> iCanDo, iMayNeed, iMayNeedFromBackends;       \
                                                                               \
        /* Module provides observable/likelihood TAG? */                       \
        template <typename TAG>                                                \
        bool provides() { return false; }                                      \
                                                                               \
        /* Overloaded, non-templated version */                                \
        bool provides(str obs)                                                 \
        {                                                                      \
          if (map_bools.find(obs) == map_bools.end()) { return false; }        \
          return (*map_bools[obs])();                                          \
        }                                                                      \
                                                                               \
        /* Module requires observable/likelihood DEP_TAG to compute TAG */     \
        template <typename DEP_TAG, typename TAG>                              \
        bool requires() { return false; }                                      \
                                                                               \
        /* Overloaded, non-templated version */                                \
        bool requires(str dep, str obs)                                        \
        {                                                                      \
          if (map_bools.find(dep+obs) == map_bools.end()) { return false; }    \
          return (*map_bools[dep+obs])();                                      \
        }                                                                      \
                                                                               \
        /* Additional overloaded, non-templated versions */                    \
        bool requires(str dep, str obs, str req, str be, str ver)              \
        {                                                                      \
          if (requires(dep, obs)) {return true; }                              \
          if (condit_bools.find(dep+obs+req+be) == condit_bools.end())         \
          {                                                                    \
            return false;                                                      \
          }                                                                    \
          if ((*condit_bools[dep+obs+req+be])("any"))                          \
          {                                                                    \
            return true;                                                       \
          }                                                                    \
          else                                                                 \
          {                                                                    \
            return (*condit_bools[dep+obs+req+be])(ver);                       \
          }                                                                    \
        }                                                                      \
        bool requires(str dep, str obs, str req, str be)                       \
        {                                                                      \
          return requires(dep, obs, req, be, "any");                           \
        }                                                                      \
                                                                               \
        /* Module could require quantity BE_TAG from a backend to compute TAG*/\
        template <typename BE_TAG, typename TAG>                               \
        bool could_need_from_backend() { return false; }                       \
                                                                               \
        /* Overloaded, non-templated version */                                \
        bool could_need_from_backend(str quant, str obs)                       \
        {                                                                      \
          if (map_bools.find("BE_"+quant+obs) == map_bools.end()) return false;\
          return (*map_bools["BE_"+quant+obs])();                              \
        }                                                                      \
                                                                               \
        /* Module currently requires quant from a backend to compute obs*/     \
        bool currently_needs_from_backend(str quant, str obs)                  \
        {                                                                      \
          if (map_bools.find("BE_"+quant+obs+"now") == map_bools.end())        \
           return false;                                                       \
          return (*map_bools["BE_"+quant+obs+"now"])();                        \
        }                                                                      \
                                                                               \
        /* Module may require observable/likelihood DEP_TAG to compute TAG,    \
        depending on the backend and version meeting requirement REQ_TAG.*/    \
        template <typename DEP_TAG, typename TAG, typename REQ_TAG,            \
         typename BE>                                                          \
        bool requires_conditional_on_backend(str) {return false; }             \
                                                                               \
        /* Overloaded version of templated function */                         \
        template <typename DEP_TAG, typename TAG, typename REQ_TAG,            \
         typename BE>                                                          \
        bool requires_conditional_on_backend()                                 \
        {                                                                      \
          return requires_conditional_on_backend<DEP_TAG,TAG,                  \
           REQ_TAG,BE>("any");                                                 \
        }                                                                      \
                                                                               \
        /* Module may require observable/likelihood DEP_TAG to compute TAG,    \
        depending on the model being scanned.*/                                \
        template <typename DEP_TAG, typename TAG>                              \
        bool requires_conditional_on_model(str) {return false; }               \
                                                                               \
        /* Module allows use of model MODEL_TAG when computing TAG */          \
        template <typename MODEL_TAG, typename TAG>                            \
        bool explicitly_allowed_model()                                        \
        {                                                                      \
          return false;                                                        \
        }                                                                      \
                                                                               \
        /* Overloaded, non-templated version */                                \
        bool allowed_model(str model, str obs)                                 \
        {                                                                      \
          if (model_bools.find(obs) == model_bools.end()) return true;         \
          if (model_bools[obs].find(model) == model_bools[obs].end())          \
           return false;                                                       \
          return (*model_bools[obs][model])();                                 \
        }                                                                      \
                                                                               \
      }                                                                        \
                                                                               \
      /* Resolve dependency DEP_TAG in function TAG */                         \
      template <typename DEP_TAG, typename TAG>                                \
      void resolve_dependency(functor*, module_functor_common*)                \
      {                                                                        \
        cout<<STRINGIFY(MODULE)<<" does not"<<endl;                            \
        cout<<"have this dependency for this function.";                       \
      }                                                                        \
                                                                               \
      /* Resolve backend requirement BE_REQ in function TAG */                 \
      template <typename BE_REQ, typename TAG>                                 \
      void resolve_backendreq(functor* be_functor)                             \
      {                                                                        \
        cout<<STRINGIFY(MODULE)<<" does not"<<endl;                            \
        cout<<"have this backend requirement for this function.";              \
      }                                                                        \
                                                                               \
      /* Runtime registration function for nesting requirements of             \
      observable/likelihood function TAG*/                                     \
      template <typename TAG>                                                  \
      void rt_register_function_nesting ()                                     \
      {                                                                        \
        cout<<"This tag is not supported by "<<STRINGIFY(MODULE)<<"."<<endl;   \
      }                                                                        \
                                                                               \
      /* Runtime registration function for dependency DEP_TAG of function TAG*/\
      template <typename DEP_TAG, typename TAG>                                \
      void rt_register_dependency ()                                           \
      {                                                                        \
        cout<<STRINGIFY(MODULE)<<" does not"<<endl;                            \
        cout<<"have this dependency for this function.";                       \
      }                                                                        \
                                                                               \
      /* Runtime registration of conditional dependency DEP_TAG of function    \
      TAG, where dependency exists if TAG requires backend function BE_REQ,    \
      and BE_REQ is provided by backend BE.*/                                  \
      template <typename DEP_TAG, typename TAG, typename BE_REQ, typename BE>  \
      void rt_register_conditional_dependency ()                               \
      {                                                                        \
        rt_register_conditional_dependency<DEP_TAG, TAG>();                    \
      }                                                                        \
      template <typename DEP_TAG, typename TAG>                                \
      void rt_register_conditional_dependency ()                               \
      {                                                                        \
        cout<<STRINGIFY(MODULE)<<" does not"<<endl;                            \
        cout<<"have any matching conditional dependency.";                     \
      }                                                                        \
                                                                               \
      /* Runtime registration function for backend req BE_REQ of               \
      function TAG*/                                                           \
      template <typename BE_REQ, typename TAG>                                 \
      void rt_register_req ()                                                  \
      {                                                                        \
        cout<<STRINGIFY(MODULE)<<" does not"<<endl;                            \
        cout<<"have this backend requirement for this function.";              \
      }                                                                        \


/// Redirection of \link START_CAPABILITY() START_CAPABILITY\endlink when  
/// invoked from within the core.
#define CORE_START_CAPABILITY(MODULE, CAPABILITY)                              \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "START_CAPABILITY."))                                                       \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling START_CAPABILITY. Please check the rollcall header for "           \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    /* Add CAPABILITY to the global set of things that can be calculated*/     \
    ADD_TAG_IN_CURRENT_NAMESPACE(CAPABILITY)                                   \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      namespace Accessors                                                      \
      {                                                                        \
        /* Indicate that this module can provide quantity CAPABILITY */        \
        template <>                                                            \
        bool provides<Gambit::Tags::CAPABILITY>() { return true; }             \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of \link START_FUNCTION() START_FUNCTION\endlink when invoked 
/// from within the core.
#define CORE_DECLARE_FUNCTION(MODULE, CAPABILITY, FUNCTION, TYPE, FLAG)        \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "START_FUNCTION."))                                                         \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling START_FUNCTION. Please check the rollcall header for "             \
   STRINGIFY(MODULE) "."))                                                     \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "START_FUNCTION. Please check the rollcall header for "                     \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    /* Fail if a void-type function is declared, unless it can manage loops or \
       is an initialisation function. */                                       \
    BOOST_PP_IIF(BOOST_PP_BITAND(IS_TYPE(void,TYPE), BOOST_PP_EQUAL(FLAG, 0)), \
      FAIL("Module functions cannot have void results, unless they manage "    \
       "loops or are initialisation functions.  Loop managers are declared "   \
       "by adding CAN_MANAGE_LOOPS as the second argument of START_FUNCTION."  \
       "Initialisation functions are declared from frontend headers by using " \
       "the BE_INI_FUNCTION macro.  Please check the header file for module "  \
       STRINGIFY(MODULE) ", function " STRINGIFY(FUNCTION) ".")                \
    ,)                                                                         \
                                                                               \
    BOOST_PP_IIF(BOOST_PP_BITAND(BOOST_PP_NOT(IS_TYPE(void,TYPE)),             \
                                 BOOST_PP_EQUAL(FLAG, 2) ),                    \
      /* Fail if an initialisation function has a non-void return type */      \
      FAIL("Initialisation functions must have void results. This is "         \
       "indicated by using the BE_INI_FUNCTION macro in a frontend header.")   \
    ,)                                                                         \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
      /* Add FUNCTION to the module's set of recognised functions. */          \
      ADD_TAG_IN_CURRENT_NAMESPACE(FUNCTION)                                   \
                                                                               \
      /* Register (prototype) the function */                                  \
      BOOST_PP_IIF(IS_TYPE(void,TYPE),                                         \
        void FUNCTION();                                                       \
      ,                                                                        \
        void FUNCTION (TYPE &);                                                \
      )                                                                        \
                                                                               \
      /* Wrap it in a functor */                                               \
      MAKE_FUNCTOR(FUNCTION,TYPE,CAPABILITY,MODULE,BOOST_PP_EQUAL(FLAG, 1))    \
    }                                                                          \
                                                                               \
  }                                                                            \


// Determine whether to make registration calls to the Core in the MAKE_FUNCTOR
// macro, depending on STANDALONE flag 
#ifdef STANDALONE
  #define MAKE_FUNCTOR(FUNCTION,TYPE,CAPABILITY,ORIGIN,CAN_MANAGE)             \
          MAKE_FUNCTOR_MAIN(FUNCTION,TYPE,CAPABILITY,ORIGIN,CAN_MANAGE)
#else
  #define MAKE_FUNCTOR(FUNCTION,TYPE,CAPABILITY,ORIGIN,CAN_MANAGE)             \
          MAKE_FUNCTOR_MAIN(FUNCTION,TYPE,CAPABILITY,ORIGIN,CAN_MANAGE)        \
          const int CAT(FUNCTION,_registered2) =                               \
           register_module_functor_core(Functown::FUNCTION);
#endif


/// Main parts of the functor creation
#define MAKE_FUNCTOR_MAIN(FUNCTION,TYPE,CAPABILITY,ORIGIN,CAN_MANAGE)          \
                                                                               \
  namespace Functown                                                           \
  {                                                                            \
    /* Create the function wrapper object (functor) */                         \
    BOOST_PP_IIF(IS_TYPE(ModelParameters,TYPE),                                \
      model_functor                                                            \
    ,                                                                          \
      module_functor<TYPE>                                                     \
    )                                                                          \
    FUNCTION (&ORIGIN::FUNCTION, STRINGIFY(FUNCTION), STRINGIFY(CAPABILITY),   \
     STRINGIFY(TYPE), STRINGIFY(ORIGIN), Models::ModelDB());                   \
    /* Set up a helper function to call the iterate method if the functor is   \
    able to manage loops. */                                                   \
    BOOST_PP_IIF(BOOST_PP_EQUAL(CAN_MANAGE, 1),                                \
     void CAT(FUNCTION,_iterate)(long long it) { FUNCTION.iterate(it); }       \
    ,)                                                                         \
    /* Create a helper function to indicate whether a given model is in use. */\
    BOOST_PP_IIF(IS_TYPE(ModelParameters,TYPE), ,                              \
     bool CAT(FUNCTION,_modelInUse)(str model)                                 \
     {                                                                         \
       return FUNCTION.getActiveModelFlag(model);                              \
     }                                                                         \
    )                                                                          \
  }                                                                            \
                                                                               \
  namespace Pipes                                                              \
  {                                                                            \
                                                                               \
    namespace FUNCTION                                                         \
    {                                                                          \
      /* Create a map to hold pointers to all the model parameters accessible  \
      to this functor */                                                       \
      Models::safe_param_map<safe_ptr<const double> > Param;                   \
      /* Pointer to function indicating whether a given model is in use.*/     \
      BOOST_PP_IIF(IS_TYPE(ModelParameters,TYPE), ,                            \
       bool (*ModelInUse)(str) = &Functown::CAT(FUNCTION,_modelInUse); )       \
      /* Declare a safe pointer to the functor's run options. */               \
      safe_ptr<Options> runOptions;                                            \
      BOOST_PP_IIF(CAN_MANAGE,                                                 \
       namespace Loop                                                          \
       {                                                                       \
         /* Create a pointer to the single iteration of the loop that can      \
         be executed by this functor */                                        \
         void (*executeIteration)(long long)=&Functown::CAT(FUNCTION,_iterate);\
         /* Declare a safe pointer to the flag indicating that a managed loop  \
         is ready for breaking. */                                             \
         safe_ptr<bool> done;                                                  \
         /* Declare a function that is used to reset the done flag. */         \
         void reset() { Functown::FUNCTION.resetLoop(); }                      \
       }                                                                       \
      ,)                                                                       \
    }                                                                          \
                                                                               \
  }                                                                            \
                                                                               \
  /* Register the function */                                                  \
  const int VARIABLE_IS_NOT_USED CAT(FUNCTION,_registered1) = register_function(Functown::FUNCTION, \
   CAN_MANAGE,                                                                 \
   BOOST_PP_IIF(CAN_MANAGE, &Pipes::FUNCTION::Loop::done, NULL),               \
   Accessors::iCanDo, Accessors::map_bools,                                    \
   Accessors::provides<Gambit::Tags::CAPABILITY>, Pipes::FUNCTION::runOptions);\

// Determine whether to make registration calls to the Core in the 
// CORE_NEEDS_MANAGER_WITH_CAPABILITY macro, depending on STANDALONE flag 
#ifdef STANDALONE
  #define CORE_NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN)                          \
          CORE_NEEDS_MANAGER_WITH_CAPABILITY_MAIN(LOOPMAN)
#else
  #define CORE_NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN)                          \
          CORE_NEEDS_MANAGER_WITH_CAPABILITY_MAIN(LOOPMAN)                     \
          namespace Gambit { namespace MODULE { const int CAT(FUNCTION,        \
           _registered3) = register_management_req(Functown::FUNCTION); } } 
#endif

/// Main redirection of NEEDS_MANAGER_WITH_CAPABILITY(LOOPMAN) when invoked from 
/// within the core.
#define CORE_NEEDS_MANAGER_WITH_CAPABILITY_MAIN(LOOPMAN)                       \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "NEEDS_MANAGER_WITH_CAPABILITY."))                                          \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling NEEDS_MANAGER_WITH_CAPABILITY. Please check the rollcall header "  \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "NEEDS_MANAGER_WITH_CAPABILITY. Please check the rollcall header for "      \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace Loop                                                       \
          {                                                                    \
            /* Create a safe pointer to the iteration number of the loop this  \
            functor is running within. */                                      \
            omp_safe_ptr<long long> iteration;                                 \
            /* Create a loop-breaking function that can be called to tell the  \
            functor's loop manager that it is time to break. */                \
            void wrapup() { Functown::FUNCTION.breakLoopFromManagedFunctor(); }\
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Set up the runtime commands that register the fact that this FUNCTION \
      requires it be run inside a loop manager with capability LOOPMAN. */     \
      template <>                                                              \
      void rt_register_function_nesting<Tags::FUNCTION> ()                     \
      {                                                                        \
        Functown::FUNCTION.setLoopManagerCapability(STRINGIFY(LOOPMAN));       \
        Pipes::FUNCTION::Loop::iteration = Functown::FUNCTION.iterationPtr();  \
      }                                                                        \
                                                                               \
      /* Create the corresponding initialisation object */                     \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT(FUNCTION,_nesting)                                        \
         (&rt_register_function_nesting<Tags::FUNCTION>);                      \
      }                                                                        \
    }                                                                          \
  }                                                                            \


/// First common component of CORE_DEPENDENCY(DEP, TYPE, MODULE, FUNCTION) and 
/// CORE_START_CONDITIONAL_DEPENDENCY(TYPE).
#define DEPENDENCY_COMMON_1(DEP, TYPE, MODULE, FUNCTION)                       \
                                                                               \
      /* Given that TYPE is not void, create a safety_bucket for the           \
      dependency result. To be initialized automatically at runtime            \
      when the dependency is resolved. */                                      \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          BOOST_PP_IIF(IS_TYPE(void,TYPE), ,                                   \
           namespace Dep {dep_bucket<TYPE> DEP(STRINGIFY(MODULE),              \
           STRINGIFY(FUNCTION),STRINGIFY(DEP));})                              \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Resolve dependency DEP in FUNCTION */                                 \
      template <>                                                              \
      void resolve_dependency<Gambit::Tags::DEP, Tags::FUNCTION>(functor*      \
       dep_functor, module_functor_common* BOOST_PP_IIF(IS_TYPE(void,TYPE), ,  \
       this_functor))                                                          \
      {                                                                        \
        /* First try casting the dep pointer passed in to a module_functor */  \
        module_functor<TYPE> * ptr =                                           \
         dynamic_cast<module_functor<TYPE>*>(dep_functor);                     \
                                                                               \
        /* Now test if that cast worked */                                     \
        if (ptr == 0)  /* It didn't; throw an error. */                        \
        {                                                                      \
          str errmsg = "Null returned from dynamic cast of";                   \
          errmsg +=  "\ndependency functor in MODULE::resolve_dependency, for" \
                     "\ndependency DEP of function FUNCTION.  Attempt was to"  \
                     "\nresolve to " + dep_functor->name() + " in " +          \
                     dep_functor->origin() + ".";                              \
          utils_error().raise(LOCAL_INFO,errmsg);                              \
        }                                                                      \
                                                                               \
        /* It did! Now initialize the safety_bucket using the functors.*/      \
        BOOST_PP_IIF(IS_TYPE(void,TYPE), ,                                     \
          Pipes::FUNCTION::Dep::DEP.initialize(ptr,this_functor);              \
        )                                                                      \
                                                                               \
      }                                                                        \


/// Second common component of CORE_DEPENDENCY(DEP, TYPE, MODULE, FUNCTION) and 
/// CORE_START_CONDITIONAL_DEPENDENCY(TYPE).
#define DEPENDENCY_COMMON_2(DEP,FUNCTION)                                      \
                                                                               \
  /* Create the dependency initialisation object */                            \
  namespace Ini                                                                \
  {                                                                            \
    ini_code CAT_3(DEP,_for_,FUNCTION)                                         \
     (&rt_register_dependency<Gambit::Tags::DEP, Tags::FUNCTION>);             \
  }                                                                            \
                                                                            

/// Redirection of DEPENDENCY(DEP, TYPE) when invoked from within the core.
#define CORE_DEPENDENCY(DEP, TYPE, MODULE, FUNCTION, IS_MODEL_DEP)             \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    /* Add DEP to global set of tags of recognised module capabilities/deps */ \
    ADD_TAG_IN_CURRENT_NAMESPACE(DEP)                                          \
                                                                               \
    /* Put everything inside the Models namespace if this is a model dep */    \
    BOOST_PP_IIF(IS_MODEL_DEP, namespace Models {, )                           \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      DEPENDENCY_COMMON_1(DEP, TYPE, MODULE, FUNCTION)                         \
                                                                               \
      namespace Accessors                                                      \
      {                                                                        \
        /* Indicate that FUNCTION requires DEP to be computed previously*/     \
        template <>                                                            \
        bool requires<Gambit::Tags::DEP, Tags::FUNCTION>() { return true; }    \
      }                                                                        \
                                                                               \
      /* Set up the commands to be called at runtime to register dependency*/  \
      template <>                                                              \
      void rt_register_dependency<Gambit::Tags::DEP, Tags::FUNCTION> ()        \
      {                                                                        \
        Accessors::map_bools[STRINGIFY(CAT(DEP,FUNCTION))] =                   \
         &Accessors::requires<Gambit::Tags::DEP, Tags::FUNCTION>;              \
        Accessors::iMayNeed[STRINGIFY(DEP)] = STRINGIFY(TYPE);                 \
        Functown::FUNCTION.setDependency(STRINGIFY(DEP),STRINGIFY(TYPE),       \
         &resolve_dependency<Gambit::Tags::DEP, Tags::FUNCTION>);              \
      }                                                                        \
                                                                               \
      DEPENDENCY_COMMON_2(DEP, FUNCTION)                                       \
                                                                               \
    }                                                                          \
                                                                               \
    /* Close the Models namespace if this is a model dep */                    \
    BOOST_PP_IIF(IS_MODEL_DEP, }, )                                            \
                                                                               \
  }                                                                            \

/// Redirection of ALLOW_MODEL when invoked from within the core.
#define CORE_ALLOWED_MODEL(MODULE,FUNCTION,MODEL)                              \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "ALLOW_MODEL(S)."))                                                         \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "ALLOW_MODEL(S). Please check the rollcall header for "                     \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    /* Add MODEL to global set of tags of recognised models */                 \
    ADD_MODEL_TAG_IN_CURRENT_NAMESPACE(MODEL)                                  \
    CORE_ALLOWED_MODEL_ARRANGE_DEP(MODULE,FUNCTION,MODEL)                      \
    CORE_ALLOW_MODEL(MODULE,FUNCTION,MODEL)                                    \
  }                                                                            \

/// Redirection of ALLOW_MODEL_DEPENDENCE when invoked from within the core.
#define CORE_ALLOW_MODEL_DEPENDENCE(MODULE,FUNCTION,MODEL)                     \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "ALLOW_MODEL_DEPENDENCE."))                                                 \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "ALLOW_MODEL_DEPENDENCE. Please check the rollcall header for "             \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    /* Add MODEL to global set of tags of recognised models */                 \
    ADD_MODEL_TAG_IN_CURRENT_NAMESPACE(MODEL)                                  \
    CORE_ALLOWED_MODEL_ARRANGE_DEP(MODULE,FUNCTION,MODEL)                      \
  }                                                                            \

/// Set up the dependency on the parameters object of a given model.
#define CORE_ALLOWED_MODEL_ARRANGE_DEP(MODULE,FUNCTION,MODEL)                  \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Create a safety bucket to the model parameter values. To be filled    \
      automatically at runtime when the dependency is resolved. */             \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace Dep                                                        \
          {                                                                    \
            dep_bucket<ModelParameters> CAT(MODEL,_parameters)                 \
             (STRINGIFY(MODULE),STRINGIFY(FUNCTION),                           \
             STRINGIFY(CAT(MODEL,_parameters)));                               \
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Resolve dependency on parameters of MODEL in FUNCTION */              \
      template <>                                                              \
      void resolve_dependency<ModelTags::MODEL, Tags::FUNCTION>                \
       (functor* params_functor, module_functor_common* this_functor)          \
      {                                                                        \
        /* First try casting the pointer passed in to a module_functor */      \
        module_functor<ModelParameters>* ptr =                                 \
         dynamic_cast<module_functor<ModelParameters>*>(params_functor);       \
                                                                               \
        /* Now test if that cast worked */                                     \
        if (ptr == 0)  /* It didn't; throw an error. */                        \
        {                                                                      \
          str errmsg = "Null returned from dynamic cast in";                   \
          errmsg +=  "\nMODULE::resolve_dependency, for model"                 \
                     "\nMODEL with function FUNCTION.  Attempt was to"         \
                     "\nresolve to " + params_functor->name() + " in " +       \
                     params_functor->origin() + ".";                           \
          utils_error().raise(LOCAL_INFO,errmsg);                              \
        }                                                                      \
                                                                               \
        /* It did! Now initialize the safety_bucket using the functors.*/      \
        Pipes::FUNCTION::Dep::CAT(MODEL,_parameters).initialize(ptr,           \
         this_functor);                                                        \
        /* Get a pointer to the parameter map provided by this MODEL */        \
        safe_ptr<ModelParameters> model_safe_ptr =                             \
         Pipes::FUNCTION::Dep::CAT(MODEL,_parameters).safe_pointer();          \
        /* Use that to add the parameters provided by this MODEL to the map    \
        of safe pointers to model parameters. */                               \
        for (std::map<str,double>::const_iterator it = model_safe_ptr->begin();\
         it != model_safe_ptr->end(); ++it)                                    \
        {                                                                      \
          BOOST_PP_IIF(ALLOW_DUPLICATES_IN_PARAMS_MAP, ,                       \
          if (Pipes::FUNCTION::Param.find(it->first) ==                        \
              Pipes::FUNCTION::Param.end())                                    \
          )                                                                    \
          {                                                                    \
            /* Add a safe pointer to the value of this parameter to the map*/  \
            Pipes::FUNCTION::Param.insert(                                     \
             std::pair<str,safe_ptr<const double> >(it->first,                 \
             safe_ptr<const double>(&(it->second)))                            \
            );                                                                 \
          }                                                                    \
          BOOST_PP_IIF(ALLOW_DUPLICATES_IN_PARAMS_MAP, ,                       \
          else                                                                 \
          {                                                                    \
            /* This parameter already exists in the map! Fail. */              \
            str errmsg = "Problem in " STRINGIFY(MODULE) "::resolve_";         \
            errmsg +=    "dependency, for model " STRINGIFY(MODEL)             \
                         " with function\n" STRINGIFY(FUNCTION) ".  Attempt "  \
                         "was to resolve to\n" + params_functor->name() +      \
                         " in " + params_functor->origin()                     \
                         + ".\nYou have tried to scan two models "             \
                         "simultaneously that have one or more\n parameters "  \
                         "in common.\nProblem parameter: " + it->first;        \
            utils_error().raise(LOCAL_INFO,errmsg);                            \
          }                                                                    \
          )                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Set up the commands to be called at runtime to register the           \
      compatibility of the model with the functor */                           \
      template <>                                                              \
      void rt_register_dependency<ModelTags::MODEL, Tags::FUNCTION> ()         \
      {                                                                        \
        Accessors::iMayNeed[STRINGIFY(CAT(MODEL,_parameters))] =               \
         "ModelParameters";                                                    \
        Functown::FUNCTION.setModelConditionalDependency(STRINGIFY(MODEL),     \
         STRINGIFY(CAT(MODEL,_parameters)),"ModelParameters",                  \
         &resolve_dependency<ModelTags::MODEL, Tags::FUNCTION>);               \
      }                                                                        \
                                                                               \
      /* Create the dependency initialisation object */                        \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_3(MODEL,_params_for_,FUNCTION)                            \
        (&rt_register_dependency<ModelTags::MODEL, Tags::FUNCTION>);           \
      }                                                                        \
                                                                               \
    }                                                                          \


/// Tell the functor that a single model is enough for it to be allowed to run.   
#define CORE_ALLOW_MODEL(MODULE,FUNCTION,MODEL)                                \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      namespace Accessors                                                      \
      {                                                                        \
        /* Indicate that FUNCTION can be used with MODEL */                    \
        template <>                                                            \
        bool explicitly_allowed_model<ModelTags::MODEL,Tags::FUNCTION >()      \
        {                                                                      \
          return true;                                                         \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Set up the commands to be called at runtime to register the           \
      compatibility of the model with the functor */                           \
      void CAT_4(rt_register_model_singly_,FUNCTION,_,MODEL)()                 \
      {                                                                        \
        Accessors::model_bools[STRINGIFY(FUNCTION)][STRINGIFY(MODEL)] =        \
         &Accessors::explicitly_allowed_model<ModelTags::MODEL,Tags::FUNCTION>;\
        Functown::FUNCTION.setAllowedModel(STRINGIFY(MODEL));                  \
      }                                                                        \
                                                                               \
      /* Create the model registration initialisation object */                \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_3(MODEL,_allowed_for_,FUNCTION)                           \
         (&CAT_4(rt_register_model_singly_,FUNCTION,_,MODEL));                 \
      }                                                                        \
                                                                               \
    }                                                                          \


/// Redirection of ALLOW_MODEL_COMBINATION when invoked from the Core.
#define CORE_ALLOW_MODEL_COMBINATION(MODULE,FUNCTION,COMBO)                    \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "ALLOW_MODEL_COMBINATION."))                                                \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "ALLOW_MODEL_COMBINATION. Please check the rollcall header for "            \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  /* Register the combination as allowed with the functor */                   \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Set up the commands to be called at runtime to register the           \
      compatibility of the model combination with the functor */               \
      void CAT_4(rt_register_model_combination_,FUNCTION,_,                    \
       BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(COMBO)))))()       \
      {                                                                        \
        Functown::FUNCTION.setAllowedModelGroupCombo(#COMBO);                  \
      }                                                                        \
                                                                               \
      /* Create the dependency initialisation object */                        \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_3(FUNCTION,_,                                             \
         BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(COMBO)))))       \
         (&CAT_4(rt_register_model_combination_,FUNCTION,_,                    \
         BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(COMBO))))));     \
      }                                                                        \
                                                                               \
    }                                                                          \
  }                                                                            \

/// Redirection of DISABLE_MODEL_RELATIONSHIP when invoked from within the Core
#define CORE_DISABLE_MODEL_RELATIONSHIP(MODULE,FUNCTION,MODEL1,MODEL2)         \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
      /* Set up the command to be called at runtime to register the            \
      the relationaship disabled with the functor */                           \
      void CAT_6(rt_register_disable_model_relationship_,FUNCTION,_,MODEL1,    \
       _,MODEL2)()                                                             \
      {                                                                        \
        Functown::FUNCTION.setDisabledModelRelationship(STRINGIFY(MODEL1),     \
          STRINGIFY(MODEL2));                                                  \
      }                                                                        \
                                                                               \
      /* Create the initialisation object*/                                    \
      namespace Ini                                                            \
      {                                                                        \
       ini_code CAT_5(MODEL1,_,MODEL2,_relationship_disabled_for_,FUNCTION)    \
        (&CAT_6(rt_register_disable_model_relationship_,FUNCTION,_,MODEL1,     \
        _,MODEL2));                                                            \
      }                                                                        \
    }                                                                          \
  }                                                                            \


/// Redirection of MODEL_GROUP when invoked from within the Core.
#define CORE_MODEL_GROUP(MODULE,FUNCTION,GROUPNAME,GROUP)                      \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "MODEL_GROUP."))                                                            \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "MODEL_GROUP. Please check the rollcall header for "                        \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  /* Register the group with the functor */                                    \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Set up the commands to be called at runtime to register the           \
      the model group with the functor */                                      \
      void CAT_4(rt_register_model_group_,FUNCTION,_,GROUPNAME)()              \
      {                                                                        \
       Functown::FUNCTION.setModelGroup(STRINGIFY(GROUPNAME),STRINGIFY(GROUP));\
      }                                                                        \
                                                                               \
      /* Create the model group initialisation object */                       \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_3(GROUPNAME,_model_group_in_,FUNCTION)                    \
         (&CAT_4(rt_register_model_group_,FUNCTION,_,GROUPNAME));              \
      }                                                                        \
                                                                               \
    }                                                                          \
  }                                                                            \

/// Redirection of BACKEND_GROUP(GROUP) when invoked from within the Core.
#define CORE_BE_GROUP(GROUP)                                                   \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "BACKEND_GROUP."))                                                          \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling BACKEND_GROUP. Please check the rollcall header "                  \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "BACKEND_GROUP. Please check the rollcall header for "                      \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
    namespace MODULE                                                           \
    {                                                                          \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace BEgroup                                                    \
          {                                                                    \
            /* Declare a safe pointer to the functor's internal register of    \
            which backend requirement is activated from this group. */         \
            safe_ptr<str> GROUP;                                               \
                                                                               \
            /* Define command to be called at runtime to register the group*/  \
            void rt_register_group()                                           \
            {                                                                  \
              GROUP=Functown::FUNCTION.getChosenReqFromGroup(STRINGIFY(GROUP));\
            }                                                                  \
                                                                               \
            /* Create the group initialisation object */                       \
            namespace Ini                                                      \
            {                                                                  \
              ini_code GROUP (&rt_register_group);                             \
            }                                                                  \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }                                                                            \


/// Redirection of BACKEND_REQ(GROUP, REQUIREMENT, (TAGS), TYPE, [(ARGS)]) 
/// for declaring backend requirements when invoked from within the Core.
#define CORE_BACKEND_REQ(MODULE, CAPABILITY, FUNCTION, GROUP, REQUIREMENT,     \
                         TAGS, TYPE, ARGS, IS_VARIABLE)                        \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "BACKEND_REQ."))                                                            \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling BACKEND_REQ. Please check the rollcall header "                    \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "BACKEND_REQ. Please check the rollcall header for "                        \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    /* If scan-level initialisation functions are implemented, the macro should\
    fail here if the user has tried to declare that a scan-level initialisation\
    function has a backend requirement. */                                     \
                                                                               \
    /* Add REQUIREMENT to global set of recognised backend func tags */        \
    ADD_BETAG_IN_CURRENT_NAMESPACE(REQUIREMENT)                                \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
      namespace Pipes                                                          \
      {                                                                        \
        namespace FUNCTION                                                     \
        {                                                                      \
          namespace BEreq                                                      \
          {                                                                    \
            /* Create a safety_bucket for the backend variable/function.       \
            To be initialized by the dependency resolver at runtime. */        \
            typedef BEvariable_bucket<TYPE> CAT(REQUIREMENT,var);              \
            typedef BEfunction_bucket<BOOST_PP_IIF(IS_VARIABLE,int,TYPE(*)     \
             CONVERT_VARIADIC_ARG(ARGS)), TYPE                                 \
             INSERT_NONEMPTY(STRIP_VARIADIC_ARG(ARGS))>                        \
             CAT(REQUIREMENT,func);                                            \
            CAT(REQUIREMENT,BOOST_PP_IIF(IS_VARIABLE,var,func)) REQUIREMENT    \
             (STRINGIFY(MODULE),STRINGIFY(FUNCTION),STRINGIFY(REQUIREMENT));   \
          }                                                                    \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Indicate that FUNCTION has a potential REQUIREMENT */                 \
      namespace Accessors                                                      \
      {                                                                        \
        template <>                                                            \
        bool could_need_from_backend<BETags::REQUIREMENT, Tags::FUNCTION>()    \
        {                                                                      \
          return true;                                                         \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Resolve REQUIREMENT in FUNCTION */                                    \
      template <>                                                              \
      void resolve_backendreq<BETags::REQUIREMENT, Tags::FUNCTION>             \
       (functor* be_functor)                                                   \
      {                                                                        \
        /* Indicate that this is a current backend requirement */              \
        Accessors::map_bools[STRINGIFY(CAT_4(BE_,REQUIREMENT,FUNCTION,now))] = \
         &Accessors::could_need_from_backend<BETags::REQUIREMENT,              \
         Tags::FUNCTION>;                                                      \
                                                                               \
        /* First try casting the pointer passed in to a backend_functor*/      \
        typedef backend_functor<TYPE*(*)(), TYPE*>* var;                       \
        typedef backend_functor<BOOST_PP_IIF(IS_VARIABLE,int,TYPE(*)           \
         CONVERT_VARIADIC_ARG(ARGS)), TYPE                                     \
         INSERT_NONEMPTY(STRIP_VARIADIC_ARG(ARGS))>* func;                     \
        auto ptr =                                                             \
          dynamic_cast<BOOST_PP_IIF(IS_VARIABLE,var,func)>(be_functor);        \
                                                                               \
        /* Now test if that cast worked */                                     \
        if (ptr == 0)  /* It didn't; throw an error. */                        \
        {                                                                      \
          str errmsg = "Null returned from dynamic cast in";                   \
          errmsg +=  "\n" STRINGIFY(MODULE) "::resolve_backendreq, for backend"\
                     " requirement\n" STRINGIFY(REQUIREMENT) " of function "   \
                     STRINGIFY(FUNCTION) ".  Attempt was to"                   \
                     "\nresolve to " + be_functor->name() + " in " +           \
                     be_functor->origin() + ".";                               \
          utils_error().raise(LOCAL_INFO,errmsg);                              \
        }                                                                      \
                                                                               \
        /* It did! Now use the cast functor pointer to initialize              \
        the safety_bucket Pipes::FUNCTION::BEreq::REQUIREMENT. */              \
        Pipes::FUNCTION::BEreq::REQUIREMENT.initialize(ptr);                   \
      }                                                                        \
                                                                               \
      /* Set up the commands to be called at runtime to register req.          \
      (Note that TYPE is used for backend functions, while TYPE* is used       \
      for backend variables.) */                                               \
      template <>                                                              \
      void rt_register_req<BETags::REQUIREMENT, Tags::FUNCTION>()              \
      {                                                                        \
        Accessors::map_bools[STRINGIFY(CAT_3(BE_,REQUIREMENT,FUNCTION))] =     \
         &Accessors::could_need_from_backend<BETags::REQUIREMENT,              \
         Tags::FUNCTION>;                                                      \
                                                                               \
        str varsig = STRINGIFY(TYPE*);                                         \
        str funcsig = STRINGIFY(TYPE) STRINGIFY(CONVERT_VARIADIC_ARG(ARGS));   \
                                                                               \
        Accessors::iMayNeedFromBackends[STRINGIFY(REQUIREMENT)] =              \
          BOOST_PP_IIF(IS_VARIABLE, varsig, funcsig);                          \
                                                                               \
        Functown::FUNCTION.setBackendReq(                                      \
         STRINGIFY(GROUP),                                                     \
         STRINGIFY(REQUIREMENT),                                               \
         #TAGS,                                                                \
         BOOST_PP_IIF(IS_VARIABLE, varsig, funcsig),                           \
         &resolve_backendreq<BETags::REQUIREMENT,Tags::FUNCTION>);             \
                                                                               \
      }                                                                        \
                                                                               \
      /* Create the backend requirement initialisation object */               \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_3(REQUIREMENT,_backend_for_,FUNCTION)                     \
         (&rt_register_req<BETags::REQUIREMENT,Tags::FUNCTION>);               \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of BACKEND_OPTION(BACKEND_AND_VERSIONS, TAGS) when invoked from
/// within the core.
#define CORE_BACKEND_OPTION(BE_AND_VER,TAGS)                                   \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "BACKEND_OPTION."))                                                         \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling BACKEND_OPTION. Please check the rollcall header "                 \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "BACKEND_OPTION. Please check the rollcall header for "                     \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Set up the commands to be called at runtime to apply the rule.*/      \
      void CAT_6(apply_rule_,FUNCTION,_,                                       \
       BOOST_PP_TUPLE_ELEM(0,(STRIP_PARENS(BE_AND_VER))),_,                    \
       BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(TAGS)))) ) ()      \
      {                                                                        \
        Functown::FUNCTION.makeBackendOptionRule(#BE_AND_VER, #TAGS);          \
      }                                                                        \
                                                                               \
      /* Create the rule's initialisation object. */                           \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_5(FUNCTION,_,                                             \
         BOOST_PP_TUPLE_ELEM(0,(STRIP_PARENS(BE_AND_VER))),_,                  \
         BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(TAGS)))) )       \
         (&CAT_6(apply_rule_,FUNCTION,_,                                       \
         BOOST_PP_TUPLE_ELEM(0,(STRIP_PARENS(BE_AND_VER))),_,                  \
         BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(TAGS)))) ) );    \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of FORCE_SAME_BACKEND(TAGS) when invoked from within the core.
#define CORE_FORCE_SAME_BACKEND(...)                                           \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "FORCE_SAME_BACKEND."))                                                     \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling FORCE_SAME_BACKEND. Please check the rollcall header "             \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "FORCE_SAME_BACKEND. Please check the rollcall header for "                 \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Set up the commands to be called at runtime to apply the rule.*/      \
      void CAT_4(apply_rule_,FUNCTION,_,                                       \
       BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(__VA_ARGS__))))) ()\
      {                                                                        \
        Functown::FUNCTION.makeBackendMatchingRule(#__VA_ARGS__);              \
      }                                                                        \
                                                                               \
      /* Create the rule's initialisation object. */                           \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_3(FUNCTION,_,                                             \
         BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(__VA_ARGS__))))) \
         (&CAT_4(apply_rule_,FUNCTION,_,BOOST_PP_SEQ_CAT(                      \
         BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(__VA_ARGS__))))));                \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \



/// Redirection of START_CONDITIONAL_DEPENDENCY(TYPE) when invoked from within 
/// the core.
#define CORE_START_CONDITIONAL_DEPENDENCY(MODULE, CAPABILITY, FUNCTION,        \
 CONDITIONAL_DEPENDENCY, TYPE)                                                 \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "START_CONDITIONAL_DEPENDENCY."))                                           \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling START_CONDITIONAL_DEPENDENCY. Please check the rollcall header "   \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "START_CONDITIONAL_DEPENDENCY. Please check the rollcall header for "       \
   STRINGIFY(MODULE) "."))                                                     \
  IF_TOKEN_UNDEFINED(CONDITIONAL_DEPENDENCY,FAIL("You must define "            \
   "CONDITIONAL_DEPENDENCY before calling START_CONDITIONAL_DEPENDENCY. Please"\
   " check the rollcall header for " STRINGIFY(MODULE) "."))                   \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    /* Add DEP to global set of tags of recognised module capabilities/deps */ \
    ADD_TAG_IN_CURRENT_NAMESPACE(CONDITIONAL_DEPENDENCY)                       \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      DEPENDENCY_COMMON_1(CONDITIONAL_DEPENDENCY, TYPE, MODULE, FUNCTION)      \
                                                                               \
      /* Set up the first set of commands to be called at runtime to register  \
      the conditional dependency. */                                           \
      template <>                                                              \
      void rt_register_dependency                                              \
       <Gambit::Tags::CONDITIONAL_DEPENDENCY, Tags::FUNCTION> ()               \
      {                                                                        \
        Accessors::iMayNeed[STRINGIFY(CONDITIONAL_DEPENDENCY)]=STRINGIFY(TYPE);\
      }                                                                        \
                                                                               \
      /* Create the first conditional dependency initialisation object */      \
      DEPENDENCY_COMMON_2(CONDITIONAL_DEPENDENCY, FUNCTION)                    \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \

                                                                               
/// Redirection of ACTIVATE_DEP_BE(BACKEND_REQ, BACKEND, VERSTRING) when 
/// invoked from within the core.
#define CORE_ACTIVATE_DEP_BE(BACKEND_REQ, BACKEND, VERSTRING)                  \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "ACTIVATE_FOR_BACKEND."))                                                   \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling ACTIVATE_FOR_BACKEND. Please check the rollcall header "           \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "ACTIVATE_FOR_BACKEND. Please check the rollcall header for "               \
   STRINGIFY(MODULE) "."))                                                     \
  IF_TOKEN_UNDEFINED(CONDITIONAL_DEPENDENCY,FAIL("You must define "            \
   "CONDITIONAL_DEPENDENCY before calling ACTIVATE_FOR_BACKEND. Please"        \
   " check the rollcall header for " STRINGIFY(MODULE) "."))                   \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    /* Add BACKEND to global set of recognised backend tags */                 \
    ADD_BETAG_IN_CURRENT_NAMESPACE(BACKEND)                                    \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      namespace Accessors                                                      \
      {                                                                        \
        /* Indicate that FUNCTION requires CONDITIONAL_DEPENDENCY to have      \
        been computed previously if BACKEND is in use for BACKEND_REQ.*/       \
        template <>                                                            \
        bool requires_conditional_on_backend                                   \
         <Gambit::Tags::CONDITIONAL_DEPENDENCY, Tags::FUNCTION,                \
         BETags::BACKEND_REQ, BETags::BACKEND> (str ver)                       \
        {                                                                      \
          typedef std::vector<str> vec;                                        \
          vec versions = Utils::delimiterSplit(VERSTRING, ",");                \
          for (vec::iterator it= versions.begin() ; it != versions.end(); ++it)\
          {                                                                    \
            if (*it == ver) return true;                                       \
          }                                                                    \
          return false;                                                        \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Set up the second set of commands to be called at runtime to register \
      the conditional dependency. */                                           \
      template <>                                                              \
      void rt_register_conditional_dependency                                  \
       <Gambit::Tags::CONDITIONAL_DEPENDENCY, Tags::FUNCTION,                  \
       BETags::BACKEND_REQ, BETags::BACKEND> ()                                \
      {                                                                        \
        Accessors::condit_bools[STRINGIFY(CAT_4(CONDITIONAL_DEPENDENCY,        \
         FUNCTION,BACKEND_REQ,BACKEND))] =                                     \
         &Accessors::requires_conditional_on_backend                           \
         <Gambit::Tags::CONDITIONAL_DEPENDENCY, Tags::FUNCTION,                \
         BETags::BACKEND_REQ, BETags::BACKEND>;                                \
        Functown::FUNCTION.setBackendConditionalDependency                     \
         (STRINGIFY(BACKEND_REQ), STRINGIFY(BACKEND), VERSTRING,               \
         STRINGIFY(CONDITIONAL_DEPENDENCY),                                    \
         Accessors::iMayNeed[STRINGIFY(CONDITIONAL_DEPENDENCY)],               \
         &resolve_dependency<Gambit::Tags::CONDITIONAL_DEPENDENCY,             \
         Tags::FUNCTION>);                                                     \
      }                                                                        \
                                                                               \
      /* Create the second conditional dependency initialisation object */     \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_7(CONDITIONAL_DEPENDENCY,_for_,FUNCTION,_with_,           \
         BACKEND_REQ,_provided_by_,BACKEND)                                    \
         (&rt_register_conditional_dependency                                  \
         <Gambit::Tags::CONDITIONAL_DEPENDENCY, Tags::FUNCTION,                \
         BETags::BACKEND_REQ, BETags::BACKEND>);                               \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of ACTIVATE_BACKEND_REQ_FOR_MODELS when invoked from the Core. 
#define CORE_BE_MODEL_RULE(MODELS,TAGS)                                        \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "ACTIVATE_BACKEND_REQ_FOR_MODEL(S)."))                                      \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling ACTIVATE_BACKEND_REQ_FOR_MODEL(S). Please check the rollcall heade"\
   "r for " STRINGIFY(MODULE) "."))                                            \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "ACTIVATE_BACKEND_REQ_FOR_MODEL(S). Please check the rollcall header for "  \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      /* Apply the rule.*/                                                     \
      const int CAT_6(apply_rule_,FUNCTION,_,                                  \
       BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(MODELS)))),_,      \
       BOOST_PP_SEQ_CAT(BOOST_PP_TUPLE_TO_SEQ((STRIP_PARENS(TAGS)))) ) =       \
       set_backend_rule_for_model(Functown::FUNCTION,#MODELS,#TAGS);           \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of ACTIVATE_FOR_MODELS(MODELSTRING) when invoked from within 
/// the core, inside a CONDITIONAL_DEPENDENCY definition.
#define ACTIVATE_DEP_MODEL(MODULE, CAPABILITY, FUNCTION,                       \
 CONDITIONAL_DEPENDENCY,MODELSTRING)                                           \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "ACTIVATE_FOR_MODEL(S)."))                                                  \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling ACTIVATE_FOR_MODEL(S). Please check the rollcall header "          \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "ACTIVATE_FOR_MODEL(S). Please check the rollcall header for "              \
   STRINGIFY(MODULE) "."))                                                     \
  IF_TOKEN_UNDEFINED(CONDITIONAL_DEPENDENCY,FAIL("You must define "            \
  "CONDITIONAL_DEPENDENCY before calling ACTIVATE_FOR_MODEL(S)."               \
  " Please check the rollcall header for " STRINGIFY(MODULE) "."))             \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      namespace Accessors                                                      \
      {                                                                        \
        /* Indicate that FUNCTION requires CONDITIONAL_DEPENDENCY to be        \
        computed previously if one of the models in MODELSTRING is scanned.*/  \
        template <>                                                            \
        bool requires_conditional_on_model                                     \
         <Gambit::Tags::CONDITIONAL_DEPENDENCY,Tags::FUNCTION> (str model)     \
        {                                                                      \
          typedef std::vector<str> vec;                                        \
          vec models = Utils::delimiterSplit(MODELSTRING, ",");                \
          for (vec::iterator it = models.begin() ; it != models.end(); ++it)   \
          {                                                                    \
            if (*it == model) return true;                                     \
          }                                                                    \
          return false;                                                        \
        }                                                                      \
      }                                                                        \
                                                                               \
      /* Set up the second set of commands to be called at runtime to register \
      the conditional dependency. */                                           \
      template <>                                                              \
      void rt_register_conditional_dependency                                  \
       <Gambit::Tags::CONDITIONAL_DEPENDENCY, Tags::FUNCTION> ()               \
      {                                                                        \
        Accessors::condit_bools[STRINGIFY(CAT(CONDITIONAL_DEPENDENCY,          \
         FUNCTION))] = &Accessors::requires_conditional_on_model               \
         <Gambit::Tags::CONDITIONAL_DEPENDENCY,Tags::FUNCTION>;                \
                                                                               \
        Functown::FUNCTION.setModelConditionalDependency                       \
         (MODELSTRING, STRINGIFY(CONDITIONAL_DEPENDENCY),                      \
         Accessors::iMayNeed[STRINGIFY(CONDITIONAL_DEPENDENCY)],               \
         &resolve_dependency<Gambit::Tags::CONDITIONAL_DEPENDENCY,             \
         Tags::FUNCTION>);                                                     \
      }                                                                        \
                                                                               \
      /* Create the second conditional dependency initialisation object */     \
      namespace Ini                                                            \
      {                                                                        \
        ini_code CAT_4(CONDITIONAL_DEPENDENCY,_for_,FUNCTION,_with_models)     \
         (&rt_register_conditional_dependency                                  \
         <Gambit::Tags::CONDITIONAL_DEPENDENCY, Tags::FUNCTION>);              \
      }                                                                        \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \


/// Redirection of NEEDS_CLASSES_FROM when invoked from within the Core.
#define CORE_CLASSLOAD_NEEDED(BACKEND, VERSTRING)                              \
                                                                               \
  IF_TOKEN_UNDEFINED(MODULE,FAIL("You must define MODULE before calling "      \
   "NEEDS_CLASSES_FROM."))                                                     \
  IF_TOKEN_UNDEFINED(CAPABILITY,FAIL("You must define CAPABILITY before "      \
   "calling NEEDS_CLASSES_FROM. Please check the rollcall header "             \
   "for " STRINGIFY(MODULE) "."))                                              \
  IF_TOKEN_UNDEFINED(FUNCTION,FAIL("You must define FUNCTION before calling "  \
   "NEEDS_CLASSES_FROM. Please check the rollcall header for "                 \
   STRINGIFY(MODULE) "."))                                                     \
                                                                               \
  namespace Gambit                                                             \
  {                                                                            \
                                                                               \
    namespace MODULE                                                           \
    {                                                                          \
                                                                               \
      const int CAT_4(classloading_from_,BACKEND,_for_,FUNCTION) =             \
       set_classload_requirements(Functown::FUNCTION, STRINGIFY(BACKEND),      \
       VERSTRING, STRINGIFY(CAT(Default_,BACKEND)));                           \
                                                                               \
    }                                                                          \
                                                                               \
  }                                                                            \

/// @}

#endif // defined __core_module_macros_incore_hpp__ 

