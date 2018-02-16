///
///  \author Rose Kudzman-Blais
///  \date 2017 May
///
///  *********************************************

// Based on https://cds.cern.ch/record/2267406

#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <fstream>

#include "gambit/ColliderBit/analyses/BaseAnalysis.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
#include "gambit/ColliderBit/mt2_bisect.h"

using namespace std;

namespace Gambit {
  namespace ColliderBit {

    class Analysis_ATLAS_13TeV_MultiLEP_36invfb : public HEPUtilsAnalysis {
    private:

      // Numbers passing cuts
      double _numSR2_SF_loose, _numSR2_SF_tight, _numSR2_DF_100, _numSR2_DF_150, _numSR2_DF_200, _numSR2_DF_300, _numSR2_int, _numSR2_high, _numSR2_low, _numSR3_slep_a, _numSR3_slep_b, _numSR3_slep_c, _numSR3_slep_d, _numSR3_slep_e, _numSR3_WZ_0Ja, _numSR3_WZ_0Jb, _numSR3_WZ_0Jc, _numSR3_WZ_1Ja, _numSR3_WZ_1Jb, _numSR3_WZ_1Jc; 

      vector<int> cutFlowVector1;
      vector<string> cutFlowVector1_str;
      size_t NCUTS1;
      // vector<double> cutFlowVector1ATLAS_200_100;
      // double xsec1ATLAS_200_100; 

      vector<int> cutFlowVector2;
      vector<string> cutFlowVector2_str;
      size_t NCUTS2;
      // vector<double> cutFlowVector2ATLAS_400_200;
      // double xsec2ATLAS_400_200; 
      // vector<double> cutFlowVector2ATLAS_500_100;
      // double xsec2ATLAS_500_100; 

      vector<int> cutFlowVector3;
      vector<string> cutFlowVector3_str;
      size_t NCUTS3;
      // vector<double> cutFlowVector3ATLAS_200_100;
      // double xsec3ATLAS_200_100; 

      vector<int> cutFlowVector4;
      vector<string> cutFlowVector4_str;
      size_t NCUTS4;
      // vector<double> cutFlowVector4ATLAS_800_600;
      // double xsec4ATLAS_800_600; 

      vector<int> cutFlowVector5;
      vector<string> cutFlowVector5_str;
      size_t NCUTS5;
      // vector<double> cutFlowVector5ATLAS_401_1;
      // double xsec5ATLAS_401_1; 
      // vector<double> cutFlowVector5ATLAS_300_150;
      // double xsec5ATLAS_300_150; 

      // ofstream cutflowFile;
      // string analysisRunName;


    public:

      Analysis_ATLAS_13TeV_MultiLEP_36invfb() {

	_numSR2_SF_loose=0;
	_numSR2_SF_tight=0;
	_numSR2_DF_100=0;
	_numSR2_DF_150=0;
	_numSR2_DF_200=0;
	_numSR2_DF_300=0;
	_numSR2_int=0;
	_numSR2_high=0;
	_numSR2_low=0;
	_numSR3_slep_a=0;
	_numSR3_slep_b=0;
	_numSR3_slep_c=0;
	_numSR3_slep_d=0;
	_numSR3_slep_e=0;
	_numSR3_WZ_0Ja=0;
	_numSR3_WZ_0Jb=0;
	_numSR3_WZ_0Jc=0;
	_numSR3_WZ_1Ja=0;
	_numSR3_WZ_1Jb=0;
	_numSR3_WZ_1Jc=0;

	set_luminosity(36.1);

	NCUTS1=22;
	// xsec1ATLAS_200_100=1807.4;
        for (size_t i=0;i<NCUTS1;i++){
          cutFlowVector1.push_back(0);
          // cutFlowVector1ATLAS_200_100.push_back(0);
          cutFlowVector1_str.push_back("");
        }

	NCUTS2=14;
	// xsec2ATLAS_400_200=121.0269;
	// xsec2ATLAS_500_100=46.3576;
        for (size_t i=0;i<NCUTS2;i++){
          cutFlowVector2.push_back(0);
          // cutFlowVector2ATLAS_400_200.push_back(0);
          // cutFlowVector2ATLAS_500_100.push_back(0);
          cutFlowVector2_str.push_back("");
        }

	NCUTS3=24;
	// xsec3ATLAS_200_100=1807.4;
        for (size_t i=0;i<NCUTS3;i++){
          cutFlowVector3.push_back(0);
          // cutFlowVector3ATLAS_200_100.push_back(0);
          cutFlowVector3_str.push_back("");
        }

	NCUTS4=12;
	// xsec4ATLAS_800_600=3.803;
        for (size_t i=0;i<NCUTS4;i++){
          cutFlowVector4.push_back(0);
          // cutFlowVector4ATLAS_800_600.push_back(0);
          cutFlowVector4_str.push_back("");
        }

	NCUTS5=11;
	// xsec5ATLAS_401_1=5.43;
	// xsec5ATLAS_300_150=190.159;
        for (size_t i=0;i<NCUTS5;i++){
          cutFlowVector5.push_back(0);
          // cutFlowVector5ATLAS_401_1.push_back(0);
          // cutFlowVector5ATLAS_300_150.push_back(0);
          cutFlowVector5_str.push_back("");
        }

        // analysisRunName = "ATLAS_13TeV_MultiLEP_36invfb";
      }

      struct ptComparison {
        bool operator() (HEPUtils::Particle* i,HEPUtils::Particle* j) {return (i->pT()>j->pT());}
      } comparePt;
      
      struct ptJetComparison {
        bool operator() (HEPUtils::Jet* i,HEPUtils::Jet* j) {return (i->pT()>j->pT());}
      } compareJetPt;

