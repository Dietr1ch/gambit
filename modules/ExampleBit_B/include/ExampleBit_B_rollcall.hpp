//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for module ExampleBit_B.
///
///  Compile-time registration of available
///  observables and likelihoods, as well as their
///  dependencies.
///
///  Add to this if you want to add an observable
///  or likelihood to this module.
///
///  Don't put typedefs or other type definitions
///  in this file; see 
///  Core/include/types_rollcall.hpp for further
///  instructions on how to add new types.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott
///  \date 2012 Nov  
///  \date 2013 Jan, Feb, April, May
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2013 Nov
///  *********************************************
///
/// \def MODULE
/// Specifies that this is the MODULE named ExampleBit_B.
///
/// \def CAPABILITY
/// Specifies that ExampleBit_B (this MODULE) is 
/// capable of providing the service CAPABILITY.  
/// Usually a CAPABILITY means that the MODULE can 
/// compute some physical or statistical quantity 
/// referred to by CAPABILITY.   
///
/// \def FUNCTION
/// Specifies that ExampleBit_B (this MODULE) contains
/// a function named FUNCTION that can execute the
/// commands required to provided the current CAPABILITY.
///
/// \def BACKEND_REQ
/// Specifies that the current FUNCTION in 
/// ExampleBit_B (this MODULE) requires BACKEND_REQ from
/// a backend code.  The requirement BACKEND_REQ corresponds
/// to a capability that a valid backend function 
/// is expected to report in its corresponding 
/// registration header file.
///
/// \def CONDITIONAL_DEPENDENCY
/// Specifies that the current FUNCTION in 
/// ExampleBit_B (this MODULE) has a CONDITIONAL_DEPENDENCY
/// that is only active under certain conditions.
/// These conditions may include:
///  - a specific backend is in use in order to fill
///    a certain BACKEND_REQ
///  - a certain model is under analysis 


#ifndef __ExampleBit_B_rollcall_hpp__
#define __ExampleBit_B_rollcall_hpp__

#define MODULE ExampleBit_B
START_MODULE

  #define CAPABILITY xsection               // Observable: cross-section for some hypothetical process
  START_CAPABILITY

    #define FUNCTION xsection               // Name of specific function providing the observable
    START_FUNCTION(double)                  // Function calculates a double precision variable
    ALLOW_MODELS(CMSSM_demo,NormalDist)
    #undef FUNCTION

  #undef CAPABILITY


  #define CAPABILITY charge                 // Observable: charge of some particle
  START_CAPABILITY

    #define FUNCTION exampleCharge          // Name of specific function providing the observable
    START_FUNCTION(int)                     // Function calculates an integer variable
    ALLOW_MODEL(NormalDist)
    ALLOW_MODELS(MSSM_demo, TWOHDM, UED)    // Function is only allowed to be used with the MSSM, 2HDM, UED and their descendents
    #undef FUNCTION

  #undef CAPABILITY


  #define CAPABILITY nevents_postcuts       // Observable: number of events for process after cuts 
  START_CAPABILITY
  //LATEX_LABEL($n_{\rm events, cut}$)      // Specify the LaTeX label of this quantity

    #define FUNCTION nevents_postcuts       // Name of specific function providing the observable
    START_FUNCTION(int)                     // Function calculates an integer variable                  
    DEPENDENCY(nevents, double)             // Dependency: post-cut events needs pre-cut events
    DEPENDENCY(function_pointer, fptr)      // Dependency: some function pointer
    
    BACKEND_REQ(awesomeness, (lib123), double, (int))
    BACKEND_REQ(refex, (common_be), double, (double&))
    BACKEND_REQ(refex2, (common_be), void, (double&, double))
    BACKEND_REQ(varex, (common_be), double, (int, etc))
    BACKEND_REQ(varex2, (common_be), double, (int, etc))
    BACKEND_REQ(runMe, (), void, (double (*)(int&), int&))
    BACKEND_REQ(SomeInt, (model_dependent_reqs, libfirst1_only), int)
    BACKEND_REQ(someFunction, (libfirst1_only, common_be), void, ())

    ACTIVATE_BACKEND_REQ_FOR_MODELS( (CMSSM_demo, UED), (model_dependent_reqs) )
    BACKEND_OPTION( (LibFirst, 1.1), (libfirst1_only, lib123) )
    BACKEND_OPTION( (LibSecond), (lib123) )
    BACKEND_OPTION( (LibThird, 1.2, 1.3 , 1.5), (lib123) )
    FORCE_SAME_BACKEND(common_be)

    #define CONDITIONAL_DEPENDENCY id     // A dependency that only counts under certain conditions (must come after all BACKEND_REQs)
    START_CONDITIONAL_DEPENDENCY(std::string)              // Type of the dependency; one type permitted per CONDITIONAL_DEPENDENCY.
    ACTIVATE_FOR_BACKEND(awesomeness, LibFirst, 1.1, 1.2)  // Dependency counts if awesomeness comes from LibFirst v1.0 or 1.2 
    ACTIVATE_FOR_BACKEND(awesomeness, LibThird)            // Dependency counts when any version of LibThird is used for awesomeness
    ACTIVATE_FOR_MODEL(MSSM_demo)                          // Dependency counts when scanning the MSSM or one of its sub-models
    #undef CONDITIONAL_DEPENDENCY

    #undef FUNCTION

  #undef CAPABILITY


  #define CAPABILITY id
  START_CAPABILITY

    #define FUNCTION identity               // Observable: name of a particle    
    START_FUNCTION(std::string)                
    #undef FUNCTION

  #undef CAPABILITY


  #define CAPABILITY nevents
  START_CAPABILITY

    #define FUNCTION nevents                // Observable: num. events, defined as integer to confuse the dep resolver
    START_FUNCTION(int)                     // Function calculates an integer variable
    #undef FUNCTION

  #undef CAPABILITY

  #define CAPABILITY ptrmeth_arr_tester
  START_CAPABILITY
    #define FUNCTION ptrMethArrTester
    START_FUNCTION(int)
    DEPENDENCY(test_vector, std::vector<double>)
    BACKEND_REQ(test_vector, (), std::vector<double>)
    BACKEND_REQ(SomeArray, (), dblarr)
    #undef FUNCTION
  #undef CAPABILITY

#undef MODULE


// QUICK_FUNCTION examples
// Arguments: MODULE, CAPABILITY, NEW_CAPABILITY_FLAG, FUNCTION, TYPE, (n x ALLOWED_MODEL), m x (DEPENDENCY, DEPENDENCY_TYPE)
//            The last two arguments are optional, and n and m can be anything from 0 to 10.
QUICK_FUNCTION(ExampleBit_B, test_vector, NEW_CAPABILITY, exampleVec, std::vector<double>)                          // Observable: test vector of doubles
QUICK_FUNCTION(ExampleBit_B, lnL_ExampleBitB, NEW_CAPABILITY, lnL_ExampleBitB, double, (), (nevents_postcuts, int)) // Likelihood of type double that depends on postcuts

// Equivalent to:
//   #define CAPABILITY test_vector
//   START_CAPABILITY
//     #define FUNCTION exampleVec             
//     START_FUNCTION(std::vector<double>)
//     #undef FUNCTION
//   #undef CAPABILITY
//   #define CAPABILITY lnL_ExampleBitB
//   START_CAPABILITY
//     #define FUNCTION lnL_ExampleBitB
//     START_FUNCTION(double)
//     DEPENDENCY(nevents_postcuts, int)
//   #undef FUNCTION
//   #undef CAPABILITY


#endif // defined(__ExampleBit_B_rollcall_hpp__)

