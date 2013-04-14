/* 
 * Example of how to use the macros in 'backend_general.hpp' 
 * to set up a backend for a specific library.
 * 
 * \author Anders Kvellestad
 * \date 2013-03-26  
 * 
 * Modified: 2013-04-05
 */

#include "backend_general.hpp"


/* Specify the path to the shared library along with a backend name. */

#define LIBPATH      "./libfirst.so"
#ifdef BACKENDRENAME
  #define BACKENDNAME BACKENDRENAME
#else
  #define BACKENDNAME LibFirst
#endif


/* The following macro loads the library (using dlmopen) in LIBPATH 
 * when this header file is included somewhere. */

LOAD_LIBRARY

/* Next we use macros BE_VARIABLE and BE_FUNCTION to load pointers 
 * (using dlsym) to the variables and functions within the library.
 *  
 * These macros also set up a minimal interface providing 'get/set'
 * functions for the library variables and function pointers 
 * for the library functions.
 *  
 * Syntax for BE_VARIABLE:
 * BE_VARIABLE([choose variable name], [type], "[exact symbol name]", [choose pointer name])  */

BE_VARIABLE(SomeInt, int, "someInt", pSomeInt)
BE_VARIABLE(SomeDouble, double, "someDouble", pSomeDouble)

/* We have now set up the pointers
 *
 * GAMBIT::Backend::BACKENDNAME::pSomeInt     (int *)
 * GAMBIT::Backend::BACKENDNAME::pSomeDouble  (double *)
 * 
 * and the corresponding get/set functions
 * 
 * int  GAMBIT::Backend::BACKENDNAME::getSomeInt()
 * void GAMBIT::Backend::BACKENDNAME::setSomeInt(int)
 *
 * double GAMBIT::Backend::BACKENDNAME::getSomeDouble()
 * void   GAMBIT::Backend::BACKENDNAME::setSomeDouble(double)  */
  

/* Syntax for BE_FUNCTION:
 * BE_FUNCTION([choose function name], [type], [arguement types], "[exact symbol name]")  */

BE_FUNCTION(initialize, void, (int), "_Z10initializei")
BE_FUNCTION(someFunction, void, (), "_Z12someFunctionv")
BE_FUNCTION(returnResult, double, (), "_Z12returnResultv")

/* We have now created the following function pointers:
 *
 * GAMBIT::Backend::BACKENDNAME::initialize       (* void)(int)
 * GAMBIT::Backend::BACKENDNAME::someFunction     (* void)()
 * GAMBIT::Backend::BACKENDNAME::returnResult     (* double)()
 *
 * Calling the function of a function pointer is 
 * exactly like calling the function itself, e.g.
 *
 * GAMBIT::Backend::BACKENDNAME::initialize(10)
 */ 


/* At this point we have a minimal interface to the loaded library.
 * Any additional convenince functions could be constructed below 
 * using the available pointers. */


namespace GAMBIT
{
  namespace Backend
  {
    namespace BACKENDNAME
    {


      /* Convenience functions go here */


    } /* end namespace BACKENDNAME */                                          
  } /* end namespace Backend */                                                
} /* end namespace GAMBIT */                                                   


// Undefine macros to avoid conflict with other backends
#undef LIBPATH 
#undef BACKENDNAME

