//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of ColliderBit_eventLoop. Based
///  heavily on the ExampleBit_A Functions
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
///  \author Andy Buckley
///
///  \author Chris Rogan
///          (crogan@cern.ch)
///  \date 2014 Aug
///  \date 2015 May
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jul
///
///  *********************************************

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/Elements/mssm_slhahelp.hpp"
#include "gambit/ColliderBit/ColliderBit_rollcall.hpp"
#include "gambit/ColliderBit/lep_mssm_xsecs.hpp"

//#define DUMP_LIMIT_PLOT_DATA

namespace Gambit
{

  namespace ColliderBit
  {


    /// ********************************************
    /// Non-rollcalled Functions and Local Variables
    /// ********************************************

    /// LEP limit likelihood function
    double limitLike(double x, double x95, double sigma) {
      static double p95 = 1.;
      using std::erf;
      using std::sqrt;

      if (p95 < 1.01) {
        for (int i=0; i<20000; i++) {
          static double step = 0.1;
          if (0.5 * (1 - erf(p95 + step)) > 0.05) p95 += step;
          else step /= 10.;
        }
      }

      return 0.5 * (1 - erf(p95 + (x - x95) / sigma / sqrt(2.)));
    }

    /// Event labels
    enum specialEvents {BASE_INIT=-1, INIT = -2, START_SUBPROCESS = -3, END_SUBPROCESS = -4, FINALIZE = -5};
    /// Pythia stuff
    std::vector<std::string> pythiaNames;
    std::vector<std::string>::const_iterator iter;
    bool allProcessesVetoed;
    double xsecGen;
    int pythiaConfigurations, pythiaNumber, nEvents, counter;
    /// Analysis stuff
    std::vector<std::string> analysisNames;
    HEPUtilsAnalysisContainer* globalAnalyses = new HEPUtilsAnalysisContainer();
    /// General collider sim info stuff
    #define SHARED_OVER_OMP iter,allProcessesVetoed,xsecGen,counter,analysisNames,globalAnalyses


    /// *************************************************
    /// Rollcalled functions properly hooked up to Gambit
    /// *************************************************
    /// *** Loop Managers ***

    /// @note: Much of the loop below designed for splitting up the subprocesses to be generated.
    /// @note: For our first run, we will just run all SUSY subprocesses.

    void operatePythia()
    {
      using namespace Pipes::operatePythia;
      nEvents = 0;
      // Set allProcessesVetoed to false once some events are generated.
      allProcessesVetoed = true;

      // Do the base-level initialisation
      Loop::executeIteration(BASE_INIT);

      // Retrieve runOptions from the YAML file safely...
      GET_COLLIDER_RUNOPTION(pythiaNames, std::vector<std::string>);
      // @todo Subprocess specific nEvents
      GET_COLLIDER_RUNOPTION(nEvents, int);

      // For every collider requested in the yaml file:
      for (iter = pythiaNames.cbegin(); iter != pythiaNames.cend(); ++iter)
      {
        piped_invalid_point.check();
        pythiaNumber = 0;
        // Defaults to 1 if option unspecified
        pythiaConfigurations = runOptions->getValueOrDef<int>(1, *iter);

        while (pythiaNumber < pythiaConfigurations)
        {
          piped_invalid_point.check();
          xsecGen = 0.;
          counter = 0;
          ++pythiaNumber;
          Loop::reset();
          Loop::executeIteration(INIT);
          #pragma omp parallel shared(SHARED_OVER_OMP)
          {
            Loop::executeIteration(START_SUBPROCESS);
            // post init / xsec veto synchronization
            #pragma omp barrier
            // main event loop
      while(not *Loop::done and counter < nEvents) {
              allProcessesVetoed = false;
              // race conditions may push counter past nEvents. But that is OK.
              #pragma omp atomic
              counter++;
        Loop::executeIteration(counter);
            }
            Loop::executeIteration(END_SUBPROCESS);
          }
          std::cout << "\n\n\n\n Operation of Pythia named " << *iter
                    << " number " << std::to_string(pythiaNumber) << " has finished." << std::endl;
          #ifdef HESITATE
          std::cout<<"\n\n [Press Enter]";
          std::getchar();
          #endif
        }
      }
      Loop::executeIteration(FINALIZE);
    }



    /// *** Hard Scattering Collider Simulators ***

    void getPythia(Gambit::ColliderBit::SpecializablePythia &result)
    {
      using namespace Pipes::getPythia;

      static bool SLHA_debug_mode = false;
      static std::vector<std::string> filenames;
      static std::string pythia_doc_path;
      static bool print_pythia_banner = true;
      static unsigned int counter = -1;
      // variables for xsec veto
      std::stringstream processLevelOutput;
      std::string _junk, line;
      std::istringstream* issPtr;
      int code;
      double xsec, totalxsec;

      if (*Loop::iteration == BASE_INIT)
      {
        // Get Pythia to print its banner.
        if (print_pythia_banner)
        {
          #pragma omp_critical (runOptions)
          {
            pythia_doc_path = runOptions->getValue<std::string>("Pythia_doc_path");
          }
          result.banner(pythia_doc_path);
          result.clear();
          print_pythia_banner = false;
        }
        // If there are no debug filenames set, look for them.
        if (filenames.empty())
        {
          #pragma omp_critical (runOptions)
          {
            SLHA_debug_mode = runOptions->hasKey("debug_SLHA_filenames");
            if (SLHA_debug_mode) filenames = runOptions->getValue<std::vector<str> >("debug_SLHA_filenames");
          }
        }
        // Increment the counter if there are debug SLHA files and this is the first thread.
        if (SLHA_debug_mode)
        {
          if (omp_get_thread_num() == 0) counter++;
          if (filenames.size() <= counter) invalid_point().raise("No more SLHA files. My work is done.");
        }
      }

      else if (*Loop::iteration == START_SUBPROCESS)
      {
        result.clear();
        // Each thread gets its own Pythia instance.
        // Thus, the actual Pythia initialization is
        // *after* INIT, within omp parallel.
        std::vector<std::string> pythiaOptions;
        std::string pythiaConfigName;

        // Setup new Pythia
        pythiaConfigName = "pythiaOptions_" + std::to_string(pythiaNumber);

        // If the SpecializablePythia specialization is hard-coded, okay with no options.
        #pragma omp critical (runOptions)
        {
          if (runOptions->hasKey(*iter, pythiaConfigName))
            pythiaOptions = runOptions->getValue<std::vector<std::string>>(*iter, pythiaConfigName);
        }
        pythiaOptions.push_back("Print:quiet = on");
        pythiaOptions.push_back("SLHA:verbose = 0");
        if (omp_get_thread_num() == 0)
          pythiaOptions.push_back("Init:showProcesses = on");
        pythiaOptions.push_back("Random:seed = " + std::to_string(54321 + omp_get_thread_num()));

        result.resetSpecialization(*iter);

        if (SLHA_debug_mode)
        {
          // Run Pythia reading an SLHA file.
          if (omp_get_thread_num() == 0)
            logger() << "Reading SLHA file: " << filenames.at(counter) << EOM;
          pythiaOptions.push_back("SLHA:file = " + filenames.at(counter));
          try {
            if (omp_get_thread_num() == 0)
              result.init(pythia_doc_path, pythiaOptions, processLevelOutput);
            else
              result.init(pythia_doc_path, pythiaOptions);
          } catch (SpecializablePythia::InitializationError &e) {
            piped_invalid_point.request("Bad point: Pythia can't initialize");
            Loop::wrapup();
            return;
          }
        }
        else
        {
          // Run Pythia using an SLHAea object constructed from dependencies on the spectrum and decays.
          SLHAstruct slha = Dep::decay_rates->as_slhaea();
          if (ModelInUse("MSSM78atQ") or ModelInUse("MSSM78atMGUT"))
          {
            // MSSM-specific
            SLHAstruct spectrum;
            #pragma omp critical (spectrum_as_slhaea)
            {
              spectrum = (*Dep::MSSM_spectrum)->getSLHAea();
            }
            SLHAea::Block block("MODSEL");
            block.push_back("BLOCK MODSEL              # Model selection");
            SLHAea::Line line;
            line << 1 << 0 << "# General MSSM";
            block.push_back(line);
            slha.insert(slha.begin(), spectrum.begin(), spectrum.end());
            slha.push_front(block);
          }
          else
          {
            ColliderBit_error().raise(LOCAL_INFO, "No spectrum object available for this model.");
          }
          pythiaOptions.push_back("SLHA:file = slhaea");

          try
          {
            if (omp_get_thread_num() == 0)
              result.init(pythia_doc_path, pythiaOptions, &slha, processLevelOutput);
            else
              result.init(pythia_doc_path, pythiaOptions, &slha);
          }
          catch (SpecializablePythia::InitializationError &e)
          {
            piped_invalid_point.request("Bad point: Pythia can't initialize");
            Loop::wrapup();
            return;
          }
        }

        // xsec veto
        if (omp_get_thread_num() == 0) {
          code = -1;
          totalxsec = 0.;
          while(true) {
            std::getline(processLevelOutput, line);
            issPtr = new std::istringstream(line);
            issPtr->seekg(47, issPtr->beg);
            (*issPtr) >> code;
            if (!issPtr->good() && totalxsec > 0.) break;
            (*issPtr) >> _junk >> xsec;
            if (issPtr->good()) totalxsec += xsec;
            delete issPtr;
          }

          /// @todo Remove the hard-coded 20.3 inverse femtobarns! This needs to be analysis-specific
          if (totalxsec * 1e12 * 20.3 < 1.) Loop::wrapup();
        }

      }
    }



    /// *** Detector Simulators ***

#ifndef EXCLUDE_DELPHES
    void getDelphes(Gambit::ColliderBit::DelphesVanilla &result) {
      using namespace Pipes::getDelphes;
      std::vector<std::string> delphesOptions;
      if (*Loop::iteration == INIT)
      {
        result.clear();
        #pragma omp critical (Delphes)
        {
          /// Setup new Delphes
          GET_COLLIDER_RUNOPTION(delphesOptions, std::vector<std::string>);
          result.init(delphesOptions);
        }
      }
    }
#endif // not defined EXCLUDE_DELPHES


    void getBuckFast(Gambit::ColliderBit::BuckFastSmear &result)
    {
      using namespace Pipes::getBuckFast;
      std::string buckFastOption;
      if (*Loop::iteration == INIT)
      {
        result.clear();
        #pragma omp critical (BuckFast)
        {
          /// Setup new BuckFast
          /// @note There's really nothing to do. BuckFast doesn't even have class variables.
          result.init();
        }
      }
    }


    void getBuckFastIdentity(Gambit::ColliderBit::BuckFastIdentity &result)
    {
      using namespace Pipes::getBuckFastIdentity;
      std::string buckFastOption;
      if (*Loop::iteration == INIT)
      {
        result.clear();
        #pragma omp critical (BuckFast)
        {
          /// Setup new BuckFast
          /// @note There's really nothing to do. BuckFast doesn't even have class variables.
          result.init();
        }
      }
    }



    /// *** Initialization for analyses ***

    void getAnalysisContainer(Gambit::ColliderBit::HEPUtilsAnalysisContainer& result) {
      using namespace Pipes::getAnalysisContainer;
      if (*Loop::iteration == BASE_INIT) {
        GET_COLLIDER_RUNOPTION(analysisNames, std::vector<std::string>);
        globalAnalyses->clear();
        globalAnalyses->init(analysisNames);
        return;
      }

      if (*Loop::iteration == START_SUBPROCESS)
      {
        /// Each thread gets its own Analysis container.
        /// Thus, their initialization is *after* INIT, within omp parallel.
        result.clear();
        result.init(analysisNames);
        return;
      }

      if (*Loop::iteration == END_SUBPROCESS)
      {
        const double xs = Dep::HardScatteringSim->xsec_pb();
        const double xserr = Dep::HardScatteringSim->xsecErr_pb();
        #pragma omp atomic
        xsecGen += xs;
        result.add_xsec(xs, xserr);
        #pragma omp critical (access_globalAnalyses)
        {
          globalAnalyses->add(result);
        }
        return;
      }
    }



    /// *** Hard Scattering Event Generators ***

    void generatePythia8Event(Pythia8::Event& result)
    {
      using namespace Pipes::generatePythia8Event;
      if (*Loop::iteration <= BASE_INIT) return;
      result.clear();

      /// Get the next event from Pythia8
      try {
        (*Dep::HardScatteringSim).nextEvent(result);
      } catch (SpecializablePythia::EventFailureError &e) {
        piped_invalid_point.request("Bad point: Pythia can't generate events");
        Loop::wrapup();
        return;
      }
    }