      void analyze(const HEPUtils::Event* event) {
	HEPUtilsAnalysis::analyze(event);
        double met = event->met();

        // Baseline objects
        vector<HEPUtils::Particle*> baselineElectrons;
        for (HEPUtils::Particle* electron : event->electrons()) {
	  if (electron->pT()>10. && electron->abseta()<2.47)baselineElectrons.push_back(electron);
	}
	ATLAS::applyLooseIDElectronSelectionR2(baselineElectrons);

        vector<HEPUtils::Particle*> baselineMuons;
        for (HEPUtils::Particle* muon : event->muons()) {
	  if (muon->pT()>10. && muon->abseta()<2.4)baselineMuons.push_back(muon);
	}

        vector<HEPUtils::Jet*> baselineJets;
        for (HEPUtils::Jet* jet : event->jets()) {
          if (jet->pT()>20. && jet->abseta()<4.5)baselineJets.push_back(jet);
        }

	//Overlap Removal + Signal Objects	
	vector<HEPUtils::Particle*> signalElectrons;
	vector<HEPUtils::Particle*> signalMuons;
	vector<HEPUtils::Particle*> signalLeptons;
	vector<HEPUtils::Jet*> signalJets;
	vector<HEPUtils::Jet*> signalBJets;

        const vector<double> aBJet={0,10.};
        const vector<double> bBJet={0,30., 40., 50., 70., 80., 90., 100.,150., 200., 10000.};
        const vector<double> cBJet={0.63, 0.705, 0.745, 0.76, 0.775, 0.79,0.795, 0.805, 0.795, 0.76};
        HEPUtils::BinnedFn2D<double> _eff2d(aBJet,bBJet,cBJet);

	vector<HEPUtils::Jet*> overlapJet;
        for (size_t iJet=0;iJet<baselineJets.size();iJet++) {
	  vector<HEPUtils::Particle*> overlapEl;
          bool hasTag=has_tag(_eff2d, baselineJets.at(iJet)->eta(), baselineJets.at(iJet)->pT());
          for (size_t iEl=0;iEl<baselineElectrons.size();iEl++) {
            if (baselineElectrons.at(iEl)->mom().deltaR_eta(baselineJets.at(iJet)->mom())<0.2)overlapEl.push_back(baselineElectrons.at(iEl));
	  }
	  if (overlapEl.size()>0 && (baselineJets.at(iJet)->btag() && hasTag)) {
	    for (size_t iO=0;iO<overlapEl.size();iO++) {
	      baselineElectrons.erase(remove(baselineElectrons.begin(), baselineElectrons.end(), overlapEl.at(iO)), baselineElectrons.end());
	    }
	  }
	  if (overlapEl.size()>0 && !(baselineJets.at(iJet)->btag() && hasTag))overlapJet.push_back(baselineJets.at(iJet));
	}
	for (size_t iO=0;iO<overlapJet.size();iO++) {
	  baselineJets.erase(remove(baselineJets.begin(), baselineJets.end(), overlapJet.at(iO)), baselineJets.end());
	}

	for (size_t iEl=0;iEl<baselineElectrons.size();iEl++) {
	  bool overlap=false;
	  for (size_t iJet=0;iJet<baselineJets.size();iJet++) {
            if (baselineElectrons.at(iEl)->mom().deltaR_eta(baselineJets.at(iJet)->mom())<0.4)overlap=true;
	  }
	  if (!overlap)signalElectrons.push_back(baselineElectrons.at(iEl));
	}
	ATLAS::applyMediumIDElectronSelectionR2(signalElectrons);

	for (size_t iJet=0;iJet<baselineJets.size();iJet++) {
	  bool overlap=false;
	  for (size_t iMu=0;iMu<baselineMuons.size();iMu++) {
            if (baselineMuons.at(iMu)->mom().deltaR_eta(baselineJets.at(iJet)->mom())<0.2 && baselineMuons.at(iMu)->pT()>0.7*baselineJets.at(iJet)->pT())overlap=true;
	  }
	  if (!overlap) {
            bool hasTag=has_tag(_eff2d, baselineJets.at(iJet)->eta(), baselineJets.at(iJet)->pT());
	    signalJets.push_back(baselineJets.at(iJet));
	    if (baselineJets.at(iJet)->btag() && hasTag && baselineJets.at(iJet)->abseta()<2.4)signalBJets.push_back(baselineJets.at(iJet));
	  }
	}

	for (size_t iMu=0;iMu<baselineMuons.size();iMu++) {
	  bool overlap=false;
	  for (size_t iJet=0;iJet<signalJets.size();iJet++) {
            if (baselineMuons.at(iMu)->mom().deltaR_eta(signalJets.at(iJet)->mom())<0.4)overlap=true;
	  }
	  if (!overlap)signalMuons.push_back(baselineMuons.at(iMu));
	}

	signalLeptons=signalElectrons;
	signalLeptons.insert(signalLeptons.end(),signalMuons.begin(),signalMuons.end());
	sort(signalJets.begin(),signalJets.end(),compareJetPt);	
        sort(signalLeptons.begin(),signalLeptons.end(),comparePt);
	size_t nBaselineLeptons=baselineElectrons.size()+baselineMuons.size();
        size_t nSignalLeptons=signalLeptons.size();
        size_t nSignalJets=signalJets.size();
	size_t nSignalBJets=signalBJets.size();

	vector<vector<HEPUtils::Particle*>> SFOSpairs=getSFOSpair(signalLeptons);
	vector<vector<HEPUtils::Particle*>> OSpairs=getOSpair(signalLeptons);

	//Variables
	double pT_l0=0.;
	double pT_l1=0.;
	double pT_l2=0.;
	// double mlll=0.;
	double pTlll=999.;
	double mll=999.;
	double mT2=0;
	double deltaR_ll=999.;

	double pT_j0=0.;
	double pT_j1=0.;
	double pT_j2=0.;
	double mjj=0;
	double deltaR_jj=999.;

	HEPUtils::P4 Z;
	double deltaPhi_met_Z=999.;

	HEPUtils::P4 W;
	vector<HEPUtils::P4> W_ISR;
        double deltaPhi_met_W=0.;
	double deltaPhi_met_ISR=0.;
	double deltaPhi_met_jet0=0.;

	double mTmin=999;
	double mSFOS=999;

	bool central_jet_veto=true;
	bool bjet_veto=false;

	for (size_t iJet=0;iJet<nSignalJets;iJet++) {
	  if (signalJets.at(iJet)->pT()>60 && signalJets.at(iJet)->abseta()<2.4)central_jet_veto=false;
	}
	if (nSignalBJets==0)bjet_veto=true;

	if (nSignalLeptons>0)pT_l0=signalLeptons.at(0)->pT();
	if (nSignalLeptons>1) {
	  pT_l1=signalLeptons.at(1)->pT();	  
	  mll=(signalLeptons.at(0)->mom()+signalLeptons.at(1)->mom()).m();
	  deltaR_ll=signalLeptons.at(0)->mom().deltaR_eta(signalLeptons.at(1)->mom());

          double pLep1[3] = {signalLeptons.at(0)->mass(), signalLeptons.at(0)->mom().px(), signalLeptons.at(0)->mom().py()};
          double pLep2[3] = {signalLeptons.at(1)->mass(), signalLeptons.at(1)->mom().px(), signalLeptons.at(1)->mom().py()};
          double pMiss[3] = {0., event->missingmom().px(), event->missingmom().py() };
          double mn = 0.;

          mt2_bisect::mt2 mt2_calc;
          mt2_calc.set_momenta(pLep1,pLep2,pMiss);
          mt2_calc.set_mn(mn);
          mT2 = mt2_calc.get_mt2();

	  Z=signalLeptons.at(0)->mom()+signalLeptons.at(1)->mom();
	  deltaPhi_met_Z=Z.deltaPhi(event->missingmom());
	  for (size_t iPa=0;iPa<SFOSpairs.size();iPa++) {
	    for (size_t iLep=0;iLep<signalLeptons.size();iLep++) {
	      if (signalLeptons.at(iLep)!=SFOSpairs.at(iPa).at(0) && signalLeptons.at(iLep)!=SFOSpairs.at(iPa).at(1)) {
	        double mT=sqrt(2*signalLeptons.at(iLep)->pT()*met*(1-cos(signalLeptons.at(iLep)->mom().deltaPhi(event->missingmom()))));
		if (mT<mTmin) {
		  mTmin=mT;
		  mSFOS=(SFOSpairs.at(iPa).at(0)->mom()+SFOSpairs.at(iPa).at(1)->mom()).m();
	 	}
	      }
	    }
	  }
	}

	if (nSignalLeptons>2) {
	  pT_l2=signalLeptons.at(2)->pT();
	  // mlll=(signalLeptons.at(0)->mom()+signalLeptons.at(1)->mom()+signalLeptons.at(2)->mom()).m();
	  pTlll=(signalLeptons.at(0)->mom()+signalLeptons.at(1)->mom()+signalLeptons.at(2)->mom()).pT();
	}

	if (nSignalJets>0) {
	  pT_j0=signalJets.at(0)->pT();
	  deltaPhi_met_jet0=signalJets.at(0)->mom().deltaPhi(event->missingmom());
	}
	if (nSignalJets>1) {
	  pT_j1=signalJets.at(1)->pT();
	  if (nSignalJets<3 && bjet_veto) {
	    W=signalJets.at(0)->mom()+signalJets.at(1)->mom();
	    mjj=W.m();
	    deltaR_jj=signalJets.at(0)->mom().deltaR_eta(signalJets.at(1)->mom());
	    deltaPhi_met_W=W.deltaPhi(event->missingmom());
	  }
	  if (nSignalJets>2 && nSignalJets<6 && nSignalLeptons>1 && bjet_veto) {
	    W_ISR=get_W_ISR(signalJets,Z,event->missingmom());
	    W=W_ISR.at(0);
	    mjj=W.m();
	    deltaR_jj=W_ISR.at(3).deltaR_eta(W_ISR.at(2));
	    deltaPhi_met_W=W.deltaPhi(event->missingmom());
	    deltaPhi_met_ISR=W_ISR.at(1).deltaPhi(event->missingmom());
	  }
	}
	if (nSignalJets>2)pT_j2=signalJets.at(2)->pT();

        bool preselection=false; 
	if ((nSignalLeptons==2 || nSignalLeptons==3) && nBaselineLeptons==nSignalLeptons && pT_l0>25 && pT_l1>20)preselection=true;

	//Signal Regions
	//2lep+0jet
	if (preselection && nSignalLeptons==2 && OSpairs.size()==1 && mll>40 && central_jet_veto && bjet_veto) {
	  if (SFOSpairs.size()==1) {
	    if (mT2>100 && mll>111)_numSR2_SF_loose++;
	    if (mT2>130 && mll>300)_numSR2_SF_tight++; 
	  }
	  if (SFOSpairs.size()==0) {
	    if (mT2>100)_numSR2_DF_100++;
	    if (mT2>150)_numSR2_DF_150++;
	    if (mT2>200)_numSR2_DF_200++;
	    if (mT2>300)_numSR2_DF_300++;
	  }
	}
	
	//2lep+jets
	if (preselection && nSignalLeptons==2 && SFOSpairs.size()==1 && bjet_veto && nSignalJets>1 && pT_j0>30 && pT_j1>30 && pT_l1>25) {
	  //SR2_int + SR2_high
	  if (mll>81. && mll<101. && mjj>70. && mjj<100. && Z.pT()>80. && W.pT()>100. && mT2>100. && deltaR_jj<1.5 && deltaR_ll<1.8 && deltaPhi_met_W>0.5 && deltaPhi_met_W<3.0) {
            if (met>150)_numSR2_int++;
            if (met>250)_numSR2_high++;
	  }
	  //SR2_low_2J
	  if (nSignalJets==2 && mll>81. && mll<101. && mjj>70. && mjj<90. && met>100. && Z.pT()>60. && deltaPhi_met_Z<0.8 && deltaPhi_met_W>1.5 && (met/Z.pT())>0.6 && (met/Z.pT())<1.6 && (met/W.pT())<0.8)_numSR2_low++;
	  //SR2_low_3J
	  if (nSignalJets>2 && nSignalJets<6 && mll>86 && mll<96 && mjj>70. && mjj<90. && met>100 && Z.pT()>40 && deltaR_jj<2.2 && deltaPhi_met_W<2.2 && deltaPhi_met_ISR>2.4 && deltaPhi_met_jet0>2.6 && (met/W_ISR.at(1).pT())>0.4 && (met/W_ISR.at(1).pT())<0.8 && Z.abseta()<1.6 && pT_j2>30.)_numSR2_low++;
	}
	
	//3lep
	if (preselection && nSignalLeptons==3 && bjet_veto && SFOSpairs.size()) {
	  if (mSFOS<81.2 && met>130. && mTmin>110.) {
	    if (pT_l2>20. && pT_l2<30.)_numSR3_slep_a++;
	    if (pT_l2>30.)_numSR3_slep_b++;
	  }
	  if (mSFOS>101.2 && met>130. && mTmin>110.) {
	    if (pT_l2>20. && pT_l2<50.)_numSR3_slep_c++;
	    if (pT_l2>50. && pT_l2<80.)_numSR3_slep_d++;
	    if (pT_l2>80.)_numSR3_slep_e++;
	  }
	  if (mSFOS>81.2 && mSFOS<101.2 && nSignalJets==0 && mTmin>110.) {
	    if (met>60. && met<120.)_numSR3_WZ_0Ja++;
	    if (met>120. && met<170.)_numSR3_WZ_0Jb++;
	    if (met>170.)_numSR3_WZ_0Jc++;
	  }
	  if (mSFOS>81.2 && mSFOS<101.2 && nSignalJets>0) {
	    if (met>120. && met<200. && mTmin>110. && pTlll<120. && pT_j1>70.)_numSR3_WZ_1Ja++;
	    if (met>200. && mTmin>110. && mTmin<160.)_numSR3_WZ_1Jb++;
	    if (met>200. && pT_l2>35. && mTmin>160.)_numSR3_WZ_1Jc++;
	  }
	}

	// if (analysisRunName.find("200_100") != string::npos) {

 //  	  cutFlowVector1_str[0] = "All events";
 //          cutFlowVector1_str[1] = "$\\geq$ 2 signal leptons \\& SFOS";
 //          cutFlowVector1_str[2] = "3 signal leptons \\& extra lepton veto";
 //          cutFlowVector1_str[3] = "B-jet veto";
 //          cutFlowVector1_str[4] = "$p_{T}^{l0} > 25 GeV$";
 //          cutFlowVector1_str[5] = "$p_{T}^{l2} > 20 GeV$";
 //          cutFlowVector1_str[6] = "$m_{lll} > 20 GeV$";
 //          cutFlowVector1_str[7] = "$| m_{ll} - m_{Z} | < 10 GeV$";
 //          cutFlowVector1_str[8] = "0 jets";
 //          cutFlowVector1_str[9] = "$60 < E^{miss}_{T} < 120 GeV$";
 //          cutFlowVector1_str[10] = "$m_{T}^{min} > 110 GeV$";
 //          cutFlowVector1_str[11] = "$120 < E^{miss}_{T} < 170 GeV$";
 //          cutFlowVector1_str[12] = "$m_{T}^{min} > 110 GeV$";
 //          cutFlowVector1_str[13] = "$E^{miss}_{T} > 170 GeV$";
 //          cutFlowVector1_str[14] = "$m_{T}^{min} > 110 GeV$";
 //          cutFlowVector1_str[15] = "$\\geq$ 1 jet";
 //          cutFlowVector1_str[16] = "$120 < E^{miss}_{T} < 200 GeV$";
 //          cutFlowVector1_str[17] = "$m_{T}^{min} > 110 GeV$";
 //          cutFlowVector1_str[18] = "$p_{T}^{lll} < 120 GeV$";
 //          cutFlowVector1_str[19] = "$p_{T}^{j0} > 70 GeV$";
 //          cutFlowVector1_str[20] = "$E^{miss}_{T} > 200 GeV$";
 //          cutFlowVector1_str[21] = "$110 < m_{T}^{min} < 160 GeV$";

 //          cutFlowVector1ATLAS_200_100[0]=17682.;
	//   cutFlowVector1ATLAS_200_100[1]=425.63;
 // 	  cutFlowVector1ATLAS_200_100[2]=424.59;
	//   cutFlowVector1ATLAS_200_100[3]=414.43;
	//   cutFlowVector1ATLAS_200_100[4]=413.98;
	//   cutFlowVector1ATLAS_200_100[5]=306.91;
	//   cutFlowVector1ATLAS_200_100[6]=301.70;
	//   cutFlowVector1ATLAS_200_100[7]=227.15;
	//   cutFlowVector1ATLAS_200_100[8]=110.35;
	//   cutFlowVector1ATLAS_200_100[9]=43.24;
	//   cutFlowVector1ATLAS_200_100[10]=8.91;
	//   cutFlowVector1ATLAS_200_100[11]=6.02;
	//   cutFlowVector1ATLAS_200_100[12]=1.1;
	//   cutFlowVector1ATLAS_200_100[13]=3.15;
	//   cutFlowVector1ATLAS_200_100[14]=0.49;
	//   cutFlowVector1ATLAS_200_100[15]=116.81;
	//   cutFlowVector1ATLAS_200_100[16]=18.86;
	//   cutFlowVector1ATLAS_200_100[17]=5.8;
	//   cutFlowVector1ATLAS_200_100[18]=4.63;
	//   cutFlowVector1ATLAS_200_100[19]=3.18;
	//   cutFlowVector1ATLAS_200_100[20]=7.32;
	//   cutFlowVector1ATLAS_200_100[21]=1.85;

   //        for (size_t j=0;j<NCUTS1;j++){
   //          if(
   //            (j==0) ||

	  //     (j==1 && nSignalLeptons>=2 && SFOSpairs.size()>0) ||   

	  //     (j==2 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3) ||   

	  //     (j==3 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto) ||   

	  //     (j==4 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25.) ||   

	  //     (j==5 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20.) ||   

	  //     (j==6 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20.) ||   

	  //     (j==7 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10.) ||   

	  //     (j==8 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets==0) ||   

	  //     (j==9 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && met>60. && met<120. && nSignalJets==0) ||   

	  //     (j==10 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && met>60. && met<120.&& nSignalJets==0 && mTmin>110.) ||   
 
	  //     (j==11 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets==0 && met>120. && met<170.) ||   

	  //     (j==12 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets==0 && met>120. && met<170. && mTmin>110.) ||   

	  //     (j==13 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets==0 && met>170.) ||   

	  //     (j==14 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets==0 && met>170. && mTmin>110.) ||   

	  //     (j==15 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets>0) ||   

	  //     (j==16 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets>0 && met>120. && met<200.) ||   

	  //     (j==17 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets>0 && met>120. && met<200. && mTmin>110.) ||   

	  //     (j==18 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets>0 && met>120. && met<200. && mTmin>110. && pTlll<120.) ||   

	  //     (j==19 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets>0 && met>120. && met<200. && mTmin>110. && pTlll<120. && pT_j0>70.) ||   

	  //     (j==20 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets>0 && met>200.) ||   

	  //     (j==21 && nSignalLeptons>=2 && SFOSpairs.size()>0 && nSignalLeptons==3 && nBaselineLeptons==3 && bjet_veto && pT_l0>25. && pT_l2>20. && mlll>20. && fabs(mSFOS-91.2)<10. && nSignalJets>0 && met>200. && mTmin>110. && mTmin<160.) )   

	  //     cutFlowVector1[j]++;
	  // }

  	  // cutFlowVector3_str[0] = "All events";
  	  // cutFlowVector3_str[1] = "2 signal leptons \\& SFOS";
  	  // cutFlowVector3_str[2] = "B-jet veto";
  	  // cutFlowVector3_str[3] = "$E_{T}^{miss} > 100 GeV$";
  	  // cutFlowVector3_str[4] = "2 signal jets";
  	  // cutFlowVector3_str[5] = "$p_{T}^{j0}, p_{T}^{j1} > 30 GeV$";
  	  // cutFlowVector3_str[6] = "$81 < m_{Z} < 101 GeV$";
  	  // cutFlowVector3_str[7] = "$70 < m_{W} < 90 GeV$";
  	  // cutFlowVector3_str[8] = "$p_{T}^{Z} > 60 GeV$";
  	  // cutFlowVector3_str[9] = "$\\Delta\\phi(E_{T}^{miss},Z) < 0.8$";
  	  // cutFlowVector3_str[10] = "$\\Delta\\phi(E_{T}^{miss},W) > 1.5$";
  	  // cutFlowVector3_str[11] = "$E_{T}^{miss}/p_{T}^{W} < 0.8$";
  	  // cutFlowVector3_str[12] = "$0.6 < E_{T}^{miss}/p_{T}^{Z} < 1.6$";
  	  // cutFlowVector3_str[13] = "3-5 signal jets";
  	  // cutFlowVector3_str[14] = "$p_{T}^{j0}, p_{T}^{j1}, p_{T}^{j2} > 30 GeV$";
  	  // cutFlowVector3_str[15] = "$81 < m_{Z} < 101 GeV$";
  	  // cutFlowVector3_str[16] = "$70 < m_{W} < 90 GeV$";
  	  // cutFlowVector3_str[17] = "$||\\eta (Z)|| < 1.6$";
  	  // cutFlowVector3_str[18] = "$p_{T}^{Z} > 40 GeV$";
  	  // cutFlowVector3_str[19] = "$\\Delta\\phi (E_{T}^{miss},ISR) > 2.4$";
  	  // cutFlowVector3_str[20] = "$\\Delta\\phi (E_{T}^{miss},j1) > 2.6$";
  	  // cutFlowVector3_str[21] = "$\\Delta\\phi (E_{T}^{miss},W) < 2.2$ ";
  	  // cutFlowVector3_str[22] = "$0.4 < E_{T}^{miss}/ISR < 0.8$";
  	  // cutFlowVector3_str[23] = "$\\Delta R(W\\rightarrow 2j) < 2.2$";

     //      cutFlowVector3ATLAS_200_100[0]=20000.;
     //      cutFlowVector3ATLAS_200_100[1]=957.;
     //      cutFlowVector3ATLAS_200_100[2]=880.6;
     //      cutFlowVector3ATLAS_200_100[3]=120.8;
     //      cutFlowVector3ATLAS_200_100[4]=30.2;
     //      cutFlowVector3ATLAS_200_100[5]=20.6;
     //      cutFlowVector3ATLAS_200_100[6]=18.8;
     //      cutFlowVector3ATLAS_200_100[7]=6.2;
     //      cutFlowVector3ATLAS_200_100[8]=5.1;
     //      cutFlowVector3ATLAS_200_100[9]=2.7;
     //      cutFlowVector3ATLAS_200_100[10]=2.7;
     //      cutFlowVector3ATLAS_200_100[11]=2.6;
     //      cutFlowVector3ATLAS_200_100[12]=2.2;
     //      cutFlowVector3ATLAS_200_100[13]=71.7;
     //      cutFlowVector3ATLAS_200_100[14]=47.9;
     //      cutFlowVector3ATLAS_200_100[15]=37.1;
     //      cutFlowVector3ATLAS_200_100[16]=9.3;
     //      cutFlowVector3ATLAS_200_100[17]=7.1;
     //      cutFlowVector3ATLAS_200_100[18]=6.9;
     //      cutFlowVector3ATLAS_200_100[19]=6.3;
     //      cutFlowVector3ATLAS_200_100[20]=5.3;
     //      cutFlowVector3ATLAS_200_100[21]=4.8;
     //      cutFlowVector3ATLAS_200_100[22]=4.0;
     //      cutFlowVector3ATLAS_200_100[23]=3.6;

 //          for (size_t j=0;j<NCUTS3;j++){
 //            if(
 //              (j==0) ||

	//       (j==1 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0) || 

	//       (j==2 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto) || 

	//       (j==3 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100.) || 

	//       (j==4 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2) || 

	//       (j==5 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30.) || 

	//       (j==6 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30. && mll>81. && mll<101.) || 

	//       (j==7 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30. && mll>81. && mll<101. && mjj>70. && mjj<90.) || 

	//       (j==8 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.pT()>60.) || 

	//       (j==9 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.pT()>60. && deltaPhi_met_Z<0.8) || 

	//       (j==10 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.pT()>60. && deltaPhi_met_Z<0.8 && deltaPhi_met_W>1.5) || 

	//       (j==11 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.pT()>60. && deltaPhi_met_Z<0.8 && deltaPhi_met_W>1.5 && met/W.pT()<0.8) || 

	//       (j==12 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets==2 && pT_j0>30. && pT_j1>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.pT()>60. && deltaPhi_met_Z<0.8 && deltaPhi_met_W>1.5 && met/W.pT()<0.8 && met/Z.pT()>0.6 && met/Z.pT()<1.6) || 

	//       (j==13 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6) || 

	//       (j==14 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30.) || 

	//       (j==15 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101.) || 

	//       (j==16 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90.) || 

	//       (j==17 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.abseta()<1.6) || 

	//       (j==18 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.abseta()<1.6 && Z.pT()>40.) || 

	//       (j==19 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.abseta()<1.6 && Z.pT()>40. && deltaPhi_met_ISR>2.4) || 

	//       (j==20 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.abseta()<1.6 && Z.pT()>40. && deltaPhi_met_ISR>2.4 && deltaPhi_met_jet0>2.6) || 

	//       (j==21 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.abseta()<1.6 && Z.pT()>40. && deltaPhi_met_ISR>2.4 && deltaPhi_met_jet0>2.6 && deltaPhi_met_W<2.2) || 

	//       (j==22 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.abseta()<1.6 && Z.pT()>40. && deltaPhi_met_ISR>2.4 && deltaPhi_met_jet0>2.6 && deltaPhi_met_W<2.2 && met/W_ISR.at(1).pT()>0.4 && met/W_ISR.at(1).pT()<0.8) || 

	//       (j==23 && preselection && pT_l1>25. && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && met>100. && nSignalJets>2 && nSignalJets<6 && pT_j0>30. && pT_j1>30. && pT_j2>30. && mll>81. && mll<101. && mjj>70. && mjj<90. && Z.abseta()<1.6 && Z.pT()>40. && deltaPhi_met_ISR>2.4 && deltaPhi_met_jet0>2.6 && deltaPhi_met_W<2.2 && met/W_ISR.at(1).pT()>0.4 && met/W_ISR.at(1).pT()<0.8 && deltaR_jj<2.2) ) 

	//       cutFlowVector3[j]++;
	//   }

	// }

	// if ((analysisRunName.find("400_200") != string::npos) || (analysisRunName.find("500_100") != string::npos)){

 //  	  cutFlowVector2_str[0] = "All events";
 //  	  cutFlowVector2_str[1] = "2 signal leptons \\& SFOS";
 //  	  cutFlowVector2_str[2] = "B-jet veto";
 //  	  cutFlowVector2_str[3] = "$\\geq$ 2 signal jets";
 //  	  cutFlowVector2_str[4] = "$p_{T}^{j0}, p_{T}^{j1} > 30 GeV$";
 //  	  cutFlowVector2_str[5] = "$E_{T}^{miss} > 150 GeV$";
 //  	  cutFlowVector2_str[6] = "$p_{T}^{Z} > 80 GeV$";
 //  	  cutFlowVector2_str[7] = "$p_{T}^{W} > 100 GeV$";
 //  	  cutFlowVector2_str[8] = "$ 81 < m_{Z} < 101 GeV$";
 //  	  cutFlowVector2_str[9] = "$70 < m_{W} < 100 GeV$";
 //  	  cutFlowVector2_str[10] = "$m_{T2} > 100 GeV$";
 //  	  cutFlowVector2_str[11] = "$0.5 < \\Delta\\phi(E_{T}^{miss}, W) < 3.0$";
 //  	  cutFlowVector2_str[12] = "$\\Delta R(W\\rightarrow jj) <1.5$";
 //  	  cutFlowVector2_str[13] = "$\\Delta R(Z\\rightarrow ll) <1.8$";

 //          cutFlowVector2ATLAS_400_200[0]=10000.;
 //          cutFlowVector2ATLAS_400_200[1]=83.1;
 //          cutFlowVector2ATLAS_400_200[2]=75.8;
 //          cutFlowVector2ATLAS_400_200[3]=64.7;
 //          cutFlowVector2ATLAS_400_200[4]=53.3;
 //          cutFlowVector2ATLAS_400_200[5]=29.8;
 //          cutFlowVector2ATLAS_400_200[6]=25.0;
 //          cutFlowVector2ATLAS_400_200[7]=20.3;
 //          cutFlowVector2ATLAS_400_200[8]=18.4;
 //          cutFlowVector2ATLAS_400_200[9]=7.7;
 //          cutFlowVector2ATLAS_400_200[10]=5.8;
 //          cutFlowVector2ATLAS_400_200[11]=5.5;
 //          cutFlowVector2ATLAS_400_200[12]=5.4;
 //          cutFlowVector2ATLAS_400_200[13]=5.2;

 //          cutFlowVector2ATLAS_500_100[0]=5000.;
 //          cutFlowVector2ATLAS_500_100[1]=37.9;
 //          cutFlowVector2ATLAS_500_100[2]=33.7;
 //          cutFlowVector2ATLAS_500_100[3]=28.9;
 //          cutFlowVector2ATLAS_500_100[4]=25.3;
 //          cutFlowVector2ATLAS_500_100[5]=20.5;
 //          cutFlowVector2ATLAS_500_100[6]=19.4;
 //          cutFlowVector2ATLAS_500_100[7]=17.5;
 //          cutFlowVector2ATLAS_500_100[8]=15.6;
 //          cutFlowVector2ATLAS_500_100[9]=7.4;
 //          cutFlowVector2ATLAS_500_100[10]=6.7;
 //          cutFlowVector2ATLAS_500_100[11]=5.9;
 //          cutFlowVector2ATLAS_500_100[12]=5.9;
 //          cutFlowVector2ATLAS_500_100[13]=5.9;
	// //maybe add pt_l1>25.
 //          for (size_t j=0;j<NCUTS2;j++){
 //            if(
 //              (j==0) ||

	//       (j==1 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0) ||   

	//       (j==2 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto) ||   

	//       (j==3 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2) ||   

	//       (j==4 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30.) ||   

	//       (j==5 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150.) ||   

	//       (j==6 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80.) ||   

	//       (j==7 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80. && W.pT()>100.) ||   

	//       (j==8 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80. && W.pT()>100. && mll>81. && mll<101.) ||   

	//       (j==9 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80. && W.pT()>100. && mll>81. && mll<101. && mjj>70. && mjj<100.) ||   

	//       (j==10 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80. && W.pT()>100. && mll>81. && mll<101. && mjj>70. && mjj<100. && mT2>100.) ||   

	//       (j==11 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80. && W.pT()>100. && mll>81. && mll<101. && mjj>70. && mjj<100. && mT2>100. && deltaPhi_met_W>0.5 && deltaPhi_met_W<3.0) ||   

	//       (j==12 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80. && W.pT()>100. && mll>81. && mll<101. && mjj>70. && mjj<100. && mT2>100. && deltaPhi_met_W>0.5 && deltaPhi_met_W<3.0 && deltaR_jj<1.5) ||   

	//       (j==13 && preselection && nSignalLeptons==2 && SFOSpairs.size()>0 && bjet_veto && nSignalJets>=2 && pT_j0>30. && pT_j1>30. && met>150. && Z.pT()>80. && W.pT()>100. && mll>81. && mll<101. && mjj>70. && mjj<100. && mT2>100. && deltaPhi_met_W>0.5 && deltaPhi_met_W<3.0 && deltaR_jj<1.5 && deltaR_ll<1.8) )   

	//       cutFlowVector2[j]++;
	//   }
	// }

	// if (analysisRunName.find("800_600") != string::npos){

 //  	  cutFlowVector4_str[0] = "All events";
 //  	  cutFlowVector4_str[1] = "3 signal leptons \\& SFOS";
 //  	  cutFlowVector4_str[2] = "Pass event cleaning";
 //  	  cutFlowVector4_str[3] = "$m_{T}^{min} > 110 GeV$";
 //  	  cutFlowVector4_str[4] = "$E_{T}^{miss} > 130 GeV$";
 //  	  cutFlowVector4_str[5] = "$m^{min}_{SFOS} < 81.2 GeV$";
 //  	  cutFlowVector4_str[6] = "$20 < p_{T}^{l2} < 30 GeV$";
 //  	  cutFlowVector4_str[7] = "$p_{T}^{l2} > 30 GeV$";
 //  	  cutFlowVector4_str[8] = "$m^{min}_{SFOS} > 101.2 GeV$";
 //  	  cutFlowVector4_str[9] = "$20 < p_{T}^{l2} < 50 GeV$";
 //  	  cutFlowVector4_str[10] = "$50 < p_{T}^{l2} < 80 GeV$";
 //  	  cutFlowVector4_str[11] = "$p_{T}^{l2} > 80 GeV$";

 //          // cutFlowVector4ATLAS_800_600[0]=9291.;
 //          // cutFlowVector4ATLAS_800_600[1]=25.13;
 //          // cutFlowVector4ATLAS_800_600[2]=23.54;
 //          // cutFlowVector4ATLAS_800_600[3]=14.43;
 //          // cutFlowVector4ATLAS_800_600[4]=10.22;
 //          // cutFlowVector4ATLAS_800_600[5]=2.10;
 //          // cutFlowVector4ATLAS_800_600[6]=0.11;
 //          // cutFlowVector4ATLAS_800_600[7]=1.99;
 //          // cutFlowVector4ATLAS_800_600[8]=6.8;
 //          // cutFlowVector4ATLAS_800_600[9]=2.53;
 //          // cutFlowVector4ATLAS_800_600[10]=3.01;
 //          // cutFlowVector4ATLAS_800_600[11]=1.25;

 //          for (size_t j=0;j<NCUTS4;j++){
 //            if(
 //              (j==0) ||

	//       (j==1 && nSignalLeptons==3 && SFOSpairs.size()>0) ||   

	//       (j==2 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto) ||   

	//       (j==3 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110.) ||   

	//       (j==4 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130.) ||   

	//       (j==5 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130. && mSFOS<81.2) ||   

	//       (j==6 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130. && mSFOS<81.2 && pT_l2>20. && pT_l2<30.) ||   

	//       (j==7 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130. && mSFOS<81.2 && pT_l2>30.) ||   

	//       (j==8 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130. && mSFOS>101.2) ||   

	//       (j==9 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130. && mSFOS>101.2 && pT_l2>20. && pT_l2<50.) ||   

	//       (j==10 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130. && mSFOS>101.2 && pT_l2>50. && pT_l2<80.) ||   

	//       (j==11 && nSignalLeptons==3 && SFOSpairs.size()>0 && preselection && bjet_veto && mTmin>110. && met>130. && mSFOS>101.2 && pT_l2>80.) ) 

	//       cutFlowVector4[j]++;
	//   }
	// }

	// if ((analysisRunName.find("401_1") != string::npos) || (analysisRunName.find("300_150") != string::npos)){

 //  	  cutFlowVector5_str[0] = "All events";
 //  	  cutFlowVector5_str[1] = "2 signal leptons \\& OS";
 //  	  cutFlowVector5_str[2] = "$p_{T}^{l0} > 25 GeV$";
 //  	  cutFlowVector5_str[3] = "Jet veto";
 //  	  cutFlowVector5_str[4] = "$m_{ll} > 40 GeV$";
 //  	  cutFlowVector5_str[5] = "Same flavour";
 //  	  cutFlowVector5_str[6] = "$m_{ll} > 111 GeV$";
 //  	  cutFlowVector5_str[7] = "$m_{T2} > 100 GeV$";
 //  	  cutFlowVector5_str[8] = "Different flavour";
 //  	  cutFlowVector5_str[9] = "$m_{ll} > 111 GeV$";
 //  	  cutFlowVector5_str[10] = "$m_{T2} > 100 GeV$";

 //          cutFlowVector5ATLAS_401_1[0]=10000.;
 //          cutFlowVector5ATLAS_401_1[1]=89.7;
 //          cutFlowVector5ATLAS_401_1[2]=89.7;
 //          cutFlowVector5ATLAS_401_1[3]=89.5;
 //          cutFlowVector5ATLAS_401_1[4]=55.7;
 //          cutFlowVector5ATLAS_401_1[5]=55.7;
 //          cutFlowVector5ATLAS_401_1[6]=53.7;
 //          cutFlowVector5ATLAS_401_1[7]=40.4;

 //          cutFlowVector5ATLAS_300_150[0]=25000;
 //          cutFlowVector5ATLAS_300_150[1]=1797.;
 //          cutFlowVector5ATLAS_300_150[2]=1795.3;
 //          cutFlowVector5ATLAS_300_150[3]=1692.1;
 //          cutFlowVector5ATLAS_300_150[4]=1262.;
 //          cutFlowVector5ATLAS_300_150[5]=667.4;
 //          cutFlowVector5ATLAS_300_150[6]=405.;
 //          cutFlowVector5ATLAS_300_150[7]=46.9;
 //          cutFlowVector5ATLAS_300_150[8]=594.5;
 //          cutFlowVector5ATLAS_300_150[9]=363.8;
 //          cutFlowVector5ATLAS_300_150[10]=45.7;

 //          for (size_t j=0;j<NCUTS5;j++){
 //            if(
 //              (j==0) ||

	//       (j==1 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0) ||   

	//       (j==2 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25.) ||   

	//       (j==3 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto) ||   

	//       (j==4 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto && mll>40.) ||   

	//       (j==5 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto && mll>40. && SFOSpairs.size()>0) ||   

	//       (j==6 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto && mll>40. && SFOSpairs.size()>0 && mll>111.) ||   

	//       (j==7 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto && mll>40. && SFOSpairs.size()>0 && mll>111. && mT2>100.) ||   

	//       (j==8 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto && mll>40. && SFOSpairs.size()==0) ||   

	//       (j==9 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto && mll>40. && SFOSpairs.size()==0 && mll>111.) ||   

	//       (j==10 && nBaselineLeptons==nSignalLeptons && nSignalLeptons==2 && OSpairs.size()>0 && pT_l0>25. && central_jet_veto && mll>40. && SFOSpairs.size()==0 && mll>111. && mT2>100.) )   

	//       cutFlowVector5[j]++;
	//   }
	// }

      }


