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
            namespace obscura
            {
                typedef ::obscura_1_0_2::obscura::DM_Detector_Ionization_Migdal DM_Detector_Ionization_Migdal;
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
BE_FUNCTION(XENON1T_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("XENON1T_S2_ER__BOSS_33","_XENON1T_S2_ER__BOSS_33"), "XENON1T_S2_ER")
BE_FUNCTION(DarkSide50_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("DarkSide50_S2_ER__BOSS_34","_DarkSide50_S2_ER__BOSS_34"), "DarkSide50_S2_ER")
BE_FUNCTION(DarkSide50_S2_ER_2023, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("DarkSide50_S2_ER_2023__BOSS_35","_DarkSide50_S2_ER_2023__BOSS_35"), "DarkSide50_S2_ER_2023")
BE_FUNCTION(PandaX_4T_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("PandaX_4T_S2_ER__BOSS_36","_PandaX_4T_S2_ER__BOSS_36"), "PandaX_4T_S2_ER")
BE_FUNCTION(LZ_S2_ER, obscura_1_0_2::obscura::DM_Detector_Ionization_ER, (), ("LZ_S2_ER__BOSS_37","_LZ_S2_ER__BOSS_37"), "LZ_S2_ER")
BE_FUNCTION(SENSEI_at_MINOS, obscura_1_0_2::obscura::DM_Detector_Crystal, (), ("SENSEI_at_MINOS__BOSS_38","_SENSEI_at_MINOS__BOSS_38"), "SENSEI_at_MINOS")
BE_FUNCTION(CDMS_HVeV_2020, obscura_1_0_2::obscura::DM_Detector_Crystal, (), ("CDMS_HVeV_2020__BOSS_39","_CDMS_HVeV_2020__BOSS_39"), "CDMS_HVeV_2020")
BE_FUNCTION(DAMIC_M_2023, obscura_1_0_2::obscura::DM_Detector_Crystal, (), ("DAMIC_M_2023__BOSS_40","_DAMIC_M_2023__BOSS_40"), "DAMIC_M_2023")
BE_FUNCTION(XENON1T_S2_Migdal, obscura_1_0_2::obscura::DM_Detector_Ionization_Migdal, (), ("XENON1T_S2_Migdal__BOSS_41","_XENON1T_S2_Migdal__BOSS_41"), "XENON1T_S2_Migdal")
BE_FUNCTION(DarkSide50_S2_Migdal, obscura_1_0_2::obscura::DM_Detector_Ionization_Migdal, (), ("DarkSide50_S2_Migdal__BOSS_42","_DarkSide50_S2_Migdal__BOSS_42"), "DarkSide50_S2_Migdal")
BE_FUNCTION(DarkSide50_S2_Migdal_2023, obscura_1_0_2::obscura::DM_Detector_Ionization_Migdal, (), ("DarkSide50_S2_Migdal_2023__BOSS_43","_DarkSide50_S2_Migdal_2023__BOSS_43"), "DarkSide50_S2_Migdal_2023")
BE_FUNCTION(PandaX_4T_S2_Migdal, obscura_1_0_2::obscura::DM_Detector_Ionization_Migdal, (), ("PandaX_4T_S2_Migdal__BOSS_44","_PandaX_4T_S2_Migdal__BOSS_44"), "PandaX_4T_S2_Migdal")
BE_FUNCTION(LZ_S2_Migdal, obscura_1_0_2::obscura::DM_Detector_Ionization_Migdal, (), ("LZ_S2_Migdal__BOSS_45","_LZ_S2_Migdal__BOSS_45"), "LZ_S2_Migdal")
// Variables

// Initialisation function (dependencies)

// Convenience functions (registration)

// Initialisation function (definition)
BE_INI_FUNCTION{} END_BE_INI_FUNCTION

// Convenience functions (definitions)

// End
#include "gambit/Backends/backend_undefs.hpp"
