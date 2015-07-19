#ifndef __boss__Pythia_Pythia_8_209_h__
#define __boss__Pythia_Pythia_8_209_h__

// Pythia.h is a part of the PYTHIA event generator.
// Copyright (C) 2015 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This file contains the main class for event generation.
// Pythia: provide the main user interface to everything else.

#ifndef Pythia8_Pythia_H
#define Pythia8_Pythia_H

// Version number defined for use in macros and for consistency checks.
#define PYTHIA_VERSION 8.209

// Header files for the Pythia class and for what else the user may need.
#include "Pythia8/Analysis.h"
#include "Pythia8/Basics.h"
#include "Pythia8/BeamParticle.h"
#include "Pythia8/BeamShape.h"
#include "Pythia8/ColourReconnection.h"
#include "Pythia8/Event.h"
#include "Pythia8/FragmentationFlavZpT.h"
#include "Pythia8/HadronLevel.h"
#include "Pythia8/History.h"
#include "Pythia8/Info.h"
#include "Pythia8/JunctionSplitting.h"
#include "Pythia8/LesHouches.h"
#include "Pythia8/Merging.h"
#include "Pythia8/MergingHooks.h"
#include "Pythia8/PartonLevel.h"
#include "Pythia8/ParticleData.h"
#include "Pythia8/PartonDistributions.h"
#include "Pythia8/PartonSystems.h"
#include "Pythia8/ProcessLevel.h"
#include "Pythia8/PythiaStdlib.h"
#include "Pythia8/ResonanceWidths.h"
#include "Pythia8/RHadrons.h"
#include "Pythia8/Settings.h"
#include "Pythia8/SigmaTotal.h"
#include "Pythia8/SpaceShower.h"
#include "Pythia8/StandardModel.h"
#include "Pythia8/SLHAinterface.h"
#include "Pythia8/TimeShower.h"
#include "Pythia8/UserHooks.h"

namespace Pythia8 {

//==========================================================================

// The Pythia class contains the top-level routines to generate an event.

} 
#define ENUMS_DECLARED
#include "backend_types/Pythia_8_209/abstract_Pythia.h"
#include "gambit/Backends/abstracttypedefs.h"
#include "gambit/Backends/wrappertypedefs.h"
namespace Pythia8 { 
class Pythia : public virtual Abstract_Pythia {

public:

  // Constructor. (See Pythia.cc file.)
  Pythia(string xmlDir = "../share/Pythia8/xmldoc", bool printBanner = true);

  // Destructor. (See Pythia.cc file.)
  ~Pythia();

  // Read in one update for a setting or particle data from a single line.
  bool readString(string, bool warn = true);

  // Read in updates for settings or particle data from user-defined file.
  bool readFile(string fileName, bool warn = true,
    int subrun = SUBRUNDEFAULT);
  bool readFile(string fileName, int subrun) {
    return readFile(fileName, true, subrun);}
  bool readFile(istream& is = cin, bool warn = true,
    int subrun = SUBRUNDEFAULT);
  bool readFile(istream& is, int subrun) {
    return readFile(is, true, subrun);}

  // Possibility to pass in pointers to PDF's.
  bool setPDFPtr( PDF* pdfAPtrIn, PDF* pdfBPtrIn, PDF* pdfHardAPtrIn = 0,
    PDF* pdfHardBPtrIn = 0, PDF* pdfPomAPtrIn = 0, PDF* pdfPomBPtrIn = 0);

  // Possibility to pass in pointer to external LHA-interfaced generator.
  bool setLHAupPtr( LHAup* lhaUpPtrIn) {lhaUpPtr = lhaUpPtrIn; return true;}

  // Possibility to pass in pointer for external handling of some decays.
  bool setDecayPtr( DecayHandler* decayHandlePtrIn,
    vector<int> handledParticlesIn) {decayHandlePtr = decayHandlePtrIn;
    handledParticles.resize(0);
    for(int i = 0; i < int(handledParticlesIn.size()); ++i)
    handledParticles.push_back( handledParticlesIn[i] ); return true;}