      void add(BaseAnalysis* other) {
        // The base class add function handles the signal region vector and total # events.
        
	HEPUtilsAnalysis::add(other);

        Analysis_ATLAS_13TeV_MultiLEP_36invfb* specificOther
                = dynamic_cast<Analysis_ATLAS_13TeV_MultiLEP_36invfb*>(other);

        // Here we will add the subclass member variables:
        if (NCUTS1 != specificOther->NCUTS1) NCUTS1 = specificOther->NCUTS1;
        for (size_t j = 0; j < NCUTS1; j++) {
          cutFlowVector1[j] += specificOther->cutFlowVector1[j];
          cutFlowVector1_str[j] = specificOther->cutFlowVector1_str[j];
        }
        if (NCUTS2 != specificOther->NCUTS2) NCUTS2 = specificOther->NCUTS2;
        for (size_t j = 0; j < NCUTS2; j++) {
          cutFlowVector2[j] += specificOther->cutFlowVector2[j];
          cutFlowVector2_str[j] = specificOther->cutFlowVector2_str[j];
        }

	_numSR2_SF_loose+= specificOther->_numSR2_SF_loose;
	_numSR2_SF_tight+= specificOther->_numSR2_SF_tight;
	_numSR2_DF_100+= specificOther->_numSR2_DF_100;
	_numSR2_DF_150+= specificOther->_numSR2_DF_150;
	_numSR2_DF_200+= specificOther->_numSR2_DF_200;
	_numSR2_DF_300+= specificOther->_numSR2_DF_300;
	_numSR2_int+= specificOther->_numSR2_int;
	_numSR2_high+= specificOther->_numSR2_high;
	_numSR2_low+= specificOther->_numSR2_low;
	_numSR3_slep_a+= specificOther->_numSR3_slep_a;
	_numSR3_slep_b+= specificOther->_numSR3_slep_b;
	_numSR3_slep_c+= specificOther->_numSR3_slep_c;
	_numSR3_slep_d+= specificOther->_numSR3_slep_d;
	_numSR3_slep_e+= specificOther->_numSR3_slep_e;
	_numSR3_WZ_0Ja+= specificOther->_numSR3_WZ_0Ja;
	_numSR3_WZ_0Jb+= specificOther->_numSR3_WZ_0Jb;
	_numSR3_WZ_0Jc+= specificOther->_numSR3_WZ_0Jc;
	_numSR3_WZ_1Ja+= specificOther->_numSR3_WZ_1Ja;
	_numSR3_WZ_1Jb+= specificOther->_numSR3_WZ_1Jb;
	_numSR3_WZ_1Jc+= specificOther->_numSR3_WZ_1Jc;
      }


