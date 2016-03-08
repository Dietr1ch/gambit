//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Register the definitions of SubSpectrum 
///  contents here.
///
///  *********************************************
///
///  Authors: 
///  <!-- add name and date if you modify -->
///   
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2016 Feb 
///
///  *********************************************

#ifndef __registeredspectra_hpp__ 
#define __registeredspectra_hpp__ 

#include "gambit/Models/SpectrumContents/subspectrum_contents.hpp"

/// Just declare the classes here; should be defined in source files

namespace Gambit {
  namespace SpectrumContents {

    struct SM              : SubSpectrumContents { SM(); };
    struct SM_slha         : SubSpectrumContents { SM_slha(); }; // Missing some running masses that aren't part of SMINPUTS in slha
    struct SMHiggs         : SubSpectrumContents { SMHiggs(); };
    struct ScalarSingletDM : SubSpectrumContents { ScalarSingletDM(); };
    struct MSSM            : SubSpectrumContents { MSSM(); };

 }
}
#endif
