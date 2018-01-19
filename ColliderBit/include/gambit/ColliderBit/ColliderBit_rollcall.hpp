//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for ColliderBit module.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Aldo Saavedra
///
///  \author Christopher Rogan
///          (christophersrogan@gmail.com)
///  \date 2015 Apr
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jul
///  \date 2018 Jan
///
///  \author Andy Buckley
///          (andy.buckley@cern.ch)
///  \date 2017 Jun
///
///  *********************************************

#ifndef __ColliderBit_rollcall_hpp__
#define __ColliderBit_rollcall_hpp__

#include <string>

#include "gambit/cmake/cmake_variables.hpp"
#include "gambit/Elements/shared_types.hpp"
#include "gambit/ColliderBit/ColliderBit_types.hpp"
#include "gambit/ColliderBit/ColliderBit_macros.hpp"



#define MODULE ColliderBit
START_MODULE

  #include "ColliderBit_Higgs_rollcall.hpp"
  #include "ColliderBit_LEP_rollcall.hpp"

  /// Sets the options for establishing convergence of Monte Carlo simulations
  #define CAPABILITY MC_ConvergenceSettings
  START_CAPABILITY
    #define FUNCTION MC_ConvergenceSettings_from_YAML
    START_FUNCTION(convergence_settings)
    #undef FUNCTION
  #undef CAPABILITY

  /// Controls looping of Collider simulations
  #define CAPABILITY ColliderOperator
  START_CAPABILITY
    #define FUNCTION operateLHCLoop
    START_FUNCTION(void, CAN_MANAGE_LOOPS)
    DEPENDENCY(MC_ConvergenceSettings, convergence_settings)
    #undef FUNCTION
  #undef CAPABILITY


  /// Collider sim capabilities
  #define CAPABILITY HardScatteringSim
  START_CAPABILITY
    #define FUNCTION getPythia
    START_FUNCTION(Gambit::ColliderBit::SpecializablePythia)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    NEEDS_CLASSES_FROM(Pythia, default)
    DEPENDENCY(decay_rates, DecayTable)
    MODEL_CONDITIONAL_DEPENDENCY(MSSM_spectrum, Spectrum, MSSM63atQ, MSSM63atMGUT)
    #undef FUNCTION

    #define FUNCTION getPythiaFileReader
    START_FUNCTION(Gambit::ColliderBit::SpecializablePythia)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    NEEDS_CLASSES_FROM(Pythia, default)
    #undef FUNCTION
  #undef CAPABILITY


  /// Detector sim capabilities
  #ifndef EXCLUDE_DELPHES
    #define CAPABILITY DetectorSim
    START_CAPABILITY
      #define FUNCTION getDelphes
      START_FUNCTION(Gambit::ColliderBit::DelphesVanilla)
      NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
      NEEDS_CLASSES_FROM(Pythia, default)
      #undef FUNCTION
    #undef CAPABILITY
  #endif

  #define CAPABILITY SimpleSmearingSim
  START_CAPABILITY
    #define FUNCTION getBuckFastATLAS
    START_FUNCTION(Gambit::ColliderBit::BuckFastSmearATLAS)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    #undef FUNCTION

    #define FUNCTION getBuckFastCMS
    START_FUNCTION(Gambit::ColliderBit::BuckFastSmearCMS)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    #undef FUNCTION

    #define FUNCTION getBuckFastIdentity
    START_FUNCTION(Gambit::ColliderBit::BuckFastIdentity)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    #undef FUNCTION
  #undef CAPABILITY


  /// Capability that holds list of analyses to run
  /// Eventually needs to be configurable from yaml file
  #define CAPABILITY ATLASAnalysisContainer
  START_CAPABILITY
    #define FUNCTION getATLASAnalysisContainer
    START_FUNCTION(HEPUtilsAnalysisContainer)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY CMSAnalysisContainer
  START_CAPABILITY
    #define FUNCTION getCMSAnalysisContainer
    START_FUNCTION(HEPUtilsAnalysisContainer)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY IdentityAnalysisContainer
  START_CAPABILITY
    #define FUNCTION getIdentityAnalysisContainer
    START_FUNCTION(HEPUtilsAnalysisContainer)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    #undef FUNCTION
  #undef CAPABILITY

  #ifndef EXCLUDE_DELPHES
  #define CAPABILITY DetAnalysisContainer
  START_CAPABILITY
    #define FUNCTION getDetAnalysisContainer
    START_FUNCTION(HEPUtilsAnalysisContainer)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    #undef FUNCTION
  #undef CAPABILITY
  #endif // not defined EXCLUDE_DELPHES



  /// Event capabilities
  #define CAPABILITY HardScatteringEvent
  START_CAPABILITY
    #define FUNCTION generatePythia8Event
    START_FUNCTION(Pythia8::Event)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    NEEDS_CLASSES_FROM(Pythia, default)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    #undef FUNCTION
  #undef CAPABILITY

  /// Detector simulators that directly produce the standard event format
  #ifndef EXCLUDE_DELPHES
    #define CAPABILITY ReconstructedEvent
    START_CAPABILITY
      #define FUNCTION reconstructDelphesEvent
      START_FUNCTION(HEPUtils::Event)
      NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
      NEEDS_CLASSES_FROM(Pythia, default)
      DEPENDENCY(HardScatteringEvent, Pythia8::Event)
      DEPENDENCY(DetectorSim, Gambit::ColliderBit::DelphesVanilla)
      #undef FUNCTION
    #undef CAPABILITY
  #endif

  #define CAPABILITY ATLASSmearedEvent
  START_CAPABILITY
    #define FUNCTION smearEventATLAS
    START_FUNCTION(HEPUtils::Event)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(HardScatteringEvent, Pythia8::Event)
    DEPENDENCY(SimpleSmearingSim, Gambit::ColliderBit::BuckFastSmearATLAS)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY CMSSmearedEvent
  START_CAPABILITY
    #define FUNCTION smearEventCMS
    START_FUNCTION(HEPUtils::Event)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(HardScatteringEvent, Pythia8::Event)
    DEPENDENCY(SimpleSmearingSim, Gambit::ColliderBit::BuckFastSmearCMS)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY CopiedEvent
  START_CAPABILITY
    #define FUNCTION copyEvent
    START_FUNCTION(HEPUtils::Event)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(HardScatteringEvent, Pythia8::Event)
    DEPENDENCY(SimpleSmearingSim, Gambit::ColliderBit::BuckFastIdentity)
    #undef FUNCTION
  #undef CAPABILITY

  // A capability that calculates the log likelihood
  // Runs all analyses and fills vector of analysis results
  #ifndef EXCLUDE_DELPHES
  #define CAPABILITY DetAnalysisNumbers
  START_CAPABILITY
    #define FUNCTION runDetAnalyses
    START_FUNCTION(AnalysisNumbers)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(MC_ConvergenceSettings, convergence_settings)
    DEPENDENCY(ReconstructedEvent, HEPUtils::Event)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    DEPENDENCY(DetAnalysisContainer, HEPUtilsAnalysisContainer)
    #undef FUNCTION
  #undef CAPABILITY
  #endif // not defined EXCLUDE_DELPHES

  #define CAPABILITY ATLASAnalysisNumbers
  START_CAPABILITY
    #define FUNCTION runATLASAnalyses
    START_FUNCTION(AnalysisNumbers)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(MC_ConvergenceSettings, convergence_settings)
    DEPENDENCY(ATLASSmearedEvent, HEPUtils::Event)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    DEPENDENCY(ATLASAnalysisContainer, HEPUtilsAnalysisContainer)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY CMSAnalysisNumbers
  START_CAPABILITY
    #define FUNCTION runCMSAnalyses
    START_FUNCTION(AnalysisNumbers)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(MC_ConvergenceSettings, convergence_settings)
    DEPENDENCY(CMSSmearedEvent, HEPUtils::Event)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    DEPENDENCY(CMSAnalysisContainer, HEPUtilsAnalysisContainer)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY IdentityAnalysisNumbers
  START_CAPABILITY
    #define FUNCTION runIdentityAnalyses
    START_FUNCTION(AnalysisNumbers)
    NEEDS_MANAGER_WITH_CAPABILITY(ColliderOperator)
    DEPENDENCY(MC_ConvergenceSettings, convergence_settings)
    DEPENDENCY(CopiedEvent, HEPUtils::Event)
    DEPENDENCY(HardScatteringSim, Gambit::ColliderBit::SpecializablePythia)
    DEPENDENCY(IdentityAnalysisContainer, HEPUtilsAnalysisContainer)
    #undef FUNCTION
  #undef CAPABILITY

  // Collect all the analysis numbers in one place
  #define CAPABILITY AllAnalysisNumbers
  START_CAPABILITY
    #define FUNCTION CollectAnalyses
    START_FUNCTION(AnalysisNumbers)
    DEPENDENCY(ATLASAnalysisNumbers, AnalysisNumbers)
    DEPENDENCY(CMSAnalysisNumbers, AnalysisNumbers)
    DEPENDENCY(IdentityAnalysisNumbers, AnalysisNumbers)
    #ifndef EXCLUDE_DELPHES
      DEPENDENCY(DetAnalysisNumbers, AnalysisNumbers)
    #endif
    #undef FUNCTION
  #undef CAPABILITY

  // Extract the signal predictions and uncertainties for all analyses
  #define CAPABILITY LHC_signals
  START_CAPABILITY
    #define FUNCTION calc_LHC_signals
    START_FUNCTION(map_str_dbl)
    DEPENDENCY(AllAnalysisNumbers, AnalysisNumbers)
    #undef FUNCTION
  #undef CAPABILITY

  // Calculate the log likelihood for each analysis from the analysis numbers
  #define CAPABILITY LHC_LogLikes
  START_CAPABILITY
    #define FUNCTION calc_LHC_LogLike_per_analysis
    START_FUNCTION(map_str_dbl)
    DEPENDENCY(AllAnalysisNumbers, AnalysisNumbers)
    BACKEND_REQ_FROM_GROUP(lnlike_marg_poisson, lnlike_marg_poisson_lognormal_error, (), double, (const int&, const double&, const double&, const double&) )
    BACKEND_REQ_FROM_GROUP(lnlike_marg_poisson, lnlike_marg_poisson_gaussian_error, (), double, (const int&, const double&, const double&, const double&) )
    BACKEND_GROUP(lnlike_marg_poisson)
    #undef FUNCTION
  #undef CAPABILITY

  // Calculate the total log likelihood
  #define CAPABILITY LHC_Combined_LogLike
  START_CAPABILITY
    #define FUNCTION calc_LHC_LogLike
    START_FUNCTION(double)
    DEPENDENCY(LHC_LogLikes, map_str_dbl)
    #undef FUNCTION
  #undef CAPABILITY


  // Dummy observable that creates a dependency on TestModel1D, which is used to satisfy the normal
  // GAMBIT model requrements in a minimal way. This is useful in the case where we just want to test
  // ColliderBit on a single point with Pythia's SLHA interface, but not use the ColliderBit standalone
  // interface.
  #define CAPABILITY DummyColliderObservable
  START_CAPABILITY
    #define FUNCTION getDummyColliderObservable
      START_FUNCTION(double)
      ALLOW_MODELS(TestModel1D)
    #undef FUNCTION
  #undef CAPABILITY


#undef MODULE


#endif /* defined __ColliderBit_rollcall_hpp__ */
