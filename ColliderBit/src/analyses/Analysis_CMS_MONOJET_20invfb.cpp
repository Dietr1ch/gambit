#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>

#include "gambit/ColliderBit/analyses/BaseAnalysis.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
//#include "gambit/ColliderBit/mt2w.h"

/// @todo Remove the ROOT classes...

using namespace std;

// The CMS monojet analysis (20fb^-1)

// based on: http://lanl.arxiv.org/pdf/1408.3583v1.pdf

//    Code by Martin White
//    Known issues:
//    a) Not validated against a CMS cutflow.
//    b) Overlap removal is not applied (CMS do not use it, but we don't exactly use their particle flow technique either)
//    c) Jets here need kT radius of 0.5 not 0.4

namespace Gambit {
  namespace ColliderBit {

    class Analysis_CMS_MONOJET_20invfb : public HEPUtilsAnalysis {
    private:

      // Numbers passing cuts
      int _num250,_num300,_num350,_num400,_num450,_num500,_num550;

      vector<int> cutFlowVector;
      vector<string> cutFlowVector_str;
      int NCUTS; //=24;

      // Debug histos

    public:

      Analysis_CMS_MONOJET_20invfb()
        : _num250(0),_num300(0),_num350(0),_num400(0),_num450(0),_num500(0),_num550(0),
          NCUTS(12)
      {
        for (int i=0; i<NCUTS; i++) {
          cutFlowVector.push_back(0);
          cutFlowVector_str.push_back("");
        }
      }

      double SmallestdPhi(std::vector<HEPUtils::Jet *> jets,double phi_met)
      {
        if (jets.size()<2) return(999);
        double dphi1 = std::acos(std::cos(jets.at(0)->phi()-phi_met));
        double dphi2 = std::acos(std::cos(jets.at(1)->phi()-phi_met));
        //double dphi3 = 999;
        //if (jets.size() > 2 && jets[2]->pT() > 40.)
        //  dphi3 = std::acos(std::cos(jets[2]->phi() - phi_met));
        double min1 = std::min(dphi1, dphi2);

        return min1;

      }

      void analyze(const HEPUtils::Event* event) {

        // Missing energy
        //HEPUtils::P4 ptot = event->missingmom();
        double met = event->met();

        // Now define vectors of baseline objects

        vector<HEPUtils::Particle*> baselineElectrons;
        for (HEPUtils::Particle* electron : event->electrons()) {
          if (electron->pT() > 10. && fabs(electron->eta()) < 2.5) {
            baselineElectrons.push_back(electron);
          }
        }
        vector<HEPUtils::Particle*> baselineMuons;
        for (HEPUtils::Particle* muon : event->muons()) {
          if (muon->pT() > 10. && fabs(muon->eta()) < 2.5) {
            baselineMuons.push_back(muon);
          }
        }

        vector<HEPUtils::Particle*> baselineTaus;
        for (HEPUtils::Particle* tau : event->taus()) {
          if (tau->pT() > 20. && fabs(tau->eta()) < 2.3) {
            baselineTaus.push_back(tau);
          }
        }

        vector<HEPUtils::Jet*> baselineJets;
        vector<HEPUtils::P4> jets;

        for (HEPUtils::Jet* jet : event->jets()) {
          if (jet->pT() > 30. && fabs(jet->eta()) < 4.5) {
            baselineJets.push_back(jet);
          }
        }

        // Calculate common variables and cuts first
        //applyTightIDElectronSelection(signalElectrons);

        //int nElectrons = signalElectrons.size();
        //int nMuons = signalMuons.size();
        int nJets = baselineJets.size();
        int nLeptons = baselineElectrons.size()+baselineMuons.size()+baselineTaus.size();

        // CUTS
        // pT(j1) > 110 GeV & eta < 2.4
        // njets <=2
        // dPhi(j1,j2) < 2.5
        // nLeptons = 0
        // met > 250
        // met > 300
        // met > 350
        // met > 400
        // met > 450
        // met > 500
        // met > 550

        cutFlowVector_str[0] = "No cuts ";
        cutFlowVector_str[1] = "pT(j1) > 110 GeV and |eta(j1)| < 2.4 ";
        cutFlowVector_str[2] = "njets <=2 ";
        cutFlowVector_str[3] = "dPhi(j1,j2) < 2.5 ";
        cutFlowVector_str[4] = "nLeptons = 0 ";
        cutFlowVector_str[5] = "met > 250 ";
        cutFlowVector_str[6] = "met > 300 ";
        cutFlowVector_str[7] = "met > 350 ";
        cutFlowVector_str[8] = "met > 400 ";
        cutFlowVector_str[9] = "met > 450 ";
        cutFlowVector_str[10] = "met > 500 ";
        cutFlowVector_str[11] = "met > 550 ";

        double dPhiJ1J2 = 5.;
        if(nJets>=2)dPhiJ1J2=acos(cos((baselineJets[0]->phi() - baselineJets[1]->phi())));


        for(int j=0;j<NCUTS;j++){
          if(
             (j==0) ||

             (j==1 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4) ||

             (j==2 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2) ||

             (j==3 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5) ||

             (j==4 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0) ||

             (j==5 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 250.) ||

             (j==6 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 300.) ||

             (j==7 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 350.) ||

             (j==8 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 400.) ||

             (j==9 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 450.) ||

             (j==10 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 500.) ||

             (j==11 && nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 550.))

            cutFlowVector[j]++;
        }

        //We're now ready to apply the cuts for each signal region
        //_numSR1, _numSR2, _numSR3;

        if(nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 250.)_num250++;
        if(nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 350.)_num350++;
        if(nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 400.)_num400++;
        if(nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 450.)_num450++;
        if(nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 500.)_num500++;
        if(nJets > 0 && baselineJets[0]->pT() > 110. && fabs(baselineJets[0]->eta()) < 2.4 && nJets <=2 && dPhiJ1J2 < 2.5 && nLeptons==0 && met > 550.)_num550++;

        return;

      }