    /// Convert a hadron-level Pythia8::Event into an unsmeared HEPUtils::Event
    /// @todo Overlap between jets and prompt containers: need some isolation in MET calculation
    void convertPythia8ParticleEvent(HEPUtils::Event& result)
    {
      using namespace Pipes::convertPythia8ParticleEvent;
      if (*Loop::iteration <= BASE_INIT) return;
      result.clear();

      /// Get the next event from Pythia8
      const Pythia8::Event& pevt = *Dep::HardScatteringEvent;

      std::vector<fastjet::PseudoJet> bhadrons; //< for input to FastJet b-tagging
      std::vector<HEPUtils::Particle*> bpartons;
      std::vector<HEPUtils::Particle*> tauCandidates;
      HEPUtils::P4 pout; //< Sum of momenta outside acceptance

      // Make a first pass of non-final particles to gather b-hadrons and taus
      for (int i = 0; i < pevt.size(); ++i) {
        const Pythia8::Particle& p = pevt[i];

        // Find last b-hadrons in b decay chains as the best proxy for b-tagging
        if(p.idAbs()==5) {
          std::vector<int> bDaughterList = p.daughterList();
          bool isGoodB=true;

          for (size_t daughter = 0; daughter < bDaughterList.size(); daughter++) {
            const Pythia8::Particle& pDaughter = pevt[bDaughterList[daughter]];
            int daughterID = pDaughter.idAbs();
            if(daughterID == 5)isGoodB=false;
          }

          if(isGoodB){
            HEPUtils::Particle* tmpB = new HEPUtils::Particle(mk_p4(p.p()), p.id());
            bpartons.push_back(tmpB);
          }

        }

        // Veto leptonic taus
        if(p.idAbs()==15) {
          std::vector<int> tauDaughterList = p.daughterList();
          HEPUtils::P4 tmpMomentum;
          bool isGoodTau=true;

          for (size_t daughter = 0; daughter < tauDaughterList.size(); daughter++) {
            const Pythia8::Particle& pDaughter = pevt[tauDaughterList[daughter]];
            int daughterID = pDaughter.idAbs();
            if (daughterID == MCUtils::PID::ELECTRON || daughterID == MCUtils::PID::MUON
                || daughterID == MCUtils::PID::WPLUSBOSON || daughterID == MCUtils::PID::TAU)
              isGoodTau=false;
            if(!daughterID == MCUtils::PID::TAU)tmpMomentum+= mk_p4(pDaughter.p());
          }

          if(isGoodTau){
            HEPUtils::Particle* tmpTau = new HEPUtils::Particle(mk_p4(p.p()), p.id());
            tauCandidates.push_back(tmpTau);
          }
        }
      }

      // Loop over final state particles for jet inputs and MET
      std::vector<fastjet::PseudoJet> jetparticles;
      for (int i = 0; i < pevt.size(); ++i) {
        const Pythia8::Particle& p = pevt[i];

        // Only consider final state particles
        if (!p.isFinal()) continue;

        // Add particle outside ATLAS/CMS acceptance to MET
        /// @todo Move out-of-acceptance MET contribution to BuckFast
        if (abs(p.eta()) > 5.0) {
          pout += mk_p4(p.p());
          continue;
        }

        // Promptness: for leptons and photons we're only interested if they don't come from hadron/tau decays
        const bool prompt = !fromHadron(i, pevt); //&& !fromTau(i, pevt);
        const bool visible = MCUtils::PID::isStrongInteracting(p.id()) || MCUtils::PID::isEMInteracting(p.id());

        // Add prompt and invisible particles as individual particles
        if (prompt || !visible) {
          HEPUtils::Particle* gp = new HEPUtils::Particle(mk_p4(p.p()), p.id());
          gp->set_prompt();
          result.add_particle(gp); // Will be automatically categorised
        }

        // All particles other than invisibles and muons are jet constituents
        if (visible && p.idAbs() != MCUtils::PID::MUON) jetparticles.push_back(mk_pseudojet(p.p()));
      }

      /// Jet finding
      /// Currently hard-coded to use anti-kT R=0.4 jets above 10 GeV (could remove pT cut entirely)
      /// @todo choose jet algorithm via _settings?
      const fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 0.4);
      fastjet::ClusterSequence cseq(jetparticles, jet_def);
      std::vector<fastjet::PseudoJet> pjets = sorted_by_pt(cseq.inclusive_jets(10));

      /// Do jet b-tagging, etc. and add to the Event
      /// @todo Use ghost tagging?
      /// @note We need to _remove_ this b-tag in the detector sim if outside the tracker acceptance!
      for (auto& pj : pjets) {
        /// @todo Replace with HEPUtils::any(bhadrons, [&](const auto& pb){ pj.delta_R(pb) < 0.4 })
        bool isB = false;

        HEPUtils::P4 jetMom=HEPUtils::mk_p4(pj);
        for (auto& pb : bpartons) {
          if (jetMom.deltaR_eta(pb->mom()) < 0.4) {
            isB = true;
            break;
          }
        }

        bool isTau=false;
        for(auto& ptau : tauCandidates){
          if(jetMom.deltaR_eta(ptau->mom()) < 0.5){
            isTau=true;
            break;
          }
        }

        // Add to the event (use jet momentum for tau)
        if(isTau){
          HEPUtils::Particle* gp = new HEPUtils::Particle(HEPUtils::mk_p4(pj), MCUtils::PID::TAU);
          gp->set_prompt();
          result.add_particle(gp);
        }

        result.add_jet(new HEPUtils::Jet(HEPUtils::mk_p4(pj), isB));
      }

      /// Calculate missing momentum
      //
      // From balance of all visible momenta (requires isolation)
      // const std::vector<Particle*> visibles = result.visible_particles();
      // HEPUtils::P4 pvis;
      // for (size_t i = 0; i < visibles.size(); ++i) {
      //   pvis += visibles[i]->mom();
      // }
      // for (size_t i = 0; i < result.jets.size(); ++i) {
      //   pvis += result.jets[i]->mom();
      // }
      // set_missingmom(-pvis);
      //
      // From sum of invisibles, including those out of range
      for (size_t i = 0; i < result.invisible_particles().size(); ++i) {
        pout += result.invisible_particles()[i]->mom();
      }
      result.set_missingmom(pout);
    }

    /// Convert a partonic (no hadrons) Pythia8::Event into an unsmeared HEPUtils::Event
    void convertPythia8PartonEvent(HEPUtils::Event& result) {
      using namespace Pipes::convertPythia8PartonEvent;
      if (*Loop::iteration <= BASE_INIT) return;
      result.clear();

      /// Get the next event from Pythia8
      std::vector<HEPUtils::Particle*> tauCandidates;
      const auto& pevt = *Dep::HardScatteringEvent;

      // Make a first pass of non-final particles to gather taus
      for (int i = 0; i < pevt.size(); ++i) {
        const Pythia8::Particle& p = pevt[i];

        // Find last tau in prompt tau replica chains as a proxy for tau-tagging
        if(p.idAbs()==15) {
          std::vector<int> tauDaughterList = p.daughterList();
          HEPUtils::P4 tmpMomentum;
          bool isGoodTau=true;

          for (size_t daughter = 0; daughter < tauDaughterList.size(); daughter++) {
            const Pythia8::Particle& pDaughter = pevt[tauDaughterList[daughter]];
            int daughterID = pDaughter.idAbs();
            if (daughterID == MCUtils::PID::ELECTRON || daughterID == MCUtils::PID::MUON
                || daughterID == MCUtils::PID::WPLUSBOSON || daughterID == MCUtils::PID::TAU)
              isGoodTau=false;
            if(!daughterID == MCUtils::PID::TAU)tmpMomentum+= mk_p4(pDaughter.p());
          }

          if(isGoodTau){
            HEPUtils::Particle* tmpTau = new HEPUtils::Particle(mk_p4(p.p()), p.id());
            tauCandidates.push_back(tmpTau);
          }
        }
      }

      std::vector<fastjet::PseudoJet> jetparticles; //< Pseudojets for input to FastJet
      HEPUtils::P4 pout; //< Sum of momenta outside acceptance

      // Make a single pass over the event to gather final leptons, partons, and photons
      for (int i = 0; i < pevt.size(); ++i) {
        const Pythia8::Particle& p = pevt[i];

        // We only use "final" particles, i.e. those with no children. So Py8 must have hadronization disabled
        if (!p.isFinal()) continue;

        // Only consider partons within ATLAS/CMS acceptance
        /// @todo We should leave this for the detector sim / analysis to deal with
        if (abs(p.eta()) > 5.0) {
          pout += mk_p4(p.p());
          continue;
        }

        // Find electrons/muons/taus/photons to be treated as prompt (+ invisibles)
        /// @todo *Some* photons should be included in jets!!! Ignore for now since no FSR
        /// @todo Lepton dressing
        const bool prompt = isFinalPhoton(i, pevt) || (isFinalLepton(i, pevt)); // && abs(p.id()) != MCUtils::PID::TAU);
        const bool visible = MCUtils::PID::isStrongInteracting(p.id()) || MCUtils::PID::isEMInteracting(p.id());
        if (prompt || !visible) {
          HEPUtils::Particle* gp = new HEPUtils::Particle(mk_p4(p.p()), p.id());
          gp->set_prompt();
          result.add_particle(gp); // Will be automatically categorised
        }

        // Everything other than invisibles and muons, including taus & partons are jet constituents
        /// @todo Only include hadronic tau fraction?
        // if (visible && (isFinalParton(i, pevt) || isFinalTau(i, pevt))) {
        if (visible && p.idAbs() != MCUtils::PID::MUON) {
          fastjet::PseudoJet pj = mk_pseudojet(p.p());
          pj.set_user_index(abs(p.id()));
          jetparticles.push_back(pj);
        }

      }

      /// Jet finding
      /// Currently hard-coded to use anti-kT R=0.4 jets above 10 GeV (could remove pT cut entirely)
      /// @todo choose jet algorithm via _settings?
      const fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 0.4);
      fastjet::ClusterSequence cseq(jetparticles, jet_def);
      std::vector<fastjet::PseudoJet> pjets = sorted_by_pt(cseq.inclusive_jets(10));
      // Add to the event, with b-tagging info"
      for (const fastjet::PseudoJet& pj : pjets) {
        // Do jet b-tagging, etc. by looking for b quark constituents (i.e. user index = |parton ID| = 5)
        /// @note This b-tag is removed in the detector sim if outside the tracker acceptance!
        const bool isB = HEPUtils::any(pj.constituents(),
                 [](const fastjet::PseudoJet& c){ return c.user_index() == MCUtils::PID::BQUARK; });
        result.add_jet(new HEPUtils::Jet(HEPUtils::mk_p4(pj), isB));

        bool isTau=false;
        for(auto& ptau : tauCandidates){
          HEPUtils::P4 jetMom = HEPUtils::mk_p4(pj);
          if(jetMom.deltaR_eta(ptau->mom()) < 0.5){
            isTau=true;
            break;
          }
        }
        // Add to the event (use jet momentum for tau)
        if(isTau){
          HEPUtils::Particle* gp = new HEPUtils::Particle(HEPUtils::mk_p4(pj), MCUtils::PID::TAU);
          gp->set_prompt();
          result.add_particle(gp);
        }
      }

      /// Calculate missing momentum
      //
      // From balance of all visible momenta (requires isolation)
      // const std::vector<Particle*> visibles = result.visible_particles();
      // HEPUtils::P4 pvis;
      // for (size_t i = 0; i < visibles.size(); ++i) {
      //   pvis += visibles[i]->mom();
      // }
      // for (size_t i = 0; i < result.jets.size(); ++i) {
      //   pvis += result.jets[i]->mom();
      // }
      // set_missingmom(-pvis);
      //
      // From sum of invisibles, including those out of range
      for (const HEPUtils::Particle* p : result.invisible_particles())
        pout += p->mom();
      result.set_missingmom(pout);
    }


    /// *** Standard Event Format Functions ***

#ifndef EXCLUDE_DELPHES
    void reconstructDelphesEvent(HEPUtils::Event& result) {
      using namespace Pipes::reconstructDelphesEvent;
      if (*Loop::iteration <= BASE_INIT) return;
      result.clear();

      #pragma omp critical (Delphes)
      {
        (*Dep::DetectorSim).processEvent(*Dep::HardScatteringEvent, result);
      }
    }
