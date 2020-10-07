//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  List of types printable by GAMBIT printers.
///  Make sure to manually update this list
///  when you want to add a new printable type.
///
///  *********************************************
///
///  Authors:
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2016 Jan
///
///  *********************************************

#ifndef __printable_types_hpp__
#define __printable_types_hpp__

#include "gambit/Elements/types_rollcall.hpp"
#include "gambit/ScannerBit/printable_types.hpp"

// Types that Gambit printers can output (if printer plugin is properly equipped)
<<<<<<< HEAD
#define PRINTABLE_TYPES             \
  SCANNER_PRINTABLE_TYPES           \
  (map_const_str_dbl)               \
  (map_str_map_str_dbl)             \
  (map_const_str_map_const_str_dbl) \
  (triplet<double>)                 \
  (DM_nucleon_couplings)            \
  (Flav_KstarMuMu_obs)              \
  (FlavBit::flav_prediction)        \
  (map_intpair_dbl)
=======
#define PRINTABLE_TYPES    \
  SCANNER_PRINTABLE_TYPES  \
  (map_intpair_dbl)        \
  (triplet<double>)        \
  (DM_nucleon_couplings)   \
  (DM_nucleon_couplings_fermionic_HP) \
  (Flav_KstarMuMu_obs)     \
  (BBN_container)          \
>>>>>>> master

// Types that can be retrieved from Gambit printer output (if printer plugin is properly equipped)
// Generally needs to be the same as the printable types, i.e. should be able to retrieve everything.
#define RETRIEVABLE_TYPES PRINTABLE_TYPES

#endif // defined __printable_types_hpp__