  // Possibility to pass in pointer for external random number generation.
  bool setRndmEnginePtr( RndmEngine* rndmEnginePtrIn)
    { return rndm.rndmEnginePtr( rndmEnginePtrIn);}

  // Possibility to pass in pointer for user hooks.
  bool setUserHooksPtr( UserHooks* userHooksPtrIn)
    { userHooksPtr = userHooksPtrIn; return true;}

  // Possibility to pass in pointer for merging hooks.
  bool setMergingHooksPtr( MergingHooks* mergingHooksPtrIn)
    { mergingHooksPtr = mergingHooksPtrIn; return true;}

  // Possibility to pass in pointer for beam shape.
  bool setBeamShapePtr( BeamShape* beamShapePtrIn)
    { beamShapePtr = beamShapePtrIn; return true;}

  // Possibility to pass in pointer(s) for external cross section,
  // with option to include external phase-space generator(s).
  bool setSigmaPtr( SigmaProcess* sigmaPtrIn, PhaseSpace* phaseSpacePtrIn = 0)
    { sigmaPtrs.push_back( sigmaPtrIn);
      phaseSpacePtrs.push_back(phaseSpacePtrIn); return true;}

  // Possibility to pass in pointer(s) for external resonance.
  bool setResonancePtr( ResonanceWidths* resonancePtrIn)
    { resonancePtrs.push_back( resonancePtrIn); return true;}

  // Possibility to pass in pointer for external showers.
  bool setShowerPtr( TimeShower* timesDecPtrIn,
    TimeShower* timesPtrIn = 0, SpaceShower* spacePtrIn = 0)
    { timesDecPtr = timesDecPtrIn; timesPtr = timesPtrIn;
    spacePtr = spacePtrIn; return true;}

  // Initialize.
  bool init();

  // Generate the next event.
  bool next();

  // Generate only a single timelike shower as in a decay.
  int forceTimeShower( int iBeg, int iEnd, double pTmax, int nBranchMax = 0)
    { info.setScalup( 0, pTmax);
    return timesDecPtr->shower( iBeg, iEnd, event, pTmax, nBranchMax); }

  // Generate only the hadronization/decay stage.
  bool forceHadronLevel( bool findJunctions = true);

  // Special routine to allow more decays if on/off switches changed.
  bool moreDecays() {return hadronLevel.moreDecays(event);}

  // Special routine to force R-hadron decay when not done before.
  bool forceRHadronDecays() {return doRHadronDecays();}

  // List the current Les Houches event.
  void LHAeventList(ostream& os = cout) {
    if (lhaUpPtr != 0) lhaUpPtr->listEvent(os);}

  // Skip a number of Les Houches events at input.
  bool LHAeventSkip(int nSkip) {
    if (lhaUpPtr != 0) return lhaUpPtr->skipEvent(nSkip); return false;}

  // Main routine to provide final statistics on generation.
  void stat();

  // Read in settings values: shorthand, not new functionality.
  bool   flag(string key) {return settings.flag(key);}
  int    mode(string key) {return settings.mode(key);}
  double parm(string key) {return settings.parm(key);}
  string word(string key) {return settings.word(key);}

  // Auxiliary to set parton densities among list of possibilities.
  PDF* getPDFPtr(int idIn, int sequence = 1, string beam = "");

  // The event record for the parton-level central process.
  Event          process;

  // The event record for the complete event history.
  Event          event;

  // Information on the generation: current subprocess and error statistics.
  Info           info;

  // Settings: databases of flags/modes/parms/words to control run.
  Settings       settings;

  // ParticleData: the particle data table/database.
  ParticleData   particleData;

  // Random number generator.
  Rndm           rndm;

  // Standard Model couplings, including alphaS and alphaEM.
  Couplings      couplings;
  Couplings*     couplingsPtr;

  // SLHA Interface
  SLHAinterface slhaInterface;

  // The partonic content of each subcollision system (auxiliary to event).
  PartonSystems  partonSystems;

