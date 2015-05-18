/* 
 * Frontend for HiggsSignals backend
 * 
 * Last modified:
 * Christopher Rogan 2014-08-28
 */

#ifdef BACKENDRENAME
  #define BACKENDNAME BACKENDRENAME
#else
  #define BACKENDNAME HiggsSignals
#endif
#define VERSION 1.2
#define SAFE_VERSION 1_2

/* The following macro loads the library using dlopen 
 * when this header file is included somewhere. */

LOAD_LIBRARY

/* Next we use macros BE_VARIABLE and BE_FUNCTION to load pointers 
 * (using dlsym) to the variables and functions within the library.
 *  
 * The macros also set up a minimal interface providing 'get/set'
 * functions for the library variables and function pointers 
 * for the library functions.
 *  
 * These functions are then wrapped in functors that the core can connect 
 * to the modules via the rollcall system */
 
/* Syntax for BE_FUNCTION:
 * BE_FUNCTION([choose function name], [type], [arguement types], "[exact symbol name]", "[choose capability name]")
 * 
 * The last argument (capability name) is optional. 
 * If left out (as done below) it will default to "[backend name]_[function name]_capability"
 * (e.g. "LibFirst_initialize_capability")  */

BE_FUNCTION(initialize_HiggsSignals_latestresults, void, (int&, int&), "initialize_higgssignals_latestresults_", "initialize_HiggsSignals_latestresults")
BE_FUNCTION(initialize_HiggsBounds_int_HS, void, (int&, int&, int&), "initialize_higgsbounds_int_", "initialize_HiggsBounds_int_HS")
BE_FUNCTION(setup_pdf, void, (int&), "setup_pdf_", "setup_pdf")
BE_FUNCTION(run_HiggsSignals, void, (int&, double&, double&, double&, int&, double&), "run_higgssignals_", "run_HiggsSignals")
BE_FUNCTION(HiggsSignals_neutral_input_MassUncertainty, void, (double*), "higgssignals_neutral_input_massuncertainty_", "HiggsSignals_neutral_input_MassUncertainty")
BE_FUNCTION(setup_rate_uncertainties, void, (double*, double*), "setup_rate_uncertainties_", "setup_rate_uncertainties")
BE_FUNCTION(finish_HiggsSignals, void, (), "finish_higgssignals_", "finish_HiggsSignals")
BE_FUNCTION(finish_HiggsBounds_HS, void, (), "finish_higgsbounds_", "finish_HiggsBounds_HS")

// input parameter functions
BE_FUNCTION(HiggsBounds_input_SLHA_HS, void, (const char&), "higgsbounds_input_slha_", "HiggsBounds_input_SLHA_HS")
BE_FUNCTION(HiggsBounds_neutral_input_part_HS, void, (double*, double*, int*, double*, double*, double*, Farray<double, 1,3, 1,3>&,
						      double*, double*, double*, double*, double*, double*, double*,
						      double*, double*, double*, double*, double*, double*, double*,
						      double*, double*, double*, double*, double*, double*, double*,
						      double*, double*, double*, double*, double*, double*, double*,
						      double*, double*, Farray<double, 1,3, 1,3>&), "higgsbounds_neutral_input_part_", "HiggsBounds_neutral_input_part_HS")
BE_FUNCTION(HiggsBounds_charged_input_HS, void, (double*, double*, double*, double*,
						 double*, double*, double*, double*), "higgsbounds_charged_input_", "HiggsBounds_charged_input_HS")

BE_INI_FUNCTION{

  // Scan-level initialisation
  static bool scan_level = true;
  if(scan_level){
    int nHneut = 3; // number of neutral higgses
    int nHplus = 1; // number of charged higgses
    
    // initialize HiggsSignals with the latest results
    initialize_HiggsSignals_latestresults(nHneut,nHplus);

    // choose which pdf style to use for Higgs lineshape
    int pdf = 2; // Gaussian
    setup_pdf(pdf);
  }
  scan_level = false;

  // clean-up
  // finish_HiggsSignals();

}
END_BE_INI_FUNCTION

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
