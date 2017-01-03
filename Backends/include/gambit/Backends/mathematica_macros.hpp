//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Macros for creating mathematica functions and
///  sending and receiving packets through WSTP
///
///  *********************************************
///
///  Authos (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (t.e.gonzalo@fys.uio.no)
///  \date 2016 Oct
///
///  *********************************************

#ifndef __MATHEMATICA_MACROS_HPP__
#define __MATHEMATICA_MACROS_HPP__

#include "gambit/Utils/util_macros.hpp"
#include "gambit/Backends/mathematica_variable.hpp"

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>

/// If not defined already, define Mathematica
#ifndef MATHEMATICA
#define MATHEMATICA 3
#endif

/// Macro to help identifying the language of the backend
#ifndef DEFINED_BACKENDLANG 
#define DEFINED_BACKENDLANG ()
#endif

/// Macro to choose between mathematica types and normal types
#ifdef HAVE_MATHEMATICA
  #define MATH_TYPE(TYPE) BOOST_PP_IF(USING_MATHEMATICA,mathematica_variable<TYPE>,TYPE)
#else
  #define MATH_TYPE(TYPE) TYPE
#endif

/// Macro that determines whether the language of the backend is mathematica
#define USING_MATHEMATICA IF_ELSE_TOKEN_DEFINED(BACKENDLANG,                                    \
        BOOST_PP_EQUAL(BACKENDLANG, MATHEMATICA), 0)

/// Macros to give names to an argument list
#define ARG_NAME(R,DATA,INDEX,ELEM) (ELEM arg##INDEX)
#define FUNCTION_ARGS_SEQ(ARGLIST) BOOST_PP_IF(ISEMPTY(ARGLIST), (),                            \
        BOOST_PP_SEQ_FOR_EACH_I(ARG_NAME, , BOOST_PP_TUPLE_TO_SEQ(ARGLIST)))
#define FUNCTION_ARGS(ARGLIST) BOOST_PP_SEQ_TO_TUPLE(FUNCTION_ARGS_SEQ(ARGLIST))

/// Macros for identifying WSTP types
#define WSGET(VAR) WSGetVariable((WSLINK)pHandle, VAR)
#define WSPUT(VAR) WSPutVariable((WSLINK)pHandle, VAR)

#define WSARG(TYPE, ARG)                                                                        \
  BOOST_PP_IF(IS_TYPE(bool, TYPE), ARG ? "True" : "False",                                      \
    BOOST_PP_IF(IS_TYPE(string, TYPE), ARG.c_str(), ARG))

// Macros for stripping to basic types
#define STRIP_void void
#define STRIP_int int
#define STRIP_float float
#define STRIP_double double
#define STRIP_bool bool
#define STRIP_char char
#define STRIP_str string
#define STRIP_std std
#define STRIP_const DUMMY
#define STRIP_CONST(TYPE) STRIP_CONST_I(TYPE)
#define STRIP_CONST_I(TYPE) CAT(STRIP_,TYPE)
#define STRIP(TYPE) STRIP_CONST(STRIP_CONST(TYPE))

/// Macro for handling errors
#define MATH_ERROR(TYPE,ERROR)                                                                  \
  backend_warning().raise(LOCAL_INFO, ERROR);                                                   \
  if(WSError((WSLINK)pHandle))                                                                  \
  {                                                                                             \
    backend_warning().raise(LOCAL_INFO, WSErrorMessage((WSLINK)pHandle));                       \
    WSClearError((WSLINK)pHandle);                                                              \
    WSNewPacket((WSLINK)pHandle);                                                               \
  }                                                                                             \
  else                                                                                          \
    backend_warning().raise(LOCAL_INFO, "Type unknown or incompatible with WSTP");              \
  BOOST_PP_IF(IS_TYPE(void, STRIP(TYPE)), return ;, return TYPE();)
 
/// Macros for sending data through WSTP
#define WSPUTARG(R, TYPE, INDEX, ELEM)                                                          \
  if(!WSPutVariable((WSLINK)pHandle,CAT(arg,INDEX)))                                            \
  {                                                                                             \
      MATH_ERROR(TYPE,"Error sending packet through WSTP");                                     \
  }                                                                                             \
  else (void)CAT(arg,INDEX);
  

/// Dummy macro for arguments to skip compiler warnings
#define VOIDARG(R, DATA, INDEX, ELEM) (void)CAT(arg,INDEX);