      void collect_results() {

        // string path = "ColliderBit/results/cutflow_";
        // path.append(analysisRunName);
        // path.append(".txt");
        // cutflowFile.open(path.c_str());

	// if (analysisRunName.find("200_100") != string::npos) {
	//   cutflowFile<<"\\begin{table}[H] \n\\caption{$\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0}$ decay via $W/Z$ (on-shell), $[\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0}, \\tilde{\\chi}_{1}^{0}]: [200,100] [GeV]$} \n\\makebox[\\linewidth]{ \n\\resizebox{\\textwidth}{!}{ \n\\renewcommand{\\arraystretch}{0.5} \n\\begin{tabular}{c c c c c c} \n\\hline"<<endl;
 //          cutflowFile<<"& ATLAS & GAMBIT & GAMBIT/ATLAS & $\\sigma$-corrected GAMBIT/ATLAS \\\\ \\hline"<<endl;
 //          cutflowFile<<"& $\\sigma (pp\\to \\tilde{\\chi}_{1}^{\\pm}, \\tilde{\\chi}_{2}^{0})$ &"<<setprecision(4)<<xsec1ATLAS_200_100<<" $fb$ &"<<setprecision(4)<<xsec()<<"$fb$ &"<<setprecision(4)<< xsec()/xsec1ATLAS_200_100<<" & 1\\\\"<<endl;
 //          cutflowFile<<" & Generated Events &"<< cutFlowVector1ATLAS_200_100[0]<<"&"<<cutFlowVector1[0]<<"& - & -\\\\ \\hline"<<endl;

    //       cutflowFile<<"\\multicolumn{6}{c}{Expected events at 36.1 $fb^{-1}$} \\\\ \\hline"<<endl;
    //       for (size_t i=1; i<NCUTS1; i++) {
	   //  if (i==1)cutflowFile<<"\\multirow{7}{*}{Event selection} &"<<cutFlowVector1_str[i]<<"&";
	   //  else if (i==8 || i==15)cutflowFile<<"\\multirow{7}{*}{"<<cutFlowVector1_str[i]<<"}&-&";
	   //  else cutflowFile<<"& "<<cutFlowVector1_str[i]<<" & ";
    //         cutflowFile<<setprecision(4)<<cutFlowVector1ATLAS_200_100[i]<<"&"<<setprecision(4)<<cutFlowVector1[i]*xsec_per_event()*luminosity()<<"&"<<setprecision(4)<<cutFlowVector1[i]*xsec_per_event()*luminosity()/cutFlowVector1ATLAS_200_100[i]<<"&"<<setprecision(4)<<(xsec1ATLAS_200_100/xsec())*cutFlowVector1[i]*xsec_per_event()*luminosity()/cutFlowVector1ATLAS_200_100[i]<<"\\\\";
	   // if (i==7 || i==14)cutflowFile<<"\\hline\\hline"<<endl;
	   // else if (i==8 || i==10 || i==12 || i==15 || i==19)cutflowFile<<"\\cline{2-6}"<<endl;
	   // else cutflowFile<<endl;
    //       }

 //          cutflowFile<<"\\hline \\multicolumn{6}{c}{Percentage (\\%)} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS1; i++) {
	//     if (i==1)cutflowFile<<"\\multirow{7}{*}{Event selection} &"<<cutFlowVector1_str[i]<<"&";
	//     else if (i==8 || i==15)cutflowFile<<"\\multirow{7}{*}{"<<cutFlowVector1_str[i]<<"}&-&";
	//     else cutflowFile<<"& "<<cutFlowVector1_str[i]<<" & ";
 //            cutflowFile<<setprecision(4)<<cutFlowVector1ATLAS_200_100[i]*100./cutFlowVector1ATLAS_200_100[2]<<"&"<<setprecision(4)<<cutFlowVector1[i]*100./cutFlowVector1[2]<<"& - & -\\\\";
	//    if (i==7 || i==14)cutflowFile<<"\\hline\\hline"<<endl;
	//    else if (i==8 || i==10 || i==12 || i==15 || i==19)cutflowFile<<"\\cline{2-6}"<<endl;
	//    else cutflowFile<<endl;
 //          }
 //          cutflowFile<<"\\end{tabular} \n} } \n\\end{table}"<<endl;

	//   cutflowFile<<"\\begin{table}[H] \n\\caption{$\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0}$ decay via $W/Z$ (on-shell), $[\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0}, \\tilde{\\chi}_{1}^{0}]: [200,100] [GeV]$} \n\\makebox[\\linewidth]{ \n\\resizebox*{!}{\\textheight}{ \n\\renewcommand{\\arraystretch}{0.5} \n\\begin{tabular}{c c c c c} \n\\hline"<<endl;
 //          cutflowFile<<"& ATLAS & GAMBIT & GAMBIT/ATLAS & $\\sigma$-corrected GAMBIT/ATLAS \\\\ \\hline"<<endl;
 //          cutflowFile<<"$\\sigma (pp\\to \\tilde{\\chi}_{1}^{\\pm}, \\tilde{\\chi}_{2}^{0})$ &"<<setprecision(4)<<xsec3ATLAS_200_100<<" $fb$ &"<<setprecision(4)<<xsec()<<"$fb$ &"<<setprecision(4)<< xsec()/xsec3ATLAS_200_100<<" & 1\\\\"<<endl;
 //          cutflowFile<<"Generated Events &"<< cutFlowVector3ATLAS_200_100[0]<<"&"<<cutFlowVector3[0]<<"& - & -\\\\ \\hline"<<endl;

 //          cutflowFile<<"\\multicolumn{5}{c}{Expected events at 36.1 $fb^{-1}$} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS3; i++) {
 //            cutflowFile<<cutFlowVector3_str[i]<<"&"<<setprecision(4)<<cutFlowVector3ATLAS_200_100[i]<<"&"<<setprecision(4)<<cutFlowVector3[i]*xsec_per_event()*luminosity()<<"&"<<setprecision(4)<<cutFlowVector3[i]*xsec_per_event()*luminosity()/cutFlowVector3ATLAS_200_100[i]<<"&"<<setprecision(4)<<(xsec3ATLAS_200_100/xsec())*cutFlowVector3[i]*xsec_per_event()*luminosity()/cutFlowVector3ATLAS_200_100[i]<<"\\\\";
	//    if (i==3 || i==12)cutflowFile<<"\\hline"<<endl;
	//    else cutflowFile<<endl;
 //          }

 //          cutflowFile<<"\\hline \\multicolumn{5}{c}{Percentage (\\%)} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS3; i++) {
 //            cutflowFile<<cutFlowVector3_str[i]<<"&"<<setprecision(4)<<cutFlowVector3ATLAS_200_100[i]*100./cutFlowVector3ATLAS_200_100[2]<<"&"<<setprecision(4)<<cutFlowVector3[i]*100./cutFlowVector3[2]<<"& - & -\\\\";
	//    if (i==3 || i==12)cutflowFile<<"\\hline"<<endl;
	//    else cutflowFile<<endl;
 //          }
 //          cutflowFile<<"\\end{tabular} \n } } \n\\end{table}"<<endl;
	// }

	// if (analysisRunName.find("400_200") != string::npos) {
	//   cutflowFile<<"\\begin{table}[H] \n\\caption{$\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0}$ decay via $W/Z$ (on-shell), $[\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0},\\tilde{\\chi}_{1}^{0}]: [400,200] [GeV]$} \n\\makebox[\\linewidth]{ \n\\renewcommand{\\arraystretch}{0.4} \n\\begin{tabular}{c c c c c} \n\\hline"<<endl;
 //          cutflowFile<<"& ATLAS & GAMBIT & GAMBIT/ATLAS & $\\sigma$-corrected GAMBIT/ATLAS \\\\ \\hline"<<endl;
 //          cutflowFile<<"$\\sigma (pp\\to \\tilde{\\chi}_{1}^{\\pm}, \\tilde{\\chi}_{2}^{0})$ &"<<setprecision(4)<<xsec2ATLAS_400_200<<" $fb$ &"<<setprecision(4)<<xsec()<<"$fb$ &"<<setprecision(4)<< xsec()/xsec2ATLAS_400_200<<" & 1\\\\"<<endl;
 //          cutflowFile<<"Generated Events &"<< cutFlowVector2ATLAS_400_200[0]<<"&"<<cutFlowVector2[0]<<"& - & -\\\\ \\hline"<<endl;
 //          cutflowFile<<"\\multicolumn{5}{c}{Expected events at 36.1 $fb^{-1}$} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS2; i++) {
 //            cutflowFile<<cutFlowVector2_str[i]<<"&"<<setprecision(4)<<cutFlowVector2ATLAS_400_200[i]<<"&"<<setprecision(4)<<cutFlowVector2[i]*xsec_per_event()*luminosity()<<"&"<<setprecision(4)<<cutFlowVector2[i]*xsec_per_event()*luminosity()/cutFlowVector2ATLAS_400_200[i]<<"&"<<setprecision(4)<<(xsec2ATLAS_400_200/xsec())*cutFlowVector2[i]*xsec_per_event()*luminosity()/cutFlowVector2ATLAS_400_200[i]<<"\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\hline \\multicolumn{5}{c}{Percentage (\\%)} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS2; i++) {
 //            cutflowFile<<cutFlowVector2_str[i]<<"&"<<setprecision(4)<<cutFlowVector2ATLAS_400_200[i]*100./cutFlowVector2ATLAS_400_200[2]<<"&"<<setprecision(4)<<cutFlowVector2[i]*100./cutFlowVector2[2]<<"& - & -\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\end{tabular} \n} \n\\end{table}"<<endl;
	// }

	// if (analysisRunName.find("500_100") != string::npos) {
	//   cutflowFile<<"\\begin{table}[H] \n\\caption{$\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0}$ decay via $W/Z$ (on-shell), $[\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0},\\tilde{\\chi}_{1}^{0}]: [500,100] [GeV]$} \n\\makebox[\\linewidth]{ \n\\renewcommand{\\arraystretch}{0.4} \n\\begin{tabular}{c c c c c} \n\\hline"<<endl;
 //          cutflowFile<<"& ATLAS & GAMBIT & GAMBIT/ATLAS & $\\sigma$-corrected GAMBIT/ATLAS \\\\ \\hline"<<endl;
 //          cutflowFile<<"$\\sigma (pp\\to \\tilde{\\chi}_{1}^{\\pm}, \\tilde{\\chi}_{2}^{0})$ &"<<setprecision(4)<<xsec2ATLAS_500_100<<" $fb$ &"<<setprecision(4)<<xsec()<<"$fb$ &"<<setprecision(4)<< xsec()/xsec2ATLAS_500_100<<" & 1\\\\"<<endl;
 //          cutflowFile<<"Generated Events &"<< cutFlowVector2ATLAS_500_100[0]<<"&"<<cutFlowVector2[0]<<"& - & -\\\\ \\hline"<<endl;
 //          cutflowFile<<"\\multicolumn{5}{c}{Expected events at 36.1 $fb^{-1}$} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS2; i++) {
 //            cutflowFile<<cutFlowVector2_str[i]<<"&"<<setprecision(4)<<cutFlowVector2ATLAS_500_100[i]<<"&"<<setprecision(4)<<cutFlowVector2[i]*xsec_per_event()*luminosity()<<"&"<<setprecision(4)<<cutFlowVector2[i]*xsec_per_event()*luminosity()/cutFlowVector2ATLAS_500_100[i]<<"&"<<setprecision(4)<<(xsec2ATLAS_500_100/xsec())*cutFlowVector2[i]*xsec_per_event()*luminosity()/cutFlowVector2ATLAS_500_100[i]<<"\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\hline \\multicolumn{5}{c}{Percentage (\\%)} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS2; i++) {
 //            cutflowFile<<cutFlowVector2_str[i]<<"&"<<setprecision(4)<<cutFlowVector2ATLAS_500_100[i]*100./cutFlowVector2ATLAS_500_100[2]<<"&"<<setprecision(4)<<cutFlowVector2[i]*100./cutFlowVector2[2]<<"& - & -\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\end{tabular} \n} \n\\end{table}"<<endl;
	// }

	// if (analysisRunName.find("800_600") != string::npos) {
	//   cutflowFile<<"\\begin{table}[H] \n\\caption{$\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0}$ decay via $\\tilde{l}$, $[\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{2}^{0},\\tilde{\\chi}_{1}^{0}]: [800,600] [GeV]$} \n\\makebox[\\linewidth]{ \n\\renewcommand{\\arraystretch}{0.4} \n\\begin{tabular}{c c c c c} \n\\hline"<<endl;
 //          cutflowFile<<"& ATLAS & GAMBIT & GAMBIT/ATLAS & $\\sigma$-corrected GAMBIT/ATLAS \\\\ \\hline"<<endl;
 //          cutflowFile<<"$\\sigma (pp\\to \\tilde{\\chi}_{1}^{\\pm}, \\tilde{\\chi}_{2}^{0})$ &"<<setprecision(4)<<xsec4ATLAS_800_600<<" $fb$ &"<<setprecision(4)<<xsec()<<"$fb$ &"<<setprecision(4)<< xsec()/xsec4ATLAS_800_600<<" & 1\\\\"<<endl;
 //          cutflowFile<<"Generated Events &"<< cutFlowVector4ATLAS_800_600[0]<<"&"<<cutFlowVector4[0]<<"& - & -\\\\ \\hline"<<endl;
 //          cutflowFile<<"\\multicolumn{5}{c}{Expected events at 36.1 $fb^{-1}$} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS4; i++) {
 //            cutflowFile<<cutFlowVector4_str[i]<<"&"<<setprecision(4)<<cutFlowVector4ATLAS_800_600[i]<<"&"<<setprecision(4)<<cutFlowVector4[i]*xsec_per_event()*luminosity()<<"&"<<setprecision(4)<<cutFlowVector4[i]*xsec_per_event()*luminosity()/cutFlowVector4ATLAS_800_600[i]<<"&"<<setprecision(4)<<(xsec4ATLAS_800_600/xsec())*cutFlowVector4[i]*xsec_per_event()*luminosity()/cutFlowVector4ATLAS_800_600[i]<<"\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\hline \\multicolumn{5}{c}{Percentage (\\%)} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS4; i++) {
 //            cutflowFile<<cutFlowVector4_str[i]<<"&"<<setprecision(4)<<cutFlowVector4ATLAS_800_600[i]*100./cutFlowVector4ATLAS_800_600[2]<<"&"<<setprecision(4)<<cutFlowVector4[i]*100./cutFlowVector4[2]<<"& - & -\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\end{tabular} \n} \n\\end{table}"<<endl;
	// }

	// if (analysisRunName.find("401_1") != string::npos) {
	//   cutflowFile<<"\\begin{table}[H] \n\\caption{$\\tilde{l}^{\\pm}\\tilde{l}^{\\mp}$ decay into leptons and $\\tilde{\\chi}_{1}^{0}, [\\tilde{l}^{\\pm},\\tilde{\\chi}_{1}^{0}]: [400.5,1] [GeV]$} \n\\makebox[\\linewidth]{ \n\\renewcommand{\\arraystretch}{0.4} \n\\begin{tabular}{c c c c c} \n\\hline"<<endl;
 //          cutflowFile<<"& ATLAS & GAMBIT & GAMBIT/ATLAS & $\\sigma$-corrected GAMBIT/ATLAS \\\\ \\hline"<<endl;
 //          cutflowFile<<"$\\sigma (pp\\to \\tilde{l}^{\\pm}\\tilde{l}^{\\mp})$ &"<<setprecision(4)<<xsec5ATLAS_401_1<<" $fb$ &"<<setprecision(4)<<xsec()<<"$fb$ &"<<setprecision(4)<< xsec()/xsec5ATLAS_401_1<<" & 1\\\\"<<endl;
 //          cutflowFile<<"Generated Events &"<< cutFlowVector5ATLAS_401_1[0]<<"&"<<cutFlowVector5[0]<<"& - & -\\\\ \\hline"<<endl;
 //          cutflowFile<<"\\multicolumn{5}{c}{Expected events at 36.1 $fb^{-1}$} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS5-3; i++) {
 //            cutflowFile<<cutFlowVector5_str[i]<<"&"<<setprecision(4)<<cutFlowVector5ATLAS_401_1[i]<<"&"<<setprecision(4)<<cutFlowVector5[i]*xsec_per_event()*luminosity()<<"&"<<setprecision(4)<<cutFlowVector5[i]*xsec_per_event()*luminosity()/cutFlowVector5ATLAS_401_1[i]<<"&"<<setprecision(4)<<(xsec5ATLAS_401_1/xsec())*cutFlowVector5[i]*xsec_per_event()*luminosity()/cutFlowVector5ATLAS_401_1[i]<<"\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\hline \\multicolumn{5}{c}{Percentage (\\%)} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS5-3; i++) {
 //            cutflowFile<<cutFlowVector5_str[i]<<"&"<<setprecision(4)<<cutFlowVector5ATLAS_401_1[i]*100./cutFlowVector5ATLAS_401_1[2]<<"&"<<setprecision(4)<<cutFlowVector5[i]*100./cutFlowVector5[2]<<"& - & -\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\end{tabular} \n} \n\\end{table}"<<endl;
	// }

	// if (analysisRunName.find("300_150") != string::npos) {
	//   cutflowFile<<"\\begin{table}[H] \n\\caption{$\\tilde{\\chi}_{1}^{\\pm}\\tilde{\\chi}_{1}^{\\mp}$ decay via $\\tilde{l}$, $[\\tilde{\\chi}_{1}^{\\pm}, \\tilde{\\chi}_{1}^{0}]: [300,150] [GeV]$} \n\\makebox[\\linewidth]{ \n\\renewcommand{\\arraystretch}{0.4} \n\\begin{tabular}{c c c c c} \n\\hline"<<endl;
 //          cutflowFile<<"& ATLAS & GAMBIT & GAMBIT/ATLAS & $\\sigma$-corrected GAMBIT/ATLAS \\\\ \\hline"<<endl;
 //          cutflowFile<<"$\\sigma (pp\\to \\tilde{l}^{\\pm}\\tilde{l}^{\\mp})$ &"<<setprecision(4)<<xsec5ATLAS_300_150<<" $fb$ &"<<setprecision(4)<<xsec()<<"$fb$ &"<<setprecision(4)<< xsec()/xsec5ATLAS_300_150<<" & 1\\\\"<<endl;
 //          cutflowFile<<"Generated Events &"<< cutFlowVector5ATLAS_300_150[0]<<"&"<<cutFlowVector5[0]<<"& - & -\\\\ \\hline"<<endl;
 //          cutflowFile<<"\\multicolumn{5}{c}{Expected events at 36.1 $fb^{-1}$} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS5; i++) {
 //            cutflowFile<<cutFlowVector5_str[i]<<"&"<<setprecision(4)<<cutFlowVector5ATLAS_300_150[i]<<"&"<<setprecision(4)<<cutFlowVector5[i]*xsec_per_event()*luminosity()<<"&"<<setprecision(4)<<cutFlowVector5[i]*xsec_per_event()*luminosity()/cutFlowVector5ATLAS_300_150[i]<<"&"<<setprecision(4)<<(xsec5ATLAS_300_150/xsec())*cutFlowVector5[i]*xsec_per_event()*luminosity()/cutFlowVector5ATLAS_300_150[i]<<"\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\hline \\multicolumn{5}{c}{Percentage (\\%)} \\\\ \\hline"<<endl;
 //          for (size_t i=1; i<NCUTS5; i++) {
 //            cutflowFile<<cutFlowVector5_str[i]<<"&"<<setprecision(4)<<cutFlowVector5ATLAS_300_150[i]*100./cutFlowVector5ATLAS_300_150[2]<<"&"<<setprecision(4)<<cutFlowVector5[i]*100./cutFlowVector5[2]<<"& - & -\\\\"<< endl;
 //          }
 //          cutflowFile<<"\\end{tabular} \n} \n\\end{table}"<<endl;
	// }

        // cutflowFile.close();

        //Now fill a results object with the results for each SR
        SignalRegionData results_SR2_SF_loose;
        results_SR2_SF_loose.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_SF_loose.sr_label = "SR2_SF_loose";
        results_SR2_SF_loose.n_observed = 153.;
        results_SR2_SF_loose.n_background = 133.; 
        results_SR2_SF_loose.background_sys = 22.;
        results_SR2_SF_loose.signal_sys = 0.; 
        results_SR2_SF_loose.n_signal = _numSR2_SF_loose;
	add_result(results_SR2_SF_loose);

        SignalRegionData results_SR2_SF_tight;
        results_SR2_SF_tight.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_SF_tight.sr_label = "SR2_SF_tight";
        results_SR2_SF_tight.n_observed = 9.;
        results_SR2_SF_tight.n_background = 9.8; 
        results_SR2_SF_tight.background_sys = 2.9;
        results_SR2_SF_tight.signal_sys = 0.; 
        results_SR2_SF_tight.n_signal = _numSR2_SF_tight;
	add_result(results_SR2_SF_tight);

        SignalRegionData results_SR2_DF_100;
        results_SR2_DF_100.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_DF_100.sr_label = "SR2_DF_100";
        results_SR2_DF_100.n_observed = 78.;
        results_SR2_DF_100.n_background = 68.; 
        results_SR2_DF_100.background_sys = 7.;
        results_SR2_DF_100.signal_sys = 0.; 
        results_SR2_DF_100.n_signal = _numSR2_DF_100;
	add_result(results_SR2_DF_100);

        SignalRegionData results_SR2_DF_150;
        results_SR2_DF_150.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_DF_150.sr_label = "SR2_DF_150";
        results_SR2_DF_150.n_observed = 11;
        results_SR2_DF_150.n_background = 11.5; 
        results_SR2_DF_150.background_sys = 3.1;
        results_SR2_DF_150.signal_sys = 0.; 
        results_SR2_DF_150.n_signal = _numSR2_DF_150;
	add_result(results_SR2_DF_150);

        SignalRegionData results_SR2_DF_200;
        results_SR2_DF_200.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_DF_200.sr_label = "SR2_DF_200";
        results_SR2_DF_200.n_observed = 6.;
        results_SR2_DF_200.n_background = 2.1; 
        results_SR2_DF_200.background_sys = 1.9;
        results_SR2_DF_200.signal_sys = 0.; 
        results_SR2_DF_200.n_signal = _numSR2_DF_200;
	add_result(results_SR2_DF_200);

        SignalRegionData results_SR2_DF_300;
        results_SR2_DF_300.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_DF_300.sr_label = "SR2_DF_300";
        results_SR2_DF_300.n_observed = 2.;
        results_SR2_DF_300.n_background = 0.6; 
        results_SR2_DF_300.background_sys = 0.6;
        results_SR2_DF_300.signal_sys = 0.; 
        results_SR2_DF_300.n_signal = _numSR2_DF_300;
	add_result(results_SR2_DF_300);

        SignalRegionData results_SR2_int;
        results_SR2_int.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_int.sr_label = "SR2_int";
        results_SR2_int.n_observed = 2.;
        results_SR2_int.n_background = 4.1; 
        results_SR2_int.background_sys = 2.6;
        results_SR2_int.signal_sys = 0.; 
        results_SR2_int.n_signal = _numSR2_int;
	add_result(results_SR2_int);

        SignalRegionData results_SR2_high;
        results_SR2_high.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_high.sr_label = "SR2_high";
        results_SR2_high.n_observed = 0.;
        results_SR2_high.n_background = 1.6; 
        results_SR2_high.background_sys = 1.6;
        results_SR2_high.signal_sys = 0.; 
        results_SR2_high.n_signal = _numSR2_high;
	add_result(results_SR2_high);

        SignalRegionData results_SR2_low;
        results_SR2_low.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR2_low.sr_label = "SR2_low";
        results_SR2_low.n_observed = 11.;
        results_SR2_low.n_background = 4.2; 
        results_SR2_low.background_sys = 3.8;
        results_SR2_low.signal_sys = 0.; 
        results_SR2_low.n_signal = _numSR2_low;
	add_result(results_SR2_low);

        SignalRegionData results_SR3_slep_a;
        results_SR3_slep_a.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_slep_a.sr_label = "SR3_slep_a";
        results_SR3_slep_a.n_observed = 4.;
        results_SR3_slep_a.n_background = 2.23; 
        results_SR3_slep_a.background_sys = 0.79;
        results_SR3_slep_a.signal_sys = 0.; 
        results_SR3_slep_a.n_signal = _numSR3_slep_a;
	add_result(results_SR3_slep_a);

        SignalRegionData results_SR3_slep_b;
        results_SR3_slep_b.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_slep_b.sr_label = "SR3_slep_b";
        results_SR3_slep_b.n_observed = 3.;
        results_SR3_slep_b.n_background = 2.79; 
        results_SR3_slep_b.background_sys = 0.43;
        results_SR3_slep_b.signal_sys = 0.; 
        results_SR3_slep_b.n_signal = _numSR3_slep_b;
	add_result(results_SR3_slep_b);

        SignalRegionData results_SR3_slep_c;
        results_SR3_slep_c.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_slep_c.sr_label = "SR3_slep_c";
        results_SR3_slep_c.n_observed = 9.;
        results_SR3_slep_c.n_background = 5.41; 
        results_SR3_slep_c.background_sys = 0.93;
        results_SR3_slep_c.signal_sys = 0.; 
        results_SR3_slep_c.n_signal = _numSR3_slep_c;
	add_result(results_SR3_slep_c);

        SignalRegionData results_SR3_slep_d;
        results_SR3_slep_d.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_slep_d.sr_label = "SR3_slep_d";
        results_SR3_slep_d.n_observed = 0.;
        results_SR3_slep_d.n_background = 1.42; 
        results_SR3_slep_d.background_sys = 0.38;
        results_SR3_slep_d.signal_sys = 0.; 
        results_SR3_slep_d.n_signal = _numSR3_slep_d;
	add_result(results_SR3_slep_d);

        SignalRegionData results_SR3_slep_e;
        results_SR3_slep_e.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_slep_e.sr_label = "SR3_slep_e";
        results_SR3_slep_e.n_observed = 0.;
        results_SR3_slep_e.n_background = 1.14; 
        results_SR3_slep_e.background_sys = 0.23;
        results_SR3_slep_e.signal_sys = 0.; 
        results_SR3_slep_e.n_signal = _numSR3_slep_e;
	add_result(results_SR3_slep_e);

        SignalRegionData results_SR3_WZ_0Ja;
        results_SR3_WZ_0Ja.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_WZ_0Ja.sr_label = "SR3_WZ_0Ja";
        results_SR3_WZ_0Ja.n_observed = 21.;
        results_SR3_WZ_0Ja.n_background = 21.74; 
        results_SR3_WZ_0Ja.background_sys = 2.85;
        results_SR3_WZ_0Ja.signal_sys = 0.; 
        results_SR3_WZ_0Ja.n_signal = _numSR3_WZ_0Ja;
	add_result(results_SR3_WZ_0Ja);

        SignalRegionData results_SR3_WZ_0Jb;
        results_SR3_WZ_0Jb.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_WZ_0Jb.sr_label = "SR3_WZ_0Jb";
        results_SR3_WZ_0Jb.n_observed = 1.;
        results_SR3_WZ_0Jb.n_background = 2.68; 
        results_SR3_WZ_0Jb.background_sys = 0.46;
        results_SR3_WZ_0Jb.signal_sys = 0.; 
        results_SR3_WZ_0Jb.n_signal = _numSR3_WZ_0Jb;
	add_result(results_SR3_WZ_0Jb);

        SignalRegionData results_SR3_WZ_0Jc;
        results_SR3_WZ_0Jc.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_WZ_0Jc.sr_label = "SR3_WZ_0Jc";
        results_SR3_WZ_0Jc.n_observed = 2.;
        results_SR3_WZ_0Jc.n_background = 1.56; 
        results_SR3_WZ_0Jc.background_sys = 0.33;
        results_SR3_WZ_0Jc.signal_sys = 0.; 
        results_SR3_WZ_0Jc.n_signal = _numSR3_WZ_0Jc;
	add_result(results_SR3_WZ_0Jc);

        SignalRegionData results_SR3_WZ_1Ja;
        results_SR3_WZ_1Ja.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_WZ_1Ja.sr_label = "SR3_WZ_1Ja";
        results_SR3_WZ_1Ja.n_observed = 1.;
        results_SR3_WZ_1Ja.n_background = 2.21; 
        results_SR3_WZ_1Ja.background_sys = 0.53;
        results_SR3_WZ_1Ja.signal_sys = 0.; 
        results_SR3_WZ_1Ja.n_signal = _numSR3_WZ_1Ja;
	add_result(results_SR3_WZ_1Ja);

        SignalRegionData results_SR3_WZ_1Jb;
        results_SR3_WZ_1Jb.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_WZ_1Jb.sr_label = "SR3_WZ_1Jb";
        results_SR3_WZ_1Jb.n_observed = 3.;
        results_SR3_WZ_1Jb.n_background = 1.82; 
        results_SR3_WZ_1Jb.background_sys = 0.26;
        results_SR3_WZ_1Jb.signal_sys = 0.; 
        results_SR3_WZ_1Jb.n_signal = _numSR3_WZ_1Jb;
	add_result(results_SR3_WZ_1Jb);

        SignalRegionData results_SR3_WZ_1Jc;
        results_SR3_WZ_1Jc.analysis_name = "Analysis_ATLAS_13TeV_MultiLEP_36invfb";
        results_SR3_WZ_1Jc.sr_label = "SR3_WZ_1Jc";
        results_SR3_WZ_1Jc.n_observed = 4.;
        results_SR3_WZ_1Jc.n_background = 1.26; 
        results_SR3_WZ_1Jc.background_sys = 0.34;
        results_SR3_WZ_1Jc.signal_sys = 0.; 
        results_SR3_WZ_1Jc.n_signal = _numSR3_WZ_1Jc;
	add_result(results_SR3_WZ_1Jc);

      }

