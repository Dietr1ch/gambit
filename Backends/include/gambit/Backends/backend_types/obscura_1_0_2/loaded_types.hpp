#ifndef __loaded_types_obscura_1_0_2_hpp__
#define __loaded_types_obscura_1_0_2_hpp__ 1

#include "wrapper_DM_Distribution.hpp"
#include "wrapper_Standard_Halo_Model.hpp"
#include "wrapper_DM_Particle_Standard.hpp"
#include "wrapper_DM_Particle_SI.hpp"
#include "wrapper_DM_Particle.hpp"
#include "wrapper_DM_Detector.hpp"
#include "wrapper_DM_Detector_Crystal.hpp"
#include "wrapper_DM_Detector_Ionization_ER.hpp"
#include "wrapper_DM_Detector_Ionization.hpp"
#include "wrapper_DM_Detector_Ionization_Migdal.hpp"
#include "identification.hpp"

// Indicate which types are provided by this backend, and what the symbols of their factories are.
#define obscura_1_0_2_all_data \
  (( /*class*/(obscura)(DM_Distribution),    /*constructors*/((("Factory_DM_Distribution_0__BOSS_5","_Factory_DM_Distribution_0__BOSS_5"),())) ((("Factory_DM_Distribution_1__BOSS_6","_Factory_DM_Distribution_1__BOSS_6"),(std::string, double, double, double))) )) \
  (( /*class*/(obscura)(Standard_Halo_Model),    /*constructors*/((("Factory_Standard_Halo_Model_0__BOSS_7","_Factory_Standard_Halo_Model_0__BOSS_7"),())) ((("Factory_Standard_Halo_Model_1__BOSS_8","_Factory_Standard_Halo_Model_1__BOSS_8"),(double, double, double, double))) ((("Factory_Standard_Halo_Model_2__BOSS_9","_Factory_Standard_Halo_Model_2__BOSS_9"),(double, double, double))) )) \
  (( /*class*/(obscura)(DM_Particle_Standard),    /*constructors*/((("Factory_DM_Particle_Standard_0__BOSS_10","_Factory_DM_Particle_Standard_0__BOSS_10"),())) ((("Factory_DM_Particle_Standard_1__BOSS_11","_Factory_DM_Particle_Standard_1__BOSS_11"),(double, double))) )) \
  (( /*class*/(obscura)(DM_Particle_SI),    /*constructors*/((("Factory_DM_Particle_SI_0__BOSS_12","_Factory_DM_Particle_SI_0__BOSS_12"),())) ((("Factory_DM_Particle_SI_1__BOSS_13","_Factory_DM_Particle_SI_1__BOSS_13"),(double))) ((("Factory_DM_Particle_SI_2__BOSS_14","_Factory_DM_Particle_SI_2__BOSS_14"),(double, double))) )) \
  (( /*class*/(obscura)(DM_Particle),    /*constructors*/((("Factory_DM_Particle_0__BOSS_15","_Factory_DM_Particle_0__BOSS_15"),())) ((("Factory_DM_Particle_1__BOSS_16","_Factory_DM_Particle_1__BOSS_16"),(double, double))) ((("Factory_DM_Particle_2__BOSS_17","_Factory_DM_Particle_2__BOSS_17"),(double))) )) \
  (( /*class*/(obscura)(DM_Detector),    /*constructors*/((("Factory_DM_Detector_0__BOSS_18","_Factory_DM_Detector_0__BOSS_18"),())) ((("Factory_DM_Detector_1__BOSS_19","_Factory_DM_Detector_1__BOSS_19"),(std::string, double, std::string))) )) \
  (( /*class*/(obscura)(DM_Detector_Crystal),    /*constructors*/((("Factory_DM_Detector_Crystal_0__BOSS_20","_Factory_DM_Detector_Crystal_0__BOSS_20"),())) ((("Factory_DM_Detector_Crystal_1__BOSS_21","_Factory_DM_Detector_Crystal_1__BOSS_21"),(std::string, double, std::string))) )) \
  (( /*class*/(obscura)(DM_Detector_Ionization_ER),    /*constructors*/((("Factory_DM_Detector_Ionization_ER_0__BOSS_22","_Factory_DM_Detector_Ionization_ER_0__BOSS_22"),())) ((("Factory_DM_Detector_Ionization_ER_1__BOSS_23","_Factory_DM_Detector_Ionization_ER_1__BOSS_23"),(std::string, double, std::string))) ((("Factory_DM_Detector_Ionization_ER_2__BOSS_24","_Factory_DM_Detector_Ionization_ER_2__BOSS_24"),(std::string, double, std::vector<std::string>, std::vector<double>))) ((("Factory_DM_Detector_Ionization_ER_3__BOSS_25","_Factory_DM_Detector_Ionization_ER_3__BOSS_25"),(std::string, double, std::vector<std::string>))) )) \
  (( /*class*/(obscura)(DM_Detector_Ionization),    /*constructors*/((("Factory_DM_Detector_Ionization_0__BOSS_26","_Factory_DM_Detector_Ionization_0__BOSS_26"),(std::string, double, std::string, std::string))) ((("Factory_DM_Detector_Ionization_1__BOSS_27","_Factory_DM_Detector_Ionization_1__BOSS_27"),(std::string, double, std::string, std::vector<std::string>, std::vector<double>))) ((("Factory_DM_Detector_Ionization_2__BOSS_28","_Factory_DM_Detector_Ionization_2__BOSS_28"),(std::string, double, std::string, std::vector<std::string>))) )) \
  (( /*class*/(obscura)(DM_Detector_Ionization_Migdal),    /*constructors*/((("Factory_DM_Detector_Ionization_Migdal_0__BOSS_29","_Factory_DM_Detector_Ionization_Migdal_0__BOSS_29"),())) ((("Factory_DM_Detector_Ionization_Migdal_1__BOSS_30","_Factory_DM_Detector_Ionization_Migdal_1__BOSS_30"),(std::string, double, std::string))) ((("Factory_DM_Detector_Ionization_Migdal_2__BOSS_31","_Factory_DM_Detector_Ionization_Migdal_2__BOSS_31"),(std::string, double, std::vector<std::string>, std::vector<double>))) ((("Factory_DM_Detector_Ionization_Migdal_3__BOSS_32","_Factory_DM_Detector_Ionization_Migdal_3__BOSS_32"),(std::string, double, std::vector<std::string>))) )) \

// If the default version has been loaded, set it as default.
#if ALREADY_LOADED(CAT_3(BACKENDNAME,_,CAT(Default_,BACKENDNAME)))
  SET_DEFAULT_VERSION_FOR_LOADING_TYPES(BACKENDNAME,SAFE_VERSION,CAT(Default_,BACKENDNAME))
#endif

// Undefine macros to avoid conflict with other backends.
#include "gambit/Backends/backend_undefs.hpp"

#endif /* __loaded_types_obscura_1_0_2_hpp__ */