/// Backend function macro for mathematica
#ifdef HAVE_MATHEMATICA
  #define BE_FUNCTION_I_MATH(NAME, TYPE, ARGLIST, SYMBOLNAME, CAPABILITY, MODELS)               \
  namespace Gambit                                                                              \
  {                                                                                             \
    namespace Backends                                                                          \
    {                                                                                           \
      namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)                                               \
      {                                                                                         \
                                                                                                \
        /* Define a type NAME_type to be a suitable function pointer. */                        \
        typedef TYPE (*NAME##_type) CONVERT_VARIADIC_ARG(ARGLIST);                              \
                                                                                                \
        TYPE NAME##_function FUNCTION_ARGS(ARGLIST)                                             \
        {                                                                                       \
                                                                                                \
          try                                                                                   \
          {                                                                                     \
            /* If TYPE is a numeric type, send N first */                                       \
            if(IS_TYPE(TYPE, int) or IS_TYPE(TYPE, float) or IS_TYPE(TYPE, double))             \
              if(!WSPutFunction((WSLINK)pHandle, "N", 1))                                       \
              {                                                                                 \
                MATH_ERROR(TYPE,"Error sending packet throught WSTP")                           \
              }                                                                                 \
                                                                                                \
            /* Send the symbol name next */                                                     \
            int size = BOOST_PP_IF(ISEMPTY(ARGLIST),0,BOOST_PP_TUPLE_SIZE(ARGLIST));            \
            if(!WSPutFunction((WSLINK)pHandle, SYMBOLNAME, size))                               \
            {                                                                                   \
              MATH_ERROR(TYPE,"Error sending packet through WSTP")                              \
            }                                                                                   \
                                                                                                \
            /* Now send all the arguments */                                                    \
            BOOST_PP_IF(ISEMPTY(ARGLIST),,                                                      \
              BOOST_PP_SEQ_FOR_EACH_I(WSPUTARG, TYPE, BOOST_PP_TUPLE_TO_SEQ(ARGLIST)))          \
                                                                                                \
            /* Last, mark the end of the message */                                             \
            if(!WSEndPacket((WSLINK)pHandle))                                                   \
            {                                                                                   \
              MATH_ERROR(TYPE,"Error sending packet through WSTP")                              \
            }                                                                                   \
                                                                                                \
            /* Wait to receive a packet from the kernel */                                      \
            int pkt;                                                                            \
            while( (pkt = WSNextPacket((WSLINK)pHandle), pkt) && pkt != RETURNPKT)              \
            {                                                                                   \
              WSNewPacket((WSLINK)pHandle);                                                     \
              if (WSError((WSLINK)pHandle)) {                                                   \
                MATH_ERROR(TYPE,"Error reading packet from WSTP")                               \
              }                                                                                 \
            }                                                                                   \
                                                                                                \
            /* Read the received packet into the return value, unless it's void */              \
            BOOST_PP_IF(IS_TYPE(void, STRIP(TYPE)),                                             \
              WSNewPacket((WSLINK)pHandle);,                                                    \
              TYPE val;                                                                         \
              if(!WSGetVariable((WSLINK)pHandle, &val))                                         \
              {                                                                                 \
                MATH_ERROR(TYPE,"Error reading packet from WSTP")                               \
              }                                                                                 \
              BOOST_PP_IF(IS_TYPE(void, STRIP(TYPE)), return ;, return val;)                    \
            )                                                                                   \
          }                                                                                     \
          catch (std::exception& e) { ini_catch(e); }                                           \
                                                                                                \
          BOOST_PP_IF(IS_TYPE(void, STRIP(TYPE)), return ;, return TYPE();)                     \
        }                                                                                       \
                                                                                                \
        extern const NAME##_type NAME = NAME##_function;                                        \
                                                                                                \
      }                                                                                         \
    }                                                                                           \
  }
#else
  /* If Mathematica is not available in the system return a dummy funtion */
  #define BE_FUNCTION_I_MATH(NAME, TYPE, ARGLIST, SYMBOLNAME, CAPABILITY, MODELS)               \
  namespace Gambit                                                                              \
  {                                                                                             \
    namespace Backends                                                                          \
    {                                                                                           \
      namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)                                               \
      {                                                                                         \
                                                                                                \
        /* Define a type NAME_type to be a suitable function pointer. */                        \
        typedef TYPE (*NAME##_type) CONVERT_VARIADIC_ARG(ARGLIST);                              \
                                                                                                \
        TYPE NAME##_function FUNCTION_ARGS(ARGLIST)                                             \
        {                                                                                       \
                                                                                                \
          /* Do something inconsequential with the args to skip compiler warnings. */           \
          BOOST_PP_IF(ISEMPTY(ARGLIST),,                                                        \
            BOOST_PP_SEQ_FOR_EACH_I(VOIDARG, , BOOST_PP_TUPLE_TO_SEQ(ARGLIST)))                 \
                                                                                                \
          /* Read the received packet into the return value, unless it's void */                \
          BOOST_PP_IF(IS_TYPE(void, STRIP(TYPE)), DUMMY, return TYPE();                         \
          )                                                                                     \
        }                                                                                       \
                                                                                                \
        extern const NAME##_type NAME = NAME##_function;                                        \
                                                                                                \
      }                                                                                         \
    }                                                                                           \
  }
#endif

/// Backend variable macro for Mathematica
#ifdef HAVE_MATHEMATICA
  #define BE_VARIABLE_I_MATH(NAME, TYPE, SYMBOLNAME, CAPABILITY, MODELS)                        \
  namespace Gambit                                                                              \
  {                                                                                             \
    namespace Backends                                                                          \
    {                                                                                           \
      namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)                                               \
      {                                                                                         \
                                                                                                \
        extern mathematica_variable<TYPE>* const NAME =                                         \
          new mathematica_variable<TYPE>((WSLINK)pHandle, SYMBOLNAME);                          \
                                                                                                \
        mathematica_variable<TYPE>* CAT(getptr,NAME)() { return NAME; }                         \
                                                                                                \
      }                                                                                         \
    }                                                                                           \
  }
#else
  /* If Mathematica is not available in the system, define a dummy variable */
  #define BE_VARIABLE_I_MATH(NAME, TYPE, SYMBOLNAME, CAPABILITY, MODELS)                        \
  namespace Gambit                                                                              \
  {                                                                                             \
    namespace Backends                                                                          \
    {                                                                                           \
      namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)                                               \
      {                                                                                         \
                                                                                                \
        extern TYPE* const NAME = new TYPE();                                                   \
                                                                                                \
        TYPE* CAT(getptr,NAME)() { return NAME; }                                               \
                                                                                                \
      }                                                                                         \
    }                                                                                           \
  }
#endif

#endif // __MATHEMATICA_MACROS_HPP__
