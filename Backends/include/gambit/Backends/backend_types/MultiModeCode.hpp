//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Helper types for MultiModeCode backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Selim C. Hotinli
///          selim.hotinli14@imperial.ac.uk
///  \date 2017 June
///  \date 2017 Oct
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2020 Feb
///
///  *************************

#include "gambit/Utils/util_types.hpp"

#ifndef __MultiModeCode_hpp__
#define __MultiModeCode_hpp__

namespace Gambit
{

    // type definition for the multimodecode output.
    typedef struct
    {
        bool check_ic_ok;
        double As;
        double A_iso;
        double A_pnad;
        double A_ent;
        double A_cross_ad_iso;
        double ns;
        double nt;
        double n_iso;
        double n_pnad;
        double n_ent;
        double N_pivot;
        double r;
        double alpha_s;
        double runofrun;
        double f_NL;
        double tau_NL;
        double k_array[1000];
        double pks_array[1000];
        double pks_array_iso[1000];
        double pkt_array[1000];
        int k_size;
        int err = 0;
    } gambit_inflation_observables;

}

#endif // defined __MultiModeCode_hpp__