      vector<vector<HEPUtils::Particle*>> getSFOSpair(vector<HEPUtils::Particle*> leptons) {
        vector<vector<HEPUtils::Particle*>> SFOSpair_container;
        for (size_t iLe1=0;iLe1<leptons.size();iLe1++) {
          for (size_t iLe2=0;iLe2<leptons.size();iLe2++) {
            if (leptons.at(iLe1)->abspid()==leptons.at(iLe2)->abspid() && leptons.at(iLe1)->pid()!=leptons.at(iLe2)->pid()) {
              vector<HEPUtils::Particle*> SFOSpair;
              SFOSpair.push_back(leptons.at(iLe1));
              SFOSpair.push_back(leptons.at(iLe2));
              SFOSpair_container.push_back(SFOSpair);
            }
          }
        }
        return SFOSpair_container;
      }

      vector<vector<HEPUtils::Particle*>> getOSpair(vector<HEPUtils::Particle*> leptons) {
        vector<vector<HEPUtils::Particle*>> OSpair_container;
        for (size_t iLe1=0;iLe1<leptons.size();iLe1++) {
          for (size_t iLe2=0;iLe2<leptons.size();iLe2++) {
            if (leptons.at(iLe1)->pid()*leptons.at(iLe2)->pid()<0.) {
              vector<HEPUtils::Particle*> OSpair;
              OSpair.push_back(leptons.at(iLe1));
              OSpair.push_back(leptons.at(iLe2));
              OSpair_container.push_back(OSpair);
            }
          }
        }
        return OSpair_container;
      }