  // Merging object as wrapper for matrix element merging routines.
  Merging        merging;

  // Pointer to MergingHooks object for user interaction with the merging.
  // MergingHooks also more generally steers the matrix element merging.
  MergingHooks*  mergingHooksPtr;

private:

  // Copy and = constructors are made private so they cannot be used.
  Pythia(const Pythia&);
  Pythia& operator=(const Pythia&);

  // Constants: could only be changed in the code itself.
  static const double VERSIONNUMBERHEAD, VERSIONNUMBERCODE;
  static const int    NTRY, SUBRUNDEFAULT;

  // Initialization data, extracted from database.
  string xmlPath;
  bool   doProcessLevel, doPartonLevel, doHadronLevel, doDiffraction,
         doHardDiff, doResDec, doFSRinRes, decayRHadrons, abortIfVeto,
         checkEvent, checkHistory;
  int    nErrList;
  double epTolErr, epTolWarn, mTolErr, mTolWarn;

  // Initialization data, extracted from init(...) call.
  bool   isConstructed, isInit, isUnresolvedA, isUnresolvedB, showSaV,
         showMaD, doReconnect, forceHadronLevelCR;
  int    idA, idB, frameType, boostType, nCount, nShowLHA, nShowInfo,
         nShowProc, nShowEvt, reconnectMode;
  double mA, mB, pxA, pxB, pyA, pyB, pzA, pzB, eA, eB,
         pzAcm, pzBcm, eCM, betaZ, gammaZ;
  Vec4   pAinit, pBinit, pAnow, pBnow;
  RotBstMatrix MfromCM, MtoCM;

  // information for error checkout.
  int    nErrEvent;
  vector<int> iErrId, iErrCol, iErrEpm, iErrNan, iErrNanVtx;

  // Pointers to the parton distributions of the two incoming beams.
  PDF* pdfAPtr;
  PDF* pdfBPtr;

  // Extra PDF pointers to be used in hard processes only.
  PDF* pdfHardAPtr;
  PDF* pdfHardBPtr;

  // Extra Pomeron PDF pointers to be used in diffractive processes only.
  PDF* pdfPomAPtr;
  PDF* pdfPomBPtr;

  // Keep track when "new" has been used and needs a "delete" for PDF's.
  bool useNewPdfA, useNewPdfB, useNewPdfHard, useNewPdfPomA, useNewPdfPomB;

  // The two incoming beams.
  BeamParticle beamA;
  BeamParticle beamB;

  // Alternative Pomeron beam-inside-beam.
  BeamParticle beamPomA;
  BeamParticle beamPomB;

  // LHAup object for generating external events.
  bool   doLHA, useNewLHA;
  LHAup* lhaUpPtr;

  // Pointer to external decay handler and list of particles it handles.
  DecayHandler* decayHandlePtr;
  vector<int>   handledParticles;

  // Pointer to UserHooks object for user interaction with program.
  UserHooks* userHooksPtr;
  bool       hasUserHooks, doVetoProcess, doVetoPartons, retryPartonLevel;

  // Pointer to BeamShape object for beam momentum and interaction vertex.
  BeamShape* beamShapePtr;
  bool       useNewBeamShape, doMomentumSpread, doVertexSpread;

  // Pointers to external processes derived from the Pythia base classes.
  vector<SigmaProcess*> sigmaPtrs;

  // Pointers to external phase-space generators derived from Pythia
  // base classes.
  vector<PhaseSpace*> phaseSpacePtrs;

  // Pointers to external calculation of resonance widths.
  vector<ResonanceWidths*> resonancePtrs;

  // Pointers to timelike and spacelike showers.
  TimeShower*  timesDecPtr;
  TimeShower*  timesPtr;
  SpaceShower* spacePtr;
  bool         useNewTimesDec, useNewTimes, useNewSpace;

  // The main generator class to define the core process of the event.
  ProcessLevel processLevel;

  // The main generator class to produce the parton level of the event.
  PartonLevel partonLevel;