#endif // not defined EXCLUDE_DELPHES

    void reconstructBuckFastEvent(HEPUtils::Event& result) {
      using namespace Pipes::reconstructBuckFastEvent;
      if (*Loop::iteration <= BASE_INIT) return;
      result.clear();

      (*Dep::SimpleSmearingSim).processEvent(*Dep::ConvertedScatteringEvent, result);
    }


    void reconstructBuckFastIdentityEvent(HEPUtils::Event& result) {
      using namespace Pipes::reconstructBuckFastIdentityEvent;
      if (*Loop::iteration <= BASE_INIT) return;
      result.clear();

      (*Dep::SimpleSmearingSim).processEvent(*Dep::ConvertedScatteringEvent, result);
    }



    /// *** Analysis Accumulators ***

    void runAnalyses(ColliderLogLikes& result)
    {
      using namespace Pipes::runAnalyses;
      if (*Loop::iteration == FINALIZE) {
        // The final iteration: get log likelihoods for the analyses
        result.clear();
        // Use counter instead of nEvents in case race conditions in the loop
        // caused counter to be slightly larger than nEvents.
        /// @todo The 20.3 hard-coded integrated lumi needs to be removed... this is an analysis-specific number
        const double scale = xsecGen * 20.3*1000 / counter;
        globalAnalyses->scale(scale);
        for (auto anaPtr = globalAnalyses->analyses.begin();
             anaPtr != globalAnalyses->analyses.end(); ++anaPtr)
        {
          //cout << "Set xsec from ana = " << (*anaPtr)->xsec() << " pb" << endl;
          // Finalize is currently only used to report a cut flow.... rename?
          (*anaPtr)->finalize();
          result.push_back((*anaPtr)->get_results());
        }
        return;
      }

      if (*Loop::iteration <= BASE_INIT) return;

      // Loop over analyses and run them... Managed by HEPUtilsAnalysisContainer
      Dep::AnalysisContainer->analyze(*Dep::ReconstructedEvent);
    }



    /// Loop over all analyses (and SRs within one analysis) and fill a vector of observed likelihoods
    void calc_LHC_LogLike(double& result) {
      using namespace Pipes::calc_LHC_LogLike;
      // xsec veto
      if (allProcessesVetoed) {
        logger() << "This point was xsec vetoed." << EOM;
        result = 0.;
        return;
      }
      ColliderLogLikes analysisResults = (*Dep::AnalysisNumbers);

      // Loop over analyses and calculate the total observed dll
      double total_dll_obs = 0;
      for (size_t analysis = 0; analysis < analysisResults.size(); ++analysis) {
        // cout << "In analysis loop" << endl;

        // Loop over the signal regions inside the analysis, and work out the total (delta) log likelihood for this analysis
        /// @note In general each analysis could/should work out its own likelihood so they can handle SR combination if possible.
        /// @note For now we just take the result from the SR *expected* to be most constraining, i.e. with highest expected dll
        double bestexp_dll_exp = 0, bestexp_dll_obs = 0;
        for (size_t SR = 0; SR < analysisResults[analysis].size(); ++SR) {
          // cout << "In signal region loop" << endl;
          SignalRegionData srData = analysisResults[analysis][SR];

          // Actual observed number of events
          const int n_obs = (int) srData.n_observed;

          // A contribution to the predicted number of events that is known exactly
          // (e.g. from data-driven background estimate)
          const double n_predicted_exact = 0;

          // A contribution to the predicted number of events that is not known exactly
          const double n_predicted_uncertain_b = srData.n_background;
          const double n_predicted_uncertain_sb = srData.n_signal + srData.n_background;

          // A fractional uncertainty on n_predicted_uncertain e.g. 0.2 from 20% uncertainty on efficencty wrt signal events
          const double bkg_ratio = srData.background_sys/srData.n_background;
          const double uncertainty_b = bkg_ratio;
          const double uncertainty_sb = sqrt(srData.background_sys*srData.background_sys + srData.signal_sys*srData.signal_sys) / n_predicted_uncertain_sb;

          const int n_predicted_total_b_int = (int) round(n_predicted_exact + n_predicted_uncertain_b);

          double llb_exp, llsb_exp, llb_obs, llsb_obs;
          // cout << "OBS " << n_obs << " EXACT " << n_predicted_exact
          //      << " UNCERTAIN_B "   << n_predicted_uncertain_b  << " UNCERTAINTY_B "   << uncertainty_b
          //      << " UNCERTAIN_S+B " << n_predicted_uncertain_sb << " UNCERTAINTY_S+B " << uncertainty_sb << endl;
          // Use a log-normal distribution for the nuisance parameter (more correct)
          if (*BEgroup::lnlike_marg_poisson == "lnlike_marg_poisson_lognormal_error") {
            llb_exp = BEreq::lnlike_marg_poisson_lognormal_error(n_predicted_total_b_int, n_predicted_exact, n_predicted_uncertain_b, uncertainty_b);
            llsb_exp = BEreq::lnlike_marg_poisson_lognormal_error(n_predicted_total_b_int, n_predicted_exact, n_predicted_uncertain_sb, uncertainty_sb);
            llb_obs = BEreq::lnlike_marg_poisson_lognormal_error(n_obs, n_predicted_exact, n_predicted_uncertain_b, uncertainty_b);
            llsb_obs = BEreq::lnlike_marg_poisson_lognormal_error(n_obs, n_predicted_exact, n_predicted_uncertain_sb, uncertainty_sb);
          }
          // Use a Gaussian distribution for the nuisance parameter (marginally faster)
          else if (*BEgroup::lnlike_marg_poisson == "lnlike_marg_poisson_gaussian_error") {
            llb_exp = BEreq::lnlike_marg_poisson_gaussian_error(n_predicted_total_b_int, n_predicted_exact, n_predicted_uncertain_b, uncertainty_b);
            llsb_exp = BEreq::lnlike_marg_poisson_gaussian_error(n_predicted_total_b_int, n_predicted_exact, n_predicted_uncertain_sb, uncertainty_sb);
            llb_obs = BEreq::lnlike_marg_poisson_gaussian_error(n_obs, n_predicted_exact, n_predicted_uncertain_b, uncertainty_b);
            llsb_obs = BEreq::lnlike_marg_poisson_gaussian_error(n_obs, n_predicted_exact, n_predicted_uncertain_sb, uncertainty_sb);
          }
          //cout << "COLLIDER_RESULT " << analysis << " " << SR << " " << llb_exp << " " << llsb_exp << " " << llb_obs << " " << llsb_obs << endl;

          // Calculate the expected dll and set the bestexp values for exp and obs dll if this one is the best so far
          const double dll_exp = llb_exp - llsb_exp; //< note positive dll convention -> more exclusion here
          if (dll_exp > bestexp_dll_exp) {
            bestexp_dll_exp = dll_exp;
            bestexp_dll_obs = llb_obs - llsb_obs;
          }

        } // end SR loop

        // Update the total obs dll
        /// @note For now we assume that the analyses are fully orthogonal, i.e. no possiblity that the same event appears twice -> straight addition
        total_dll_obs += bestexp_dll_obs;

      } // end ana loop

      // Set the single DLL to be returned (with conversion to more negative dll = more exclusion convention)
      result = -total_dll_obs;
    }


    // *** Limits from e+e- colliders ***

    /// ee --> selectron pair production cross-sections at 208 GeV
    /// @{
    void LEP208_SLHA1_convention_xsec_selselbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_selselbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "gauge_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");
      get_sigma_ee_ll(result, 208.0, 1, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_selserbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_selserbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "gauge_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");
      get_sigma_ee_ll(result, 208.0, 1, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_serserbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_serserbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "gauge_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");
      get_sigma_ee_ll(result, 208.0, 1, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_serselbar(triplet<double>& result)
    {
      result = *Pipes::LEP208_SLHA1_convention_xsec_serselbar::Dep::LEP208_xsec_selserbar;
    }
    void LEP208_SLHA1_convention_xsec_se1se1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_se1se1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");
      get_sigma_ee_ll(result, 208.0, 1, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_se1se2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_se1se2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");
      get_sigma_ee_ll(result, 208.0, 1, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_se2se2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_se2se2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");
      get_sigma_ee_ll(result, 208.0, 1, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_se2se1bar(triplet<double>& result)
    {
      result = *Pipes::LEP208_SLHA1_convention_xsec_se2se1bar::Dep::LEP208_xsec_se1se2bar;
    }
    /// @}

    /// ee --> smuon pair production cross-sections at 208 GeV
    /// @{
    void LEP208_SLHA1_convention_xsec_smulsmulbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_smulsmulbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 2, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_smulsmurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_smulsmurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 2, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_smursmurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_smursmurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 2, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_smursmulbar(triplet<double>& result)
    {
      result = *Pipes::LEP208_SLHA1_convention_xsec_smursmulbar::Dep::LEP208_xsec_smulsmurbar;
    }
    void LEP208_SLHA1_convention_xsec_smu1smu1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_smu1smu1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 2, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_smu1smu2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_smu1smu2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 2, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_smu2smu2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_smu2smu2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 2, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_smu2smu1bar(triplet<double>& result)
    {
      result = *Pipes::LEP208_SLHA1_convention_xsec_smu2smu1bar::Dep::LEP208_xsec_smu1smu2bar;
    }
    /// @}

    /// ee --> stau pair production cross-sections at 208 GeV
    /// @{
    void LEP208_SLHA1_convention_xsec_staulstaulbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_staulstaulbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 3, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_staulstaurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_staulstaurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 3, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_staurstaurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_staurstaurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 3, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP208_SLHA1_convention_xsec_staurstaulbar(triplet<double>& result)
    {
      result = *Pipes::LEP208_SLHA1_convention_xsec_staurstaulbar::Dep::LEP208_xsec_staulstaurbar;
    }
    void LEP208_SLHA1_convention_xsec_stau1stau1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_stau1stau1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 3, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_stau1stau2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_stau1stau2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 3, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_stau2stau2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_stau2stau2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 208.0, 3, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP208_SLHA1_convention_xsec_stau2stau1bar(triplet<double>& result)
    {
      result = *Pipes::LEP208_SLHA1_convention_xsec_stau2stau1bar::Dep::LEP208_xsec_stau1stau2bar;
    }
    /// @}


    /// ee --> neutralino pair production cross-sections at 208 GeV
    /// @{
    void LEP208_SLHA1_convention_xsec_chi00_11(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_11;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_12(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_12;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_13(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_13;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 1, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_14(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_14;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 1, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_22(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_22;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_23(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_23;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 2, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_24(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_24;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 2, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_33(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_33;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 3, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_34(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_34;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 3, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chi00_44(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chi00_44;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 208.0, 4, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    /// @}


    /// ee --> chargino pair production cross-sections at 208 GeV
    /// @{
    void LEP208_SLHA1_convention_xsec_chipm_11(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chipm_11;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 208.0, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chipm_12(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chipm_12;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 208.0, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chipm_22(triplet<double>& result)
    {
      using namespace Pipes::LEP208_SLHA1_convention_xsec_chipm_22;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 208.0, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP208_SLHA1_convention_xsec_chipm_21(triplet<double>& result)
    {
      result = *Pipes::LEP208_SLHA1_convention_xsec_chipm_21::Dep::LEP208_xsec_chipm_12;
    }
    /// @}


    /// ee --> selectron pair production cross-sections at 205 GeV
    /// @{
    void LEP205_SLHA1_convention_xsec_selselbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_selselbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 1, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_selserbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_selserbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 1, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_serserbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_serserbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 1, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_serselbar(triplet<double>& result)
    {
      result = *Pipes::LEP205_SLHA1_convention_xsec_serselbar::Dep::LEP205_xsec_selserbar;
    }
    void LEP205_SLHA1_convention_xsec_se1se1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_se1se1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 1, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_se1se2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_se1se2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 1, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_se2se2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_se2se2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 1, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_se2se1bar(triplet<double>& result)
    {
      result = *Pipes::LEP205_SLHA1_convention_xsec_se2se1bar::Dep::LEP205_xsec_se1se2bar;
    }
    /// @}

    /// ee --> smuon pair production cross-sections at 205 GeV
    /// @{
    void LEP205_SLHA1_convention_xsec_smulsmulbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_smulsmulbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 2, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_smulsmurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_smulsmurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 2, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_smursmurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_smursmurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 2, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_smursmulbar(triplet<double>& result)
    {
      result = *Pipes::LEP205_SLHA1_convention_xsec_smursmulbar::Dep::LEP205_xsec_smulsmurbar;
    }
    void LEP205_SLHA1_convention_xsec_smu1smu1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_smu1smu1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 2, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_smu1smu2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_smu1smu2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 2, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_smu2smu2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_smu2smu2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 2, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_smu2smu1bar(triplet<double>& result)
    {
      result = *Pipes::LEP205_SLHA1_convention_xsec_smu2smu1bar::Dep::LEP205_xsec_smu1smu2bar;
    }
    /// @}

    /// ee --> stau pair production cross-sections at 205 GeV
    /// @{
    void LEP205_SLHA1_convention_xsec_staulstaulbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_staulstaulbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 3, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_staulstaurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_staulstaurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 3, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_staurstaurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_staurstaurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 3, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP205_SLHA1_convention_xsec_staurstaulbar(triplet<double>& result)
    {
      result = *Pipes::LEP205_SLHA1_convention_xsec_staurstaulbar::Dep::LEP205_xsec_staulstaurbar;
    }
    void LEP205_SLHA1_convention_xsec_stau1stau1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_stau1stau1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 3, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_stau1stau2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_stau1stau2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 3, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_stau2stau2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_stau2stau2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 205.0, 3, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP205_SLHA1_convention_xsec_stau2stau1bar(triplet<double>& result)
    {
      result = *Pipes::LEP205_SLHA1_convention_xsec_stau2stau1bar::Dep::LEP205_xsec_stau1stau2bar;
    }
    /// @}


    /// ee --> neutralino pair production cross-sections at 205 GeV
    /// @{
    void LEP205_SLHA1_convention_xsec_chi00_11(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_11;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_12(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_12;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_13(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_13;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 1, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_14(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_14;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 1, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_22(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_22;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_23(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_23;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 2, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_24(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_24;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 2, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_33(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_33;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 3, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_34(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_34;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 3, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chi00_44(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chi00_44;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 205.0, 4, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    /// @}


    /// ee --> chargino pair production cross-sections at 205 GeV
    /// @{
    void LEP205_SLHA1_convention_xsec_chipm_11(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chipm_11;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 205.0, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chipm_12(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chipm_12;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 205.0, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chipm_22(triplet<double>& result)
    {
      using namespace Pipes::LEP205_SLHA1_convention_xsec_chipm_22;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 205.0, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP205_SLHA1_convention_xsec_chipm_21(triplet<double>& result)
    {
      result = *Pipes::LEP205_SLHA1_convention_xsec_chipm_21::Dep::LEP205_xsec_chipm_12;
    }

    /// ee --> selectron pair production cross-sections at 188.6 GeV
    /// @{
    void LEP188_SLHA1_convention_xsec_selselbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_selselbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 1, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_selserbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_selserbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 1, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_serserbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_serserbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 1, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_serselbar(triplet<double>& result)
    {
      result = *Pipes::LEP188_SLHA1_convention_xsec_serselbar::Dep::LEP188_xsec_selserbar;
    }
    void LEP188_SLHA1_convention_xsec_se1se1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_se1se1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 1, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_se1se2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_se1se2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 1, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_se2se2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_se2se2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 1, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_se2se1bar(triplet<double>& result)
    {
      result = *Pipes::LEP188_SLHA1_convention_xsec_se2se1bar::Dep::LEP188_xsec_se1se2bar;
    }
    /// @}

    /// ee --> smuon pair production cross-sections at 188.6 GeV
    /// @{
    void LEP188_SLHA1_convention_xsec_smulsmulbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_smulsmulbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 2, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_smulsmurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_smulsmurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 2, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_smursmurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_smursmurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 2, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_smursmulbar(triplet<double>& result)
    {
      result = *Pipes::LEP188_SLHA1_convention_xsec_smursmulbar::Dep::LEP188_xsec_smulsmurbar;
    }
    void LEP188_SLHA1_convention_xsec_smu1smu1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_smu1smu1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 2, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_smu1smu2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_smu1smu2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 2, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_smu2smu2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_smu2smu2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 2, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_smu2smu1bar(triplet<double>& result)
    {
      result = *Pipes::LEP188_SLHA1_convention_xsec_smu2smu1bar::Dep::LEP188_xsec_smu1smu2bar;
    }
    /// @}

    /// ee --> stau pair production cross-sections at 188.6 GeV
    /// @{
    void LEP188_SLHA1_convention_xsec_staulstaulbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_staulstaulbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 3, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_staulstaurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_staulstaurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 3, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_staurstaurbar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_staurstaurbar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 3, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, true);
    }
    void LEP188_SLHA1_convention_xsec_staurstaulbar(triplet<double>& result)
    {
      result = *Pipes::LEP188_SLHA1_convention_xsec_staurstaulbar::Dep::LEP188_xsec_staulstaurbar;
    }
    void LEP188_SLHA1_convention_xsec_stau1stau1bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_stau1stau1bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 3, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_stau1stau2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_stau1stau2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 3, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_stau2stau2bar(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_stau2stau2bar;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

      get_sigma_ee_ll(result, 188.6, 3, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV, false);
    }
    void LEP188_SLHA1_convention_xsec_stau2stau1bar(triplet<double>& result)
    {
      result = *Pipes::LEP188_SLHA1_convention_xsec_stau2stau1bar::Dep::LEP188_xsec_stau1stau2bar;
    }
    /// @}


    /// ee --> neutralino pair production cross-sections at 188.6 GeV
    /// @{
    void LEP188_SLHA1_convention_xsec_chi00_11(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_11;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_12(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_12;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_13(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_13;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 1, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_14(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_14;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 1, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_22(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_22;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_23(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_23;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 2, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_24(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_24;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 2, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_33(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_33;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 3, 3, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_34(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_34;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 3, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chi00_44(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chi00_44;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chi00(result, 188.6, 4, 4, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    /// @}


    /// ee --> chargino pair production cross-sections at 188.6 GeV
    /// @{
    void LEP188_SLHA1_convention_xsec_chipm_11(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chipm_11;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 188.6, 1, 1, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chipm_12(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chipm_12;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 188.6, 1, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chipm_22(triplet<double>& result)
    {
      using namespace Pipes::LEP188_SLHA1_convention_xsec_chipm_22;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      get_sigma_ee_chipm(result, 188.6, 2, 2, tol, pt_error, *Dep::MSSM_spectrum, Dep::Z_decay_rates->width_in_GeV);
    }
    void LEP188_SLHA1_convention_xsec_chipm_21(triplet<double>& result)
    {
      result = *Pipes::LEP188_SLHA1_convention_xsec_chipm_21::Dep::LEP188_xsec_chipm_12;
    }
    /// @}


    /// LEP Slepton Log-Likelihoods
    /// @{
    void ALEPH_Selectron_Conservative_LLike(double& result)
    {
      static ALEPHSelectronLimitAt208GeV *limitContainer = new ALEPHSelectronLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 115., 0., 100.,
                                     "lepLimitPlanev2/ALEPHSelectronLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::ALEPH_Selectron_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_seL = spec->get(Par::Pole_Mass,1000011, 0); 
      const double mass_seR = spec->get(Par::Pole_Mass,2000011, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these two processes individually:

      // se_L, se_L
      xsecLimit = limitContainer->limitAverage(mass_seL, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_selselbar;
      xsecWithError.upper *= pow(Dep::selectron_l_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.central *= pow(Dep::selectron_l_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.lower *= pow(Dep::selectron_l_decay_rates->BF("~chi0_1", "e-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // se_R, se_R
      xsecLimit = limitContainer->limitAverage(mass_seR, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_serserbar;
      xsecWithError.upper *= pow(Dep::selectron_r_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.central *= pow(Dep::selectron_r_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.lower *= pow(Dep::selectron_r_decay_rates->BF("~chi0_1", "e-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void ALEPH_Smuon_Conservative_LLike(double& result)
    {
      static ALEPHSmuonLimitAt208GeV *limitContainer = new ALEPHSmuonLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 115., 0., 100.,
                                     "lepLimitPlanev2/ALEPHSmuonLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::ALEPH_Smuon_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_smuL = spec->get(Par::Pole_Mass,1000013, 0); 
      const double mass_smuR = spec->get(Par::Pole_Mass,2000013, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these two processes individually:

      // smu_L, smu_L
      xsecLimit = limitContainer->limitAverage(mass_smuL, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_smulsmulbar;
      xsecWithError.upper *= pow(Dep::smuon_l_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.central *= pow(Dep::smuon_l_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.lower *= pow(Dep::smuon_l_decay_rates->BF("~chi0_1", "mu-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // smu_R, smu_R
      xsecLimit = limitContainer->limitAverage(mass_smuR, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_smursmurbar;
      xsecWithError.upper *= pow(Dep::smuon_r_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.central *= pow(Dep::smuon_r_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.lower *= pow(Dep::smuon_r_decay_rates->BF("~chi0_1", "mu-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void ALEPH_Stau_Conservative_LLike(double& result)
    {
      static ALEPHStauLimitAt208GeV *limitContainer = new ALEPHStauLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 115., 0., 100.,
                                     "lepLimitPlanev2/ALEPHStauLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::ALEPH_Stau_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_stau1 = spec->get(Par::Pole_Mass,1000015, 0); 
      const double mass_stau2 = spec->get(Par::Pole_Mass,2000015, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these two processes individually:

      // stau_1, stau_1
      xsecLimit = limitContainer->limitAverage(mass_stau1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_stau1stau1bar;
      xsecWithError.upper *= pow(Dep::stau_1_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.central *= pow(Dep::stau_1_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.lower *= pow(Dep::stau_1_decay_rates->BF("~chi0_1", "tau-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // stau_2, stau_2
      xsecLimit = limitContainer->limitAverage(mass_stau2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_stau2stau2bar;
      xsecWithError.upper *= pow(Dep::stau_2_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.central *= pow(Dep::stau_2_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.lower *= pow(Dep::stau_2_decay_rates->BF("~chi0_1", "tau-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void L3_Selectron_Conservative_LLike(double& result)
    {
      static L3SelectronLimitAt205GeV *limitContainer = new L3SelectronLimitAt205GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 115., 0., 100.,
                                     "lepLimitPlanev2/L3SelectronLimitAt205GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::L3_Selectron_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_seL = spec->get(Par::Pole_Mass,1000011, 0); 
      const double mass_seR = spec->get(Par::Pole_Mass,2000011, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these two processes individually:

      // se_L, se_L
      xsecLimit = limitContainer->limitAverage(mass_seL, mass_neut1, mZ);

      xsecWithError = *Dep::LEP205_xsec_selselbar;
      xsecWithError.upper *= pow(Dep::selectron_l_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.central *= pow(Dep::selectron_l_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.lower *= pow(Dep::selectron_l_decay_rates->BF("~chi0_1", "e-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // se_R, se_R
      xsecLimit = limitContainer->limitAverage(mass_seR, mass_neut1, mZ);

      xsecWithError = *Dep::LEP205_xsec_serserbar;
      xsecWithError.upper *= pow(Dep::selectron_r_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.central *= pow(Dep::selectron_r_decay_rates->BF("~chi0_1", "e-"), 2);
      xsecWithError.lower *= pow(Dep::selectron_r_decay_rates->BF("~chi0_1", "e-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void L3_Smuon_Conservative_LLike(double& result)
    {
      static L3SmuonLimitAt205GeV *limitContainer = new L3SmuonLimitAt205GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 115., 0., 100.,
                                     "lepLimitPlanev2/L3SmuonLimitAt205GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::L3_Smuon_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_smuL = spec->get(Par::Pole_Mass,1000013, 0); 
      const double mass_smuR = spec->get(Par::Pole_Mass,2000013, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these two processes individually:

      // smu_L, smu_L
      xsecLimit = limitContainer->limitAverage(mass_smuL, mass_neut1, mZ);

      xsecWithError = *Dep::LEP205_xsec_smulsmulbar;
      xsecWithError.upper *= pow(Dep::smuon_l_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.central *= pow(Dep::smuon_l_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.lower *= pow(Dep::smuon_l_decay_rates->BF("~chi0_1", "mu-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // smu_R, smu_R
      xsecLimit = limitContainer->limitAverage(mass_smuR, mass_neut1, mZ);

      xsecWithError = *Dep::LEP205_xsec_smursmurbar;
      xsecWithError.upper *= pow(Dep::smuon_r_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.central *= pow(Dep::smuon_r_decay_rates->BF("~chi0_1", "mu-"), 2);
      xsecWithError.lower *= pow(Dep::smuon_r_decay_rates->BF("~chi0_1", "mu-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void L3_Stau_Conservative_LLike(double& result)
    {
      static L3StauLimitAt205GeV *limitContainer = new L3StauLimitAt205GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 115., 0., 100.,
                                     "lepLimitPlanev2/L3StauLimitAt205GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::L3_Stau_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_stau1 = spec->get(Par::Pole_Mass,1000015, 0); 
      const double mass_stau2 = spec->get(Par::Pole_Mass,2000015, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these two processes individually:

      // stau_1, stau_1
      xsecLimit = limitContainer->limitAverage(mass_stau1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP205_xsec_stau1stau1bar;
      xsecWithError.upper *= pow(Dep::stau_1_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.central *= pow(Dep::stau_1_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.lower *= pow(Dep::stau_1_decay_rates->BF("~chi0_1", "tau-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // stau_2, stau_2
      xsecLimit = limitContainer->limitAverage(mass_stau2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP205_xsec_stau2stau2bar;
      xsecWithError.upper *= pow(Dep::stau_2_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.central *= pow(Dep::stau_2_decay_rates->BF("~chi0_1", "tau-"), 2);
      xsecWithError.lower *= pow(Dep::stau_2_decay_rates->BF("~chi0_1", "tau-"), 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }
    /// @}

    /// LEP Gaugino Log-Likelihoods
    /// @{
    void L3_Neutralino_All_Channels_Conservative_LLike(double& result)
    {
      static L3NeutralinoAllChannelsLimitAt188pt6GeV *limitContainer = new L3NeutralinoAllChannelsLimitAt188pt6GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(0., 200., 0., 100.,
                                     "lepLimitPlanev2/L3NeutralinoAllChannelsLimitAt188pt6GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::L3_Neutralino_All_Channels_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const DecayTable *decays = &(*Dep::decay_rates);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_neut2 = spec->get(Par::Pole_Mass,1000023, 0); 
      const double mass_neut3 = spec->get(Par::Pole_Mass,1000025, 0); 
      const double mass_neut4 = spec->get(Par::Pole_Mass,1000035, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // neut2, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chi00_12;
      // Total up all channels which look like Z* decays
      totalBR = 0;
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "Z0");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "ubar", "u");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "dbar", "d");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "cbar", "c");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "sbar", "s");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "bbar", "b");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "e+", "e-");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "mu+", "mu-");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "tau+", "tau-");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "nubar_e", "nu_e");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "nubar_mu", "nu_mu");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "nubar_tau", "nu_tau");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // neut3, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut3, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chi00_13;
      // Total up all channels which look like Z* decays
      totalBR = 0;
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "Z0");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "ubar", "u");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "dbar", "d");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "cbar", "c");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "sbar", "s");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "bbar", "b");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "e+", "e-");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "mu+", "mu-");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "tau+", "tau-");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "nubar_e", "nu_e");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "nubar_mu", "nu_mu");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "nubar_tau", "nu_tau");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // neut4, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut4, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chi00_14;
      // Total up all channels which look like Z* decays
      totalBR = 0;
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "Z0");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "ubar", "u");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "dbar", "d");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "cbar", "c");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "sbar", "s");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "bbar", "b");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "e+", "e-");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "mu+", "mu-");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "tau+", "tau-");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "nubar_e", "nu_e");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "nubar_mu", "nu_mu");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "nubar_tau", "nu_tau");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void L3_Neutralino_Leptonic_Conservative_LLike(double& result)
    {
      static L3NeutralinoLeptonicLimitAt188pt6GeV *limitContainer = new L3NeutralinoLeptonicLimitAt188pt6GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(0., 200., 0., 100.,
                                     "lepLimitPlanev2/L3NeutralinoLeptonicLimitAt188pt6GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::L3_Neutralino_Leptonic_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const DecayTable *decays = &(*Dep::decay_rates);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_neut2 = spec->get(Par::Pole_Mass,1000023, 0); 
      const double mass_neut3 = spec->get(Par::Pole_Mass,1000025, 0); 
      const double mass_neut4 = spec->get(Par::Pole_Mass,1000035, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // neut2, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chi00_12;
      // Total up all channels which look like leptonic Z* decays
      // Total up the leptonic Z decays first...
      totalBR = 0;
      totalBR += decays->at("Z0").BF("e+", "e-");
      totalBR += decays->at("Z0").BF("mu+", "mu-");
      totalBR += decays->at("Z0").BF("tau+", "tau-");
      totalBR = decays->at("~chi0_2").BF("~chi0_1", "Z0") * totalBR;

      totalBR += decays->at("~chi0_2").BF("~chi0_1", "e+", "e-");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "mu+", "mu-");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "tau+", "tau-");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // neut3, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut3, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chi00_13;
      // Total up all channels which look like leptonic Z* decays
      // Total up the leptonic Z decays first...
      totalBR = 0;
      totalBR += decays->at("Z0").BF("e+", "e-");
      totalBR += decays->at("Z0").BF("mu+", "mu-");
      totalBR += decays->at("Z0").BF("tau+", "tau-");
      totalBR = decays->at("~chi0_3").BF("~chi0_1", "Z0") * totalBR;

      totalBR += decays->at("~chi0_3").BF("~chi0_1", "e+", "e-");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "mu+", "mu-");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "tau+", "tau-");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // neut4, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut4, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chi00_14;
      // Total up all channels which look like leptonic Z* decays
      // Total up the leptonic Z decays first...
      totalBR = 0;
      totalBR += decays->at("Z0").BF("e+", "e-");
      totalBR += decays->at("Z0").BF("mu+", "mu-");
      totalBR += decays->at("Z0").BF("tau+", "tau-");
      totalBR = decays->at("~chi0_4").BF("~chi0_1", "Z0") * totalBR;

      totalBR += decays->at("~chi0_4").BF("~chi0_1", "e+", "e-");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "mu+", "mu-");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "tau+", "tau-");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void L3_Chargino_All_Channels_Conservative_LLike(double& result)
    {
      static L3CharginoAllChannelsLimitAt188pt6GeV *limitContainer = new L3CharginoAllChannelsLimitAt188pt6GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 100., 0., 100.,
                                     "lepLimitPlanev2/L3CharginoAllChannelsLimitAt188pt6GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::L3_Chargino_All_Channels_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const DecayTable *decays = &(*Dep::decay_rates);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_char1 = spec->get(Par::Pole_Mass,1000024, 0);
      const double mass_char2 = spec->get(Par::Pole_Mass,1000037, 0);
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // char1, neut1
      xsecLimit = limitContainer->limitAverage(mass_char1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chipm_11;
      // Total up all channels which look like W* decays
      totalBR = 0;
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "W+");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "c", "sbar");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "tau+", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // char2, neut1
      xsecLimit = limitContainer->limitAverage(mass_char2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chipm_22;
      // Total up all channels which look like W* decays
      totalBR = 0;
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "W+");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "c", "sbar");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "tau+", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void L3_Chargino_Leptonic_Conservative_LLike(double& result)
    {
      static L3CharginoLeptonicLimitAt188pt6GeV *limitContainer = new L3CharginoLeptonicLimitAt188pt6GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(45., 100., 0., 100.,
                                     "lepLimitPlanev2/L3CharginoLeptonicLimitAt188pt6GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::L3_Chargino_Leptonic_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const DecayTable *decays = &(*Dep::decay_rates);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_char1 = spec->get(Par::Pole_Mass,1000024, 0);
      const double mass_char2 = spec->get(Par::Pole_Mass,1000037, 0);
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // char1, neut1
      xsecLimit = limitContainer->limitAverage(mass_char1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chipm_11;
      // Total up all channels which look like leptonic W* decays
      // Total up the leptonic W decays first...
      totalBR = 0;
      totalBR += decays->at("W+").BF("e+", "nu_e");
      totalBR += decays->at("W+").BF("mu+", "nu_mu");
      totalBR += decays->at("W+").BF("tau+", "nu_tau");
      totalBR = decays->at("~chi+_1").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_1").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "tau+", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // char2, neut1
      xsecLimit = limitContainer->limitAverage(mass_char2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP188_xsec_chipm_22;
      // Total up all channels which look like leptonic W* decays
      // Total up the leptonic W decays first...
      totalBR = 0;
      totalBR += decays->at("W+").BF("e+", "nu_e");
      totalBR += decays->at("W+").BF("mu+", "nu_mu");
      totalBR += decays->at("W+").BF("tau+", "nu_tau");
      totalBR = decays->at("~chi+_2").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_2").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "tau+", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void OPAL_Chargino_Hadronic_Conservative_LLike(double& result)
    {
      static OPALCharginoHadronicLimitAt208GeV *limitContainer = new OPALCharginoHadronicLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(75., 105., 0., 105.,
                                     "lepLimitPlanev2/OPALCharginoHadronicLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::OPAL_Chargino_Hadronic_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const DecayTable *decays = &(*Dep::decay_rates);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_char1 = spec->get(Par::Pole_Mass,1000024, 0);
      const double mass_char2 = spec->get(Par::Pole_Mass,1000037, 0);
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // char1, neut1
      xsecLimit = limitContainer->limitAverage(mass_char1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_11;
      // Total up all channels which look like hadronic W* decays
      // Total up the hadronic W decays first...
      totalBR = decays->at("W+").BF("hadron", "hadron");
      totalBR = decays->at("~chi+_1").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_1").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "c", "sbar");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // char2, neut1
      xsecLimit = limitContainer->limitAverage(mass_char2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_22;
      // Total up all channels which look like hadronic W* decays
      // Total up the hadronic W decays first...
      totalBR = decays->at("W+").BF("hadron", "hadron");
      totalBR = decays->at("~chi+_2").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_2").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "c", "sbar");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void OPAL_Chargino_SemiLeptonic_Conservative_LLike(double& result)
    {
      static OPALCharginoSemiLeptonicLimitAt208GeV *limitContainer = new OPALCharginoSemiLeptonicLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(75., 105., 0., 105.,
                                     "lepLimitPlanev2/OPALCharginoSemiLeptonicLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::OPAL_Chargino_SemiLeptonic_Conservative_LLike;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const SubSpectrum *mssm = spec->get_HE();
      const DecayTable *decays = &(*Dep::decay_rates);
      const str snue = slhahelp::mass_es_from_gauge_es("~nu_e_L", mssm, tol, LOCAL_INFO, pt_error);
      const str snumu = slhahelp::mass_es_from_gauge_es("~nu_mu_L", mssm, tol, LOCAL_INFO, pt_error);
      const str snutau = slhahelp::mass_es_from_gauge_es("~nu_tau_L", mssm, tol, LOCAL_INFO, pt_error);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_char1 = spec->get(Par::Pole_Mass,1000024, 0);
      const double mass_char2 = spec->get(Par::Pole_Mass,1000037, 0);
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // char1, neut1
      xsecLimit = limitContainer->limitAverage(mass_char1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_11;
      // Total up all channels which look like leptonic W* decays
      // Total up the leptonic W decays first...
      totalBR = 0;
      totalBR += decays->at("W+").BF("e+", "nu_e");
      totalBR += decays->at("W+").BF("mu+", "nu_mu");
      totalBR += decays->at("W+").BF("tau+", "nu_tau");
      totalBR = decays->at("~chi+_1").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_1").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "tau+", "nu_tau");
      totalBR += decays->at("~chi+_1").BF(snue, "e+")
               * decays->at(snue).BF("~chi0_1", "nu_e");
      totalBR += decays->at("~chi+_1").BF(snumu, "mu+")
               * decays->at(snumu).BF("~chi0_1", "nu_mu");
      totalBR += decays->at("~chi+_1").BF(snutau, "tau+")
               * decays->at(snutau).BF("~chi0_1", "nu_tau");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      // ALSO, total up all channels which look like hadronic W* decays
      // Total up the hadronic W decays first...
      totalBR = decays->at("W+").BF("hadron", "hadron");
      totalBR = decays->at("~chi+_1").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_1").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "c", "sbar");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // char2, neut1
      xsecLimit = limitContainer->limitAverage(mass_char2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_22;
      // Total up all channels which look like leptonic W* decays
      // Total up the leptonic W decays first...
      totalBR = 0;
      totalBR += decays->at("W+").BF("e+", "nu_e");
      totalBR += decays->at("W+").BF("mu+", "nu_mu");
      totalBR += decays->at("W+").BF("tau+", "nu_tau");
      totalBR = decays->at("~chi+_2").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_2").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "tau+", "nu_tau");
      totalBR += decays->at("~chi+_2").BF(snue, "e+")
               * decays->at(snue).BF("~chi0_1", "nu_e");
      totalBR += decays->at("~chi+_2").BF(snumu, "mu+")
               * decays->at(snumu).BF("~chi0_1", "nu_mu");
      totalBR += decays->at("~chi+_2").BF(snutau, "tau+")
               * decays->at(snutau).BF("~chi0_1", "nu_tau");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      // ALSO, total up all channels which look like hadronic W* decays
      // Total up the hadronic W decays first...
      totalBR = decays->at("W+").BF("hadron", "hadron");
      totalBR = decays->at("~chi+_2").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_2").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "c", "sbar");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void OPAL_Chargino_Leptonic_Conservative_LLike(double& result)
    {
      static OPALCharginoLeptonicLimitAt208GeV *limitContainer = new OPALCharginoLeptonicLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(75., 105., 0., 105.,
                                     "lepLimitPlanev2/OPALCharginoLeptonicLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::OPAL_Chargino_Leptonic_Conservative_LLike;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const SubSpectrum *mssm = spec->get_HE();
      const DecayTable *decays = &(*Dep::decay_rates);
      const str snue = slhahelp::mass_es_from_gauge_es("~nu_e_L", mssm, tol, LOCAL_INFO, pt_error);
      const str snumu = slhahelp::mass_es_from_gauge_es("~nu_mu_L", mssm, tol, LOCAL_INFO, pt_error);
      const str snutau = slhahelp::mass_es_from_gauge_es("~nu_tau_L", mssm, tol, LOCAL_INFO, pt_error);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_char1 = spec->get(Par::Pole_Mass,1000024, 0);
      const double mass_char2 = spec->get(Par::Pole_Mass,1000037, 0);
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // char1, neut1
      xsecLimit = limitContainer->limitAverage(mass_char1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_11;
      // Total up all channels which look like leptonic W* decays
      // Total up the leptonic W decays first...
      totalBR = 0;
      totalBR += decays->at("W+").BF("e+", "nu_e");
      totalBR += decays->at("W+").BF("mu+", "nu_mu");
      totalBR += decays->at("W+").BF("tau+", "nu_tau");
      totalBR = decays->at("~chi+_1").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_1").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "tau+", "nu_tau");
      totalBR += decays->at("~chi+_1").BF(snue, "e+")
               * decays->at(snue).BF("~chi0_1", "nu_e");
      totalBR += decays->at("~chi+_1").BF(snumu, "mu+")
               * decays->at(snumu).BF("~chi0_1", "nu_mu");
      totalBR += decays->at("~chi+_1").BF(snutau, "tau+")
               * decays->at(snutau).BF("~chi0_1", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // char2, neut1
      xsecLimit = limitContainer->limitAverage(mass_char2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_22;
      // Total up all channels which look like leptonic W* decays
      // Total up the leptonic W decays first...
      totalBR = 0;
      totalBR += decays->at("W+").BF("e+", "nu_e");
      totalBR += decays->at("W+").BF("mu+", "nu_mu");
      totalBR += decays->at("W+").BF("tau+", "nu_tau");
      totalBR = decays->at("~chi+_2").BF("~chi0_1", "W+") * totalBR;

      totalBR += decays->at("~chi+_2").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "tau+", "nu_tau");
      totalBR += decays->at("~chi+_2").BF(snue, "e+")
               * decays->at(snue).BF("~chi0_1", "nu_e");
      totalBR += decays->at("~chi+_2").BF(snumu, "mu+")
               * decays->at(snumu).BF("~chi0_1", "nu_mu");
      totalBR += decays->at("~chi+_2").BF(snutau, "tau+")
               * decays->at(snutau).BF("~chi0_1", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void OPAL_Chargino_All_Channels_Conservative_LLike(double& result)
    {
      static OPALCharginoAllChannelsLimitAt208GeV *limitContainer = new OPALCharginoAllChannelsLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(75., 105., 0., 105.,
                                     "lepLimitPlanev2/OPALCharginoAllChannelsLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::OPAL_Chargino_All_Channels_Conservative_LLike;
      const static double tol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
      const static bool pt_error = runOptions->getValueOrDef<bool>(true, "off_diagonal_tolerance_invalidates_point_only");

      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const SubSpectrum *mssm = spec->get_HE();
      const DecayTable *decays = &(*Dep::decay_rates);
      const str snue = slhahelp::mass_es_from_gauge_es("~nu_e_L", mssm, tol, LOCAL_INFO, pt_error);
      const str snumu = slhahelp::mass_es_from_gauge_es("~nu_mu_L", mssm, tol, LOCAL_INFO, pt_error);
      const str snutau = slhahelp::mass_es_from_gauge_es("~nu_tau_L", mssm, tol, LOCAL_INFO, pt_error);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_char1 = spec->get(Par::Pole_Mass,1000024, 0);
      const double mass_char2 = spec->get(Par::Pole_Mass,1000037, 0);
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // char1, neut1
      xsecLimit = limitContainer->limitAverage(mass_char1, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_11;
      // Total up all channels which look like W* decays
      totalBR = 0;
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "W+");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "c", "sbar");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_1").BF("~chi0_1", "tau+", "nu_tau");
      totalBR += decays->at("~chi+_1").BF(snue, "e+")
               * decays->at(snue).BF("~chi0_1", "nu_e");
      totalBR += decays->at("~chi+_1").BF(snumu, "mu+")
               * decays->at(snumu).BF("~chi0_1", "nu_mu");
      totalBR += decays->at("~chi+_1").BF(snutau, "tau+")
               * decays->at(snutau).BF("~chi0_1", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // char2, neut1
      xsecLimit = limitContainer->limitAverage(mass_char2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chipm_22;
      // Total up all channels which look like W* decays
      totalBR = 0;
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "W+");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "u", "dbar");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "c", "sbar");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "e+", "nu_e");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "mu+", "nu_mu");
      totalBR += decays->at("~chi+_2").BF("~chi0_1", "tau+", "nu_tau");
      totalBR += decays->at("~chi+_2").BF(snue, "e+")
               * decays->at(snue).BF("~chi0_1", "nu_e");
      totalBR += decays->at("~chi+_2").BF(snumu, "mu+")
               * decays->at(snumu).BF("~chi0_1", "nu_mu");
      totalBR += decays->at("~chi+_2").BF(snutau, "tau+")
               * decays->at(snutau).BF("~chi0_1", "nu_tau");
      xsecWithError.upper *= pow(totalBR, 2);
      xsecWithError.central *= pow(totalBR, 2);
      xsecWithError.lower *= pow(totalBR, 2);

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    void OPAL_Neutralino_Hadronic_Conservative_LLike(double& result)
    {
      static OPALNeutralinoHadronicLimitAt208GeV *limitContainer = new OPALNeutralinoHadronicLimitAt208GeV();
#ifdef DUMP_LIMIT_PLOT_DATA
      static bool dumped=false;
      if(!dumped) {
        limitContainer->dumpPlotData(0., 200., 0., 100.,
                                     "lepLimitPlanev2/OPALNeutralinoHadronicLimitAt208GeV.dump");
        dumped=true;
      }
#endif
      using namespace Pipes::OPAL_Neutralino_Hadronic_Conservative_LLike;
      using std::pow;
      using std::log;

      const Spectrum *spec = *Dep::MSSM_spectrum;
      const DecayTable *decays = &(*Dep::decay_rates);
      const double mass_neut1 = spec->get(Par::Pole_Mass,1000022, 0); 
      const double mass_neut2 = spec->get(Par::Pole_Mass,1000023, 0); 
      const double mass_neut3 = spec->get(Par::Pole_Mass,1000025, 0); 
      const double mass_neut4 = spec->get(Par::Pole_Mass,1000035, 0); 
      const double mZ = spec->get(Par::Pole_Mass,23, 0);
      triplet<double> xsecWithError;
      double xsecLimit, totalBR;

      result = 0;
      // Due to the nature of the analysis details of the model independent limit in
      // the paper, the best we can do is to try these processes individually:

      // neut2, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut2, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chi00_12;
      // Total up all channels which look like Z* decays
      totalBR = decays->at("Z0").BF("hadron", "hadron");
      totalBR = decays->at("~chi0_2").BF("~chi0_1", "Z0") * totalBR;
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "ubar", "u");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "dbar", "d");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "cbar", "c");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "sbar", "s");
      totalBR += decays->at("~chi0_2").BF("~chi0_1", "bbar", "b");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // neut3, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut3, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chi00_13;
      // Total up all channels which look like Z* decays
      totalBR = decays->at("Z0").BF("hadron", "hadron");
      totalBR = decays->at("~chi0_3").BF("~chi0_1", "Z0") * totalBR;
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "ubar", "u");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "dbar", "d");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "cbar", "c");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "sbar", "s");
      totalBR += decays->at("~chi0_3").BF("~chi0_1", "bbar", "b");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

      // neut4, neut1
      xsecLimit = limitContainer->limitAverage(mass_neut4, mass_neut1, mZ);

      xsecWithError = *Dep::LEP208_xsec_chi00_14;
      // Total up all channels which look like Z* decays
      totalBR = decays->at("Z0").BF("hadron", "hadron");
      totalBR = decays->at("~chi0_4").BF("~chi0_1", "Z0") * totalBR;
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "ubar", "u");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "dbar", "d");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "cbar", "c");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "sbar", "s");
      totalBR += decays->at("~chi0_4").BF("~chi0_1", "bbar", "b");
      xsecWithError.upper *= totalBR;
      xsecWithError.central *= totalBR;
      xsecWithError.lower *= totalBR;

      if (xsecWithError.central < xsecLimit) {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.upper - xsecWithError.central));
      } else {
        result += log(limitLike(xsecWithError.central, xsecLimit, xsecWithError.central - xsecWithError.lower));
      }

    }

    /// @}


    // *** Higgs physics ***

    /// FeynHiggs Higgs production cross-sections
    void FH_HiggsProd(fh_HiggsProd &result)
    {
      using namespace Pipes::FH_HiggsProd;

      Farray<fh_real, 1,52> prodxs;

      fh_HiggsProd HiggsProd;
      int error;
      fh_real sqrts;

      // Tevatron
      sqrts = 2.;
      error = 1;
      BEreq::FHHiggsProd(error, sqrts, prodxs);
      if (error != 0)
      {
        std::ostringstream err;
        err << "BEreq::FHHiggsProd raised error flag for Tevatron: " << error << ".";
        invalid_point().raise(err.str());
      }
      for(int i = 0; i < 52; i++) HiggsProd.prodxs_Tev[i] = prodxs(i+1);
      // LHC7
      sqrts = 7.;
      error = 1;
      BEreq::FHHiggsProd(error, sqrts, prodxs);
      if (error != 0)
      {
        std::ostringstream err;
        err << "BEreq::FHHiggsProd raised error flag for LHC7: " << error << "."; 
        invalid_point().raise(err.str());
      }
      for(int i = 0; i < 52; i++) HiggsProd.prodxs_LHC7[i] = prodxs(i+1);
      // LHC8
      sqrts = 8.;
      error = 1;
      BEreq::FHHiggsProd(error, sqrts, prodxs);
      if (error != 0)
      {
        std::ostringstream err;
        err << "BEreq::FHHiggsProd raised error flag for LHC8: " << error << "."; 
        invalid_point().raise(err.str());
      }
      for(int i = 0; i < 52; i++) HiggsProd.prodxs_LHC8[i] = prodxs(i+1);

      result = HiggsProd;
    }

    /// SM Higgs only model parameters
    void SMHiggs_ModelParameters(hb_ModelParameters &result)
    {
      using namespace Pipes::SMHiggs_ModelParameters;

      for(int i = 0; i < 3; i++)
      {
        result.Mh[i] = 0.;
        result.deltaMh[i] = 0.;
        result.hGammaTot[i] = 0.;
        result.CP[i] = 0.;
        result.CS_lep_hjZ_ratio[i] = 0.;
        result.CS_lep_bbhj_ratio[i] = 0.;
        result.CS_lep_tautauhj_ratio[i] = 0.;
        for(int j = 0; j < 3; j++) result.CS_lep_hjhi_ratio[i][j] = 0.;
        result.CS_gg_hj_ratio[i] = 0.;
        result.CS_bb_hj_ratio[i] = 0.;
        result.CS_bg_hjb_ratio[i] = 0.;
        result.CS_ud_hjWp_ratio[i] = 0.;
        result.CS_cs_hjWp_ratio[i] = 0.;
        result.CS_ud_hjWm_ratio[i] = 0.;
        result.CS_cs_hjWm_ratio[i] = 0.;
        result.CS_gg_hjZ_ratio[i] = 0.;
        result.CS_dd_hjZ_ratio[i] = 0.;
        result.CS_uu_hjZ_ratio[i] = 0.;
        result.CS_ss_hjZ_ratio[i] = 0.;
        result.CS_cc_hjZ_ratio[i] = 0.;
        result.CS_bb_hjZ_ratio[i] = 0.;
        result.CS_tev_vbf_ratio[i] = 0.;
        result.CS_tev_tthj_ratio[i] = 0.;
        result.CS_lhc7_vbf_ratio[i] = 0.;
        result.CS_lhc7_tthj_ratio[i] = 0.;
        result.CS_lhc8_vbf_ratio[i] = 0.;
        result.CS_lhc8_tthj_ratio[i] = 0.;
        result.BR_hjss[i] = 0.;
        result.BR_hjcc[i] = 0.;
        result.BR_hjbb[i] = 0.;
        result.BR_hjmumu[i] = 0.;
        result.BR_hjtautau[i] = 0.;
        result.BR_hjWW[i] = 0.;
        result.BR_hjZZ[i] = 0.;
        result.BR_hjZga[i] = 0.;
        result.BR_hjgaga[i] = 0.;
        result.BR_hjgg[i] = 0.;
        result.BR_hjinvisible[i] = 0.;
        for(int j = 0; j < 3; j++) result.BR_hjhihi[i][j] = 0.;
      }

      result.MHplus = 0.;
      result.deltaMHplus = 0.;
      result.HpGammaTot = 0.;
      result.CS_lep_HpjHmi_ratio = 0.;
      result.BR_tWpb = 0.;
      result.BR_tHpjb = 0.;
      result.BR_Hpjcs = 0.;
      result.BR_Hpjcb = 0.;
      result.BR_Hptaunu = 0.;

      const Spectrum* fullspectrum = *Dep::SM_spectrum;
      const SubSpectrum* spec = fullspectrum->get_HE();
      const DecayTable::Entry* decays = &(*Dep::Higgs_decay_rates);

      result.Mh[0] = spec->phys().get(Par::Pole_Mass,25,0);

      result.deltaMh[0] = 0.; // Need to get theoretical error on mass
      result.hGammaTot[0] = decays->width_in_GeV;
      result.CP[0] = 1;
      result.CS_lep_hjZ_ratio[0] = 1.;
      result.CS_lep_bbhj_ratio[0] = 1.;
      result.CS_lep_tautauhj_ratio[0] = 1.;
      result.CS_gg_hj_ratio[0] = 1.;
      result.CS_bb_hj_ratio[0] = 1.;
      result.CS_bg_hjb_ratio[0] = 1.;
      result.CS_ud_hjWp_ratio[0] = 1.;
      result.CS_cs_hjWp_ratio[0] = 1.;
      result.CS_ud_hjWm_ratio[0] = 1.;
      result.CS_cs_hjWm_ratio[0] = 1.;
      result.CS_gg_hjZ_ratio[0] = 1.;
      result.CS_dd_hjZ_ratio[0] = 1.;
      result.CS_uu_hjZ_ratio[0] = 1.;
      result.CS_ss_hjZ_ratio[0] = 1.;
      result.CS_cc_hjZ_ratio[0] = 1.;
      result.CS_bb_hjZ_ratio[0] = 1.;
      result.CS_tev_vbf_ratio[0] = 1.;
      result.CS_tev_tthj_ratio[0] = 1.;
      result.CS_lhc7_vbf_ratio[0] = 1.;
      result.CS_lhc7_tthj_ratio[0] = 1.;
      result.CS_lhc8_vbf_ratio[0] = 1.;
      result.CS_lhc8_tthj_ratio[0] = 1.;
      result.BR_hjss[0] = decays->BF("s", "sbar");
      result.BR_hjcc[0] = decays->BF("c", "cbar");
      result.BR_hjbb[0] = decays->BF("b", "bbar");
      result.BR_hjmumu[0] = decays->BF("mu+", "mu-");
      result.BR_hjtautau[0] = decays->BF("tau+", "tau-");
      result.BR_hjWW[0] = decays->BF("W+", "W-");
      result.BR_hjZZ[0] = decays->BF("Z0", "Z0");
      result.BR_hjZga[0] = decays->BF("gamma", "Z0");
      result.BR_hjgaga[0] = decays->BF("gamma", "gamma");
      result.BR_hjgg[0] = decays->BF("g", "g");
    }

    /// MSSM Higgs model parameters
    void MSSMHiggs_ModelParameters(hb_ModelParameters &result)
    {
      using namespace Pipes::MSSMHiggs_ModelParameters;
      #define PDB Models::ParticleDB()

      // unpack FeynHiggs Couplings
      fh_Couplings FH_input = *Dep::Higgs_Couplings;

      std::vector<std::string> sHneut;
      sHneut.push_back("h0_1");
      sHneut.push_back("h0_2");
      sHneut.push_back("A0");

      const Spectrum* fullspectrum = *Dep::MSSM_spectrum;
      const SubSpectrum* spec = fullspectrum->get_HE();
      const DecayTable decaytable = *Dep::decay_rates;

      const DecayTable::Entry* Hneut_decays[3];
      for(int i = 0; i < 3; i++)
      {
        // Higgs masses and errors
        result.Mh[i] = spec->phys().get(Par::Pole_Mass,sHneut[i]);
        result.deltaMh[i] = 0.;
      }

      // invisible LSP?
      double lsp_mass = spec->phys().get(Par::Pole_Mass,"~chi0_1");
      int i_snu = 0;
      for(int i = 1; i <= 3; i++)
      {
        if(spec->phys().get(Par::Pole_Mass,"~nu",i)  < lsp_mass)
        {
          i_snu = i;
          lsp_mass = spec->phys().get(Par::Pole_Mass,"~nu",i);
        }
      }

      bool inv_lsp = true;
      if(spec->phys().get(Par::Pole_Mass,"~chi+",1) < lsp_mass) inv_lsp = false;
      if(spec->phys().get(Par::Pole_Mass,"~g") < lsp_mass) inv_lsp = false;
      if(inv_lsp)
      {
        for(int i = 1; i <= 6; i++)
        {
          if(spec->phys().get(Par::Pole_Mass,"~d",i) < lsp_mass)
          {
            inv_lsp = false;
            break;
          }
          if(spec->phys().get(Par::Pole_Mass,"~u",i) < lsp_mass)
          {
            inv_lsp = false;
            break;
          }
          if(spec->phys().get(Par::Pole_Mass,"~e-",i) < lsp_mass)
          {
            inv_lsp = false;
            break;
          }
        }
      }

      for(int i = 0; i < 3; i++)
      {
        // Branching ratios and total widths
        Hneut_decays[i] = &(decaytable(sHneut[i]));

        result.hGammaTot[i] = Hneut_decays[i]->width_in_GeV;

        result.BR_hjss[i] = Hneut_decays[i]->BF("s", "sbar");
        result.BR_hjcc[i] = Hneut_decays[i]->BF("c", "cbar");
        result.BR_hjbb[i] = Hneut_decays[i]->BF("b", "bbar");
        result.BR_hjmumu[i] = Hneut_decays[i]->BF("mu+", "mu-");
        result.BR_hjtautau[i] = Hneut_decays[i]->BF("tau+", "tau-");
        result.BR_hjWW[i] = Hneut_decays[i]->BF("W+", "W-");
        result.BR_hjZZ[i] = Hneut_decays[i]->BF("Z0", "Z0");
        result.BR_hjZga[i] = Hneut_decays[i]->BF("gamma", "Z0");
        result.BR_hjgaga[i] = Hneut_decays[i]->BF("gamma", "gamma");
        result.BR_hjgg[i] = Hneut_decays[i]->BF("g", "g");
        for(int j = 0; j < 3; j++)
        {
          if(2.*result.Mh[j] < result.Mh[i])
          {
            result.BR_hjhihi[i][j] = Hneut_decays[i]->BF(sHneut[j],sHneut[j]);
          }
          else
          {
            result.BR_hjhihi[i][j] = 0.;
          }
        }
        result.BR_hjinvisible[i] = 0.;
        if(inv_lsp)
        {
          // sneutrino is LSP - need to figure out how to get correct invisible BF...
          if(i_snu > 0)
          {
            result.BR_hjinvisible[i] += Hneut_decays[i]->BF(PDB.long_name("~nu",i_snu),PDB.long_name("~nubar",i_snu));
          }
          else
          {
            result.BR_hjinvisible[i] = Hneut_decays[i]->BF("~chi0_1","~chi0_1");
          }
        }
      }

      result.MHplus = spec->phys().get(Par::Pole_Mass,"H+");
      result.deltaMHplus = 0.;

      const DecayTable::Entry* Hplus_decays = &(decaytable("H+"));
      const DecayTable::Entry* top_decays = &(decaytable("t"));

      result.HpGammaTot = Hplus_decays->width_in_GeV;
      result.BR_tWpb    = top_decays->BF("W+", "b");
      result.BR_tHpjb   = top_decays->has_channel("H+", "b") ? top_decays->BF("H+", "b") : 0.0;
      result.BR_Hpjcs   = Hplus_decays->BF("c", "sbar");
      result.BR_Hpjcb   = Hplus_decays->BF("c", "bbar");
      result.BR_Hptaunu = Hplus_decays->BF("tau+", "nu_tau");

      // check SM partial width h0_1 -> b bbar
      // shouldn't be zero...
      double g2hjbb[3];
      for(int i = 0; i < 3; i++)
      {
        if(FH_input.gammas_sm[H0FF(i,4,3,3)+4] <= 0.)
          g2hjbb[i] = 0.;
        else
          g2hjbb[i] = FH_input.gammas[H0FF(i,4,3,3)+4]/FH_input.gammas_sm[H0FF(i,4,3,3)+4];
      }

      // using partial width ratio approximation for
      // h -> b bbar CS ratios
      for(int i = 0; i < 3; i++)
      {
        result.CS_bg_hjb_ratio[i] = g2hjbb[i];
        result.CS_bb_hj_ratio[i]  = g2hjbb[i];
      }

      // cross-section ratios for b bbar and tau+ tau- final states
      for(int i = 0; i < 3; i++)
      {
        fh_complex c_g2hjbb_L = FH_input.couplings[H0FF(i,4,3,3)];
        fh_complex c_g2hjbb_R = FH_input.couplings[H0FF(i,4,3,3)+Roffset];
        fh_complex c_g2hjbb_SM_L = FH_input.couplings_sm[H0FF(i,4,3,3)];
        fh_complex c_g2hjbb_SM_R = FH_input.couplings_sm[H0FF(i,4,3,3)+RSMoffset];

        fh_complex c_g2hjtautau_L = FH_input.couplings[H0FF(i,2,3,3)];
        fh_complex c_g2hjtautau_R = FH_input.couplings[H0FF(i,2,3,3)+Roffset];
        fh_complex c_g2hjtautau_SM_L = FH_input.couplings_sm[H0FF(i,2,3,3)];
        fh_complex c_g2hjtautau_SM_R = FH_input.couplings_sm[H0FF(i,2,3,3)+RSMoffset];

        double R_g2hjbb_L = sqrt(c_g2hjbb_L.re*c_g2hjbb_L.re+
               c_g2hjbb_L.im*c_g2hjbb_L.im)/
          sqrt(c_g2hjbb_SM_L.re*c_g2hjbb_SM_L.re+
               c_g2hjbb_SM_L.im*c_g2hjbb_SM_L.im);
        double R_g2hjbb_R = sqrt(c_g2hjbb_R.re*c_g2hjbb_R.re+
               c_g2hjbb_R.im*c_g2hjbb_R.im)/
          sqrt(c_g2hjbb_SM_R.re*c_g2hjbb_SM_R.re+
               c_g2hjbb_SM_R.im*c_g2hjbb_SM_R.im);

        double R_g2hjtautau_L = sqrt(c_g2hjtautau_L.re*c_g2hjtautau_L.re+
                   c_g2hjtautau_L.im*c_g2hjtautau_L.im)/
          sqrt(c_g2hjtautau_SM_L.re*c_g2hjtautau_SM_L.re+
               c_g2hjtautau_SM_L.im*c_g2hjtautau_SM_L.im);
        double R_g2hjtautau_R = sqrt(c_g2hjtautau_R.re*c_g2hjtautau_R.re+
                   c_g2hjtautau_R.im*c_g2hjtautau_R.im)/
          sqrt(c_g2hjtautau_SM_R.re*c_g2hjtautau_SM_R.re+
               c_g2hjtautau_SM_R.im*c_g2hjtautau_SM_R.im);

        double g2hjbb_s = (R_g2hjbb_L+R_g2hjbb_R)*(R_g2hjbb_L+R_g2hjbb_R)/4.;
        double g2hjbb_p = (R_g2hjbb_L-R_g2hjbb_R)*(R_g2hjbb_L-R_g2hjbb_R)/4.;
        double g2hjtautau_s = (R_g2hjtautau_L+R_g2hjtautau_R)*(R_g2hjtautau_L+R_g2hjtautau_R)/4.;
        double g2hjtautau_p = (R_g2hjtautau_L-R_g2hjtautau_R)*(R_g2hjtautau_L-R_g2hjtautau_R)/4.;

        // check CP of state
        if(g2hjbb_p < 1e-10)
          result.CP[i] = 1;
        else if(g2hjbb_s < 1e-10)
          result.CP[i] = -1;
        else
          result.CP[i] = 0.;

        result.CS_lep_bbhj_ratio[i]     = g2hjbb_s + g2hjbb_p;
        result.CS_lep_tautauhj_ratio[i] = g2hjtautau_s + g2hjtautau_p;
      }

      // cross-section ratios for di-boson final states
      for(int i = 0; i < 3; i++)
      {
        fh_complex c_gWW = FH_input.couplings[H0VV(i,4)];
        fh_complex c_gWW_SM = FH_input.couplings_sm[H0VV(i,4)];
        fh_complex c_gZZ = FH_input.couplings[H0VV(i,3)];
        fh_complex c_gZZ_SM = FH_input.couplings_sm[H0VV(i,3)];

        double g2hjWW = (c_gWW.re*c_gWW.re+c_gWW.im*c_gWW.im)/
          (c_gWW_SM.re*c_gWW_SM.re+c_gWW_SM.im*c_gWW_SM.im);

        double g2hjZZ = (c_gZZ.re*c_gZZ.re+c_gZZ.im*c_gZZ.im)/
          (c_gZZ_SM.re*c_gZZ_SM.re+c_gZZ_SM.im*c_gZZ_SM.im);

        result.CS_lep_hjZ_ratio[i] = g2hjZZ;

        result.CS_gg_hjZ_ratio[i] = 0.;
        result.CS_dd_hjZ_ratio[i] = g2hjZZ;
        result.CS_uu_hjZ_ratio[i] = g2hjZZ;
        result.CS_ss_hjZ_ratio[i] = g2hjZZ;
        result.CS_cc_hjZ_ratio[i] = g2hjZZ;
        result.CS_bb_hjZ_ratio[i] = g2hjZZ;

        result.CS_ud_hjWp_ratio[i] = g2hjWW;
        result.CS_cs_hjWp_ratio[i] = g2hjWW;
        result.CS_ud_hjWm_ratio[i] = g2hjWW;
        result.CS_cs_hjWm_ratio[i] = g2hjWW;

        result.CS_tev_vbf_ratio[i]   = g2hjWW;
        result.CS_lhc7_vbf_ratio[i]  = g2hjWW;
        result.CS_lhc8_tthj_ratio[i] = g2hjWW;
      }

      // higgs to higgs + V xsection ratios
      // retrive SMInputs dependency
      const SMInputs& sminputs = *Dep::SMINPUTS;

      double norm = sminputs.GF*sqrt(2.)*sminputs.mZ*sminputs.mZ;
      for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++)
      {
        fh_complex c_gHV = FH_input.couplings[H0HV(i,j)];
        double g2HV = c_gHV.re*c_gHV.re+c_gHV.im*c_gHV.im;
        result.CS_lep_hjhi_ratio[i][j] = g2HV/norm;
      }

      // gluon fusion x-section ratio
      for(int i = 0; i < 3; i++)
      {
        if(FH_input.gammas_sm[H0VV(i,5)] <= 0.)
          result.CS_gg_hj_ratio[i] = 0.;
        else
          result.CS_gg_hj_ratio[i] = FH_input.gammas[H0VV(i,5)]/
            FH_input.gammas_sm[H0VV(i,5)];
      }

      // unpack FeynHiggs x-sections
      fh_HiggsProd FH_prod = *Dep::FH_HiggsProd;

      // h t tbar xsection ratios
      for(int i = 0; i < 3; i++)
      {
        result.CS_tev_tthj_ratio[i] = 0.;
        result.CS_lhc7_tthj_ratio[i] = 0.;
        result.CS_lhc8_tthj_ratio[i] = 0.;
        if(FH_prod.prodxs_Tev[i+30] > 0.)
          result.CS_tev_tthj_ratio[i]  = FH_prod.prodxs_Tev[i+27]/FH_prod.prodxs_Tev[i+30];
        if(FH_prod.prodxs_Tev[i+30] > 0.)
          result.CS_lhc7_tthj_ratio[i] = FH_prod.prodxs_LHC7[i+27]/FH_prod.prodxs_LHC7[i+30];
        if(FH_prod.prodxs_Tev[i+30] > 0.)
          result.CS_lhc8_tthj_ratio[i] = FH_prod.prodxs_LHC8[i+27]/FH_prod.prodxs_LHC8[i+30];
      }
      // LEP H+ H- x-section ratio
      result.CS_lep_HpjHmi_ratio = 1.;
    }

    /// Get a LEP chisq from HiggsBounds
    void HB_LEP_lnL(double &result)
    {
      using namespace Pipes::HB_LEP_lnL;

      hb_ModelParameters ModelParam = *Dep::HB_ModelParameters;

      Farray<double, 1,3, 1,3> CS_lep_hjhi_ratio;
      Farray<double, 1,3, 1,3> BR_hjhihi;
      for(int i = 0; i < 3; i++) for(int j = 0; j < 3; j++)
      {
        CS_lep_hjhi_ratio(i+1,j+1) = ModelParam.CS_lep_hjhi_ratio[i][j];
        BR_hjhihi(i+1,j+1) = ModelParam.BR_hjhihi[i][j];
      }

      BEreq::HiggsBounds_neutral_input_part(&ModelParam.Mh[0], &ModelParam.hGammaTot[0], &ModelParam.CP[0],
              &ModelParam.CS_lep_hjZ_ratio[0], &ModelParam.CS_lep_bbhj_ratio[0],
              &ModelParam.CS_lep_tautauhj_ratio[0], CS_lep_hjhi_ratio,
              &ModelParam.CS_gg_hj_ratio[0], &ModelParam.CS_bb_hj_ratio[0],
              &ModelParam.CS_bg_hjb_ratio[0], &ModelParam.CS_ud_hjWp_ratio[0],
              &ModelParam.CS_cs_hjWp_ratio[0], &ModelParam.CS_ud_hjWm_ratio[0],
              &ModelParam.CS_cs_hjWm_ratio[0], &ModelParam.CS_gg_hjZ_ratio[0],
              &ModelParam.CS_dd_hjZ_ratio[0], &ModelParam.CS_uu_hjZ_ratio[0],
              &ModelParam.CS_ss_hjZ_ratio[0], &ModelParam.CS_cc_hjZ_ratio[0],
              &ModelParam.CS_bb_hjZ_ratio[0], &ModelParam.CS_tev_vbf_ratio[0],
              &ModelParam.CS_tev_tthj_ratio[0], &ModelParam.CS_lhc7_vbf_ratio[0],
              &ModelParam.CS_lhc7_tthj_ratio[0], &ModelParam.CS_lhc8_vbf_ratio[0],
              &ModelParam.CS_lhc8_tthj_ratio[0], &ModelParam.BR_hjss[0],
              &ModelParam.BR_hjcc[0], &ModelParam.BR_hjbb[0],
              &ModelParam.BR_hjmumu[0], &ModelParam.BR_hjtautau[0],
              &ModelParam.BR_hjWW[0], &ModelParam.BR_hjZZ[0],
              &ModelParam.BR_hjZga[0], &ModelParam.BR_hjgaga[0],
              &ModelParam.BR_hjgg[0], &ModelParam.BR_hjinvisible[0], BR_hjhihi);

      BEreq::HiggsBounds_charged_input(&ModelParam.MHplus, &ModelParam.HpGammaTot, &ModelParam.CS_lep_HpjHmi_ratio,
               &ModelParam.BR_tWpb, &ModelParam.BR_tHpjb, &ModelParam.BR_Hpjcs,
               &ModelParam.BR_Hpjcb, &ModelParam.BR_Hptaunu);

      BEreq::HiggsBounds_set_mass_uncertainties(&ModelParam.deltaMh[0],&ModelParam.deltaMHplus);

      // run Higgs bounds 'classic'
      double HBresult, obsratio;
      int chan, ncombined;
      BEreq::run_HiggsBounds_classic(HBresult,chan,obsratio,ncombined);

      // extract the LEP chisq
      double chisq_withouttheory,chisq_withtheory;
      int chan2;
      double theor_unc = 1.5; // theory uncertainty
      BEreq::HB_calc_stats(theor_unc,chisq_withouttheory,chisq_withtheory,chan2);

      result = -0.5*chisq_withouttheory;
      //std::cout << "Calculating LEP chisq: " << chisq_withouttheory << " (no theor), " << chisq_withtheory << " (with theor)" << endl;

    }

    /// Get an LHC chisq from HiggsSignals
    void HS_LHC_lnL(double &result)
    {
      using namespace Pipes::HS_LHC_lnL;

      hb_ModelParameters ModelParam = *Dep::HB_ModelParameters;

      Farray<double, 1,3, 1,3> CS_lep_hjhi_ratio;
      Farray<double, 1,3, 1,3> BR_hjhihi;
      for(int i = 0; i < 3; i++) for(int j = 0; j < 3; j++)
      {
        CS_lep_hjhi_ratio(i+1,j+1) = ModelParam.CS_lep_hjhi_ratio[i][j];
        BR_hjhihi(i+1,j+1) = ModelParam.BR_hjhihi[i][j];
      }

      BEreq::HiggsBounds_neutral_input_part_HS(&ModelParam.Mh[0], &ModelParam.hGammaTot[0], &ModelParam.CP[0],
                 &ModelParam.CS_lep_hjZ_ratio[0], &ModelParam.CS_lep_bbhj_ratio[0],
                 &ModelParam.CS_lep_tautauhj_ratio[0], CS_lep_hjhi_ratio,
                 &ModelParam.CS_gg_hj_ratio[0], &ModelParam.CS_bb_hj_ratio[0],
                 &ModelParam.CS_bg_hjb_ratio[0], &ModelParam.CS_ud_hjWp_ratio[0],
                 &ModelParam.CS_cs_hjWp_ratio[0], &ModelParam.CS_ud_hjWm_ratio[0],
                 &ModelParam.CS_cs_hjWm_ratio[0], &ModelParam.CS_gg_hjZ_ratio[0],
                 &ModelParam.CS_dd_hjZ_ratio[0], &ModelParam.CS_uu_hjZ_ratio[0],
                 &ModelParam.CS_ss_hjZ_ratio[0], &ModelParam.CS_cc_hjZ_ratio[0],
                 &ModelParam.CS_bb_hjZ_ratio[0], &ModelParam.CS_tev_vbf_ratio[0],
                 &ModelParam.CS_tev_tthj_ratio[0], &ModelParam.CS_lhc7_vbf_ratio[0],
                 &ModelParam.CS_lhc7_tthj_ratio[0], &ModelParam.CS_lhc8_vbf_ratio[0],
                 &ModelParam.CS_lhc8_tthj_ratio[0], &ModelParam.BR_hjss[0],
                 &ModelParam.BR_hjcc[0], &ModelParam.BR_hjbb[0],
                 &ModelParam.BR_hjmumu[0], &ModelParam.BR_hjtautau[0],
                 &ModelParam.BR_hjWW[0], &ModelParam.BR_hjZZ[0],
                 &ModelParam.BR_hjZga[0], &ModelParam.BR_hjgaga[0],
                 &ModelParam.BR_hjgg[0], &ModelParam.BR_hjinvisible[0], BR_hjhihi);

      BEreq::HiggsBounds_charged_input_HS(&ModelParam.MHplus, &ModelParam.HpGammaTot, &ModelParam.CS_lep_HpjHmi_ratio,
            &ModelParam.BR_tWpb, &ModelParam.BR_tHpjb, &ModelParam.BR_Hpjcs,
            &ModelParam.BR_Hpjcb, &ModelParam.BR_Hptaunu);

      BEreq::HiggsSignals_neutral_input_MassUncertainty(&ModelParam.deltaMh[0]);

      // add uncertainties to cross-sections and branching ratios
      // double dCS[5];
      // double dBR[5];
      // BEreq::setup_rate_uncertainties(dCS,dBR);

      // run HiggsSignals
      int mode = 1; // 1- peak-centered chi2 method (recommended)
      double csqmu, csqmh, csqtot, Pvalue;
      int nobs;
      BEreq::run_HiggsSignals(mode, csqmu, csqmh, csqtot, nobs, Pvalue);


      result = -0.5*csqtot;
      //std::cout << "Calculating LHC chisq: " << csqmu << " (signal strength only), " << csqmh << " (mass only), ";
      //std::cout << csqtot << " (both), Nobs: " << nobs << ", Pvalue: " << Pvalue << endl;

    }


  }
}
#undef DEBUG