      void add(BaseAnalysis* other) {
        // The base class add function handles the signal region vector and total # events.
        HEPUtilsAnalysis::add(other);

        Analysis_CMS_MONOJET_20invfb* specificOther
                = dynamic_cast<Analysis_CMS_MONOJET_20invfb*>(other);

        // Here we will add the subclass member variables:
        if (NCUTS != specificOther->NCUTS) NCUTS = specificOther->NCUTS;
        for (int j=0; j<NCUTS; j++) {
          cutFlowVector[j] += specificOther->cutFlowVector[j];
          cutFlowVector_str[j] = specificOther->cutFlowVector_str[j];
        }
        _num250 += specificOther->_num250;
        _num300 += specificOther->_num300;
        _num350 += specificOther->_num350;
        _num400 += specificOther->_num400;
        _num450 += specificOther->_num450;
        _num500 += specificOther->_num500;
        _num550 += specificOther->_num550;
      }


      void finalize() {

        using namespace std;

        //cout << "------------------------------------------------------------------------------------------------------------------------------ "<<std::endl;
        //cout << "CUT FLOW" <<std::endl;
        //cout << "------------------------------------------------------------------------------------------------------------------------------"<<std::endl;

        //std::cout<< right << setw(40) << "CUT" << setw(20) << "RAW" << setw(20) << endl;
        //for(int j=0; j<NCUTS; j++) {
	//std::cout << right << setw(40) << cutFlowVector_str[j].c_str() << setw(20) << cutFlowVector[j] << setw(20) << endl;
        //}
        //cout << "------------------------------------------------------------------------------------------------------------------------------ "<<std::endl;

        //cout << "RESULTS MONOJET " << _num250 << " " << _num300 << " " << _num350 << " " << _num400 << " " << _num450 << " " << _num500 << " " << _num550 << endl;

      }

      double loglikelihood() {
        /// @todo Implement!
        return 0;
      }

      void collect_results() {
        SignalRegionData results_250;
        results_250.set_observation(52200.);
        results_250.set_background(51800.);
        results_250.set_backgroundsys(2000.);
        results_250.set_signalsys(0.);
        results_250.set_signal(_num250);

        SignalRegionData results_300;
        results_300.set_observation(19800.);
        results_300.set_background(19600.);
        results_300.set_backgroundsys(830.);
        results_300.set_signalsys(0.);
        results_300.set_signal(_num300);

        SignalRegionData results_350;
        results_350.set_observation(8320.);
        results_350.set_background(8190.);
        results_350.set_backgroundsys(400.);
        results_350.set_signalsys(0.);
        results_350.set_signal(_num350);

        SignalRegionData results_400;
        results_400.set_observation(3830.);
        results_400.set_background(3930.);
        results_400.set_backgroundsys(230.);
        results_400.set_signalsys(0.);
        results_400.set_signal(_num400);

        SignalRegionData results_450;
        results_450.set_observation(1830.);
        results_450.set_background(2050.);
        results_450.set_backgroundsys(150.);
        results_450.set_signalsys(0.);
        results_450.set_signal(_num450);

        SignalRegionData results_500;
        results_500.set_observation(934.);
        results_500.set_background(1040.);
        results_500.set_backgroundsys(100.);
        results_500.set_signalsys(0.);
        results_500.set_signal(_num500);

        SignalRegionData results_550;
        results_550.set_observation(519.);
        results_550.set_background(509.);
        results_550.set_backgroundsys(66.);
        results_550.set_signalsys(0.);
        results_550.set_signal(_num550);

        add_result(results_250);
        add_result(results_300);
        add_result(results_350);
        add_result(results_400);
        add_result(results_450);
        add_result(results_500);
        add_result(results_550);

        return;
      }

    };

    DEFINE_ANALYSIS_FACTORY(CMS_MONOJET_20invfb)


  }
}
