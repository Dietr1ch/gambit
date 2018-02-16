//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Sequence of all types printable by the HDF5
///  printer.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2017 Mar
///
///  *********************************************

#ifndef __HDF5TYPES__
#define __HDF5TYPES__

#define HDF5_TYPES          \
  (int)                     \
  (uint)                    \
  (long)                    \
  (ulong)                   \
  (longlong)                \
  (ulonglong)               \
  (float)                   \
  (double)                  \
  (std::vector<double>)     \
  (bool)                    \
  (map_str_dbl)             \
  (ModelParameters)         \
  (triplet<double>)         \

#define HDF5_MODULE_BACKEND_TYPES \
  (DM_nucleon_couplings)    \
  (Flav_KstarMuMu_obs)      \

#endif
