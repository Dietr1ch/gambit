#ifndef __loaded_types_FlexibleSUSY_CMSSM_2_0_1_hpp__
#define __loaded_types_FlexibleSUSY_CMSSM_2_0_1_hpp__ 1

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "wrapper_CMSSM_input_parameters.h"
#include "wrapper_CMSSM_scales.h"
#include "wrapper_CMSSM_slha_io.h"
#include "wrapper_Spectrum_generator_problems.h"
#include "wrapper_Physical_input.h"
#include "wrapper_QedQcd.h"
#include "wrapper_CMSSM_spectrum_generator_Two_scale.h"
#include "wrapper_CMSSM_slha_Model_Two_scale.h"
#include "wrapper_Spectrum_generator_settings.h"
#include "wrapper_CMSSM_parameter_getter.h"
#include "wrapper_Command_line_options.h"
#include "identification.hpp"
#pragma GCC diagnostic pop

// Indicate which types are provided by this backend, and what the symbols of their factories are.
#define FlexibleSUSY_CMSSM_2_0_1_all_data \
  (( /*class*/(flexiblesusy)(CMSSM_input_parameters),    /*constructors*/(("Factory_CMSSM_input_parameters_0__BOSS_1",())) )) \
  (( /*class*/(flexiblesusy)(CMSSM_scales),    /*constructors*/(("Factory_CMSSM_scales_0__BOSS_2",())) )) \
  (( /*class*/(flexiblesusy)(CMSSM_slha_io),    /*constructors*/(("Factory_CMSSM_slha_io_0__BOSS_3",())) )) \
  (( /*class*/(flexiblesusy)(Spectrum_generator_problems),    /*constructors*/(("Factory_Spectrum_generator_problems_0__BOSS_4",())) )) \
  (( /*class*/(flexiblesusy)(Physical_input),    /*constructors*/(("Factory_Physical_input_0__BOSS_5",())) )) \
  (( /*class*/(softsusy)(QedQcd),    /*constructors*/(("Factory_QedQcd_0__BOSS_6",())) )) \
  (( /*class*/(flexiblesusy)(CMSSM_spectrum_generator_Two_scale),    /*constructors*/(("Factory_CMSSM_spectrum_generator_Two_scale_0__BOSS_7",())) )) \
  (( /*class*/(flexiblesusy)(CMSSM_slha_Model_Two_scale),    /*constructors*/(("Factory_CMSSM_slha_Model_Two_scale_0__BOSS_8",())) )) \
  (( /*class*/(flexiblesusy)(Spectrum_generator_settings),    /*constructors*/(("Factory_Spectrum_generator_settings_0__BOSS_9",())) )) \
  (( /*class*/(flexiblesusy)(CMSSM_parameter_getter),    /*constructors*/(("Factory_CMSSM_parameter_getter_0__BOSS_10",())) )) \
  (( /*class*/(flexiblesusy)(Command_line_options),    /*constructors*/(("Factory_Command_line_options_0__BOSS_11",())) (("Factory_Command_line_options_1__BOSS_12",(int, char**))) )) \

// If the default version has been loaded, set it as default.
#if ALREADY_LOADED(CAT_3(BACKENDNAME,_,CAT(Default_,BACKENDNAME)))
  SET_DEFAULT_VERSION_FOR_LOADING_TYPES(BACKENDNAME,SAFE_VERSION,CAT(Default_,BACKENDNAME))
#endif

// Undefine macros to avoid conflict with other backends.
#include "gambit/Backends/backend_undefs.hpp"

#endif /* __loaded_types_FlexibleSUSY_CMSSM_2_0_1_hpp__ */
