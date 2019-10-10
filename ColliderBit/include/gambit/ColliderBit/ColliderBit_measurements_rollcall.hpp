//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for ColliderBit measurments.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 July
///
///  *********************************************

#pragma once

#define MODULE ColliderBit


  /// Only activate this capability if HepMC and YODA are activated
  #ifndef EXCLUDE_HEPMC
    #ifndef EXCLUDE_YODA

      // Get measurements from Rivet
      #define CAPABILITY Rivet_measurements
      START_CAPABILITY
        #define FUNCTION Rivet_measurements
        START_FUNCTION(vector_shared_ptr<YODA::AnalysisObject>)
        NEEDS_CLASSES_FROM(Rivet, default)
        //DEPENDENCY(RunMC, MCLoopInfo) // Change to one that stores HepMC
        //BACKEND_REQ(Rivet_get_SM_measurements, (libRivet), double, ()) 
        //BACKEND_OPTION((Rivet, 3.0.1), (libRivet))
        #undef FUNCTION
      #undef CAPABILITY
    #endif
  #endif

  // This capability only works if YODA is activated
  #ifndef EXCLUDE_YODA

    // Calculate the log-likelihood for LHC measurements from a YODA file
    #define CAPABILITY LHC_measurements_LogLike
    START_CAPABILITY

      // GAMBIT version
      #define FUNCTION LHC_measurements_LogLike
      START_FUNCTION(double)
      DEPENDENCY(Rivet_measurements, vector_shared_ptr<YODA::AnalysisObject>)
      #undef FUNCTION

      // Contur version
      #define FUNCTION Contur_LHC_measurements_LogLike
      START_FUNCTION(double)
      DEPENDENCY(Rivet_measurements, vector_shared_ptr<YODA::AnalysisObject>)
      BACKEND_REQ(Contur_LogLike, (libcontur), double, (vector_shared_ptr<YODA::AnalysisObject> &))
      BACKEND_OPTION( (Contur), (libcontur) )
      #undef FUNCTION

      // Contur version, from file
      #define FUNCTION Contur_LHC_measurements_LogLike_from_file
      START_FUNCTION(double)
      DEPENDENCY(Rivet_measurements, vector_shared_ptr<YODA::AnalysisObject>)
      BACKEND_REQ(Contur_LogLike, (libcontur), double, (str &))
      BACKEND_OPTION( (Contur), (libcontur) )
      #undef FUNCTION
   
    #undef CAPABILITY

  #endif
  
#undef MODULE