      vector<HEPUtils::P4> get_W_ISR(vector<HEPUtils::Jet*> jets, HEPUtils::P4 Z, HEPUtils::P4 met) {
	HEPUtils::P4 Z_met_sys=Z+met;
	double deltaR_min=999;
	size_t Wjets_id1;
	size_t Wjets_id2;
	for (size_t i=0;i<jets.size();i++) {
	  for (size_t j=0;j<jets.size();j++) {
	    if (i!=j) {
	      HEPUtils::P4 jj_sys=jets.at(i)->mom()+jets.at(j)->mom();
	      double deltaR=fabs(jj_sys.deltaR_eta(Z_met_sys));
	      if (deltaR<deltaR_min) {
		deltaR_min=deltaR;
		Wjets_id1=i;
		Wjets_id2=j;
	      }
	    }
	  }
	}
	HEPUtils::P4 W=jets.at(Wjets_id2)->mom()+jets.at(Wjets_id1)->mom();
	HEPUtils::P4 ISR;
	HEPUtils::P4 j0=jets.at(Wjets_id2)->mom();
	HEPUtils::P4 j1=jets.at(Wjets_id1)->mom();
	for (size_t k=0;k<jets.size();k++) {
	  if ((k!=Wjets_id1) && (k!=Wjets_id2))ISR+=(jets.at(k)->mom());
	}
	vector<HEPUtils::P4> W_ISR;
	W_ISR.push_back(W);
	W_ISR.push_back(ISR);
	W_ISR.push_back(j0);
	W_ISR.push_back(j1);
	return W_ISR;
      }
		 

