//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  External Model-specific sources for ColliderBit.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2019 Jan
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 Oct
///
///  *********************************************

#include "gambit/ColliderBit/getPy8Collider.hpp"
#include "gambit/ColliderBit/generateEventPy8Collider.hpp"

namespace Gambit
{
  namespace ColliderBit
  {

    // Get Monte Carlo event generator
    GET_SPECIFIC_PYTHIA(getPythia_EM, Pythia_EM_default, EM_spectrum, _EM, NOT_SUSY)
    GET_PYTHIA_AS_BASE_COLLIDER(getPythia_EMAsBase)

    // Run event generator
    GET_PYTHIA_EVENT(generateEventPythia_EM, Pythia_EM_default::Pythia8::Event)

  }
}