  // The main generator class to perform trial showers of the event.
  PartonLevel trialPartonLevel;

  // Flags for defining the merging scheme.
  bool        hasMergingHooks, hasOwnMergingHooks, doMerging;

  // The Colour reconnection class.
  ColourReconnection colourReconnection;

  // The junction spltiting class.
  JunctionSplitting junctionSplitting;

  // The main generator class to produce the hadron level of the event.
  HadronLevel hadronLevel;

  // The total cross section class is used both on process and parton level.
  SigmaTotal sigmaTot;

  // The RHadrons class is used both at PartonLevel and HadronLevel.
  RHadrons   rHadrons;

  // Write the Pythia banner, with symbol and version information.
  void banner(ostream& os = cout);

  // Check for lines in file that mark the beginning of new subrun.
  int readSubrun(string line, bool warn = true, ostream& os = cout);

  // Check for lines that mark the beginning or end of commented section.
  int readCommented(string line);

  // Check that combinations of settings are allowed; change if not.
  void checkSettings();

  // Check that beams and beam combination can be handled.
  bool checkBeams();

  // Calculate kinematics at initialization.
  bool initKinematics();

  // Set up pointers to PDFs.
  bool initPDFs();

  // Recalculate kinematics for each event when beam momentum has a spread.
  void nextKinematics();

  // Boost from CM frame to lab frame, or inverse. Set production vertex.
  void boostAndVertex(bool toLab, bool setVertex);

  // Perform R-hadron decays.
  bool doRHadronDecays();

  // Check that the final event makes sense.
  bool check(ostream& os = cout);

  // Initialization of SLHA data.
  bool initSLHA ();
  stringstream particleDataBuffer;

        public:
            Abstract_Pythia* pointerCopy__BOSS();

            void pointerAssign__BOSS(Abstract_Pythia* in);

        public:
            Pythia8::Abstract_Event& process_ref__BOSS();

            Pythia8::Abstract_Event& event_ref__BOSS();

            Pythia8::Abstract_Info& info_ref__BOSS();

            Pythia8::Abstract_Settings& settings_ref__BOSS();

            Pythia8::Abstract_ParticleData& particleData_ref__BOSS();

            Pythia8::Abstract_Rndm& rndm_ref__BOSS();

            Pythia8::Abstract_Couplings& couplings_ref__BOSS();

            Pythia8::Abstract_SLHAinterface& slhaInterface_ref__BOSS();


            Pythia8::Abstract_Pythia* operator_equal__BOSS(const Pythia8::Abstract_Pythia&);


        public:
            bool readString__BOSS(std::basic_string<char,std::char_traits<char>,std::allocator<char> >);

            bool readFile__BOSS(std::basic_string<char,std::char_traits<char>,std::allocator<char> >, bool);

            bool readFile__BOSS(std::basic_string<char,std::char_traits<char>,std::allocator<char> >);

            bool readFile__BOSS(std::basic_istream<char,std::char_traits<char> >&, bool);

            bool readFile__BOSS(std::basic_istream<char,std::char_traits<char> >&);

            bool readFile__BOSS();

            bool setUserHooksPtr__BOSS(Pythia8::Abstract_UserHooks*);

            bool setSigmaPtr__BOSS(Pythia8::Abstract_SigmaProcess*);

            bool setResonancePtr__BOSS(Pythia8::Abstract_ResonanceWidths*);

            int forceTimeShower__BOSS(int, int, double);

            bool forceHadronLevel__BOSS();

            void LHAeventList__BOSS();

        private:
            void banner__BOSS();

            int readSubrun__BOSS(std::basic_string<char,std::char_traits<char>,std::allocator<char> >, bool);

            int readSubrun__BOSS(std::basic_string<char,std::char_traits<char>,std::allocator<char> >);

            bool check__BOSS();

};

//==========================================================================

} // end namespace Pythia8

#endif // Pythia8_Pythia_H

#endif /* __boss__Pythia_Pythia_8_209_h__ */