    protected:
      void clear() {
        _numSR2_SF_loose=0;
        _numSR2_SF_tight=0;
        _numSR2_DF_100=0;
        _numSR2_DF_150=0;
        _numSR2_DF_200=0;
        _numSR2_DF_300=0;
        _numSR2_int=0;
        _numSR2_high=0;
        _numSR2_low=0;
        _numSR3_slep_a=0;
        _numSR3_slep_b=0;
        _numSR3_slep_c=0;
        _numSR3_slep_d=0;
        _numSR3_slep_e=0;
        _numSR3_WZ_0Ja=0;
        _numSR3_WZ_0Jb=0;
        _numSR3_WZ_0Jc=0;
        _numSR3_WZ_1Ja=0;
        _numSR3_WZ_1Jb=0;
        _numSR3_WZ_1Jc=0;
        
        std::fill(cutFlowVector1.begin(), cutFlowVector1.end(), 0);
        std::fill(cutFlowVector2.begin(), cutFlowVector2.end(), 0);
        std::fill(cutFlowVector3.begin(), cutFlowVector3.end(), 0);
        std::fill(cutFlowVector4.begin(), cutFlowVector4.end(), 0);
        std::fill(cutFlowVector5.begin(), cutFlowVector5.end(), 0);
      }

    };


    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_MultiLEP_36invfb)


  }
}
