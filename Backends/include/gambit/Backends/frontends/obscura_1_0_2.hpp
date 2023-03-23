//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header generated by BOSS for GAMBIT backend obscura 1.0.2.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author The GAMBIT Collaboration
///
///  *********************************************

#include "gambit/Backends/backend_types/obscura_1_0_2/identification.hpp"

LOAD_LIBRARY

namespace Gambit
{
    namespace Backends
    {
        namespace obscura_1_0_2
        {
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Distribution DM_Distribution;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::Standard_Halo_Model Standard_Halo_Model;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Particle_Standard DM_Particle_Standard;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Particle_SI DM_Particle_SI;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Particle DM_Particle;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Detector DM_Detector;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Detector_Crystal DM_Detector_Crystal;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Detector_Ionization_ER DM_Detector_Ionization_ER;
            }
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Detector_Ionization DM_Detector_Ionization;
            }
        }
    }
}

// Functions
BE_FUNCTION(Fractional_Days_since_J2000, double, (int, int, int, double, double, double), ("Fractional_Days_since_J2000__BOSS_1","_Fractional_Days_since_J2000__BOSS_1"), "Fractional_Days_since_J2000")
// Other versions of this function. Only use one at a time, or set unique function names.
// BE_FUNCTION(Fractional_Days_since_J2000, double, (int, int, int, double, double), ("Fractional_Days_since_J2000__BOSS_2","_Fractional_Days_since_J2000__BOSS_2"), "Fractional_Days_since_J2000")
// BE_FUNCTION(Fractional_Days_since_J2000, double, (int, int, int, double), ("Fractional_Days_since_J2000__BOSS_3","_Fractional_Days_since_J2000__BOSS_3"), "Fractional_Days_since_J2000")
// BE_FUNCTION(Fractional_Days_since_J2000, double, (int, int, int), ("Fractional_Days_since_J2000__BOSS_4","_Fractional_Days_since_J2000__BOSS_4"), "Fractional_Days_since_J2000")
BE_FUNCTION(XENON10_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("XENON10_S2_ER__BOSS_29","_XENON10_S2_ER__BOSS_29"), "XENON10_S2_ER")
BE_FUNCTION(XENON100_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("XENON100_S2_ER__BOSS_30","_XENON100_S2_ER__BOSS_30"), "XENON100_S2_ER")
BE_FUNCTION(XENON1T_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("XENON1T_S2_ER__BOSS_31","_XENON1T_S2_ER__BOSS_31"), "XENON1T_S2_ER")
BE_FUNCTION(DarkSide50_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("DarkSide50_S2_ER__BOSS_32","_DarkSide50_S2_ER__BOSS_32"), "DarkSide50_S2_ER")
BE_FUNCTION(SENSEI_at_MINOS, obscura_1_0_2::obscura::DM_Detector_Crystal, (), ("SENSEI_at_MINOS__BOSS_33","_SENSEI_at_MINOS__BOSS_33"), "SENSEI_at_MINOS")
BE_FUNCTION(CDMS_HVeV_2020, obscura_1_0_2::obscura::DM_Detector_Crystal, (), ("CDMS_HVeV_2020__BOSS_34","_CDMS_HVeV_2020__BOSS_34"), "CDMS_HVeV_2020")
// Variables

// Initialisation function (dependencies)

// Convenience functions (registration)

// Initialisation function (definition)
BE_INI_FUNCTION{} END_BE_INI_FUNCTION

// Convenience functions (definitions)

// End
#include "gambit/Backends/backend_undefs.hpp"
