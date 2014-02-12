#include "xsec.h"
#include <iostream>
#include <string>
#include <math.h>

using namespace std;

Evaluator::Evaluator(){
    
}

double Evaluator::xsec(string process, double * par){
    // Returns cross section in pb
    // par is expected to be 24 parameter array with MSSM parameters:
    // tanB, M_1, M_2, M_3, At, Ab, Atau, mu, mA
    // meL, mmuL, mtauL, meR, mmuR, mtauR
    // mqL1, muR, mdR, mqL2, mcR, msR, mqL3, mtR, mbR
    
    double xsec = -1;
    
    if(process == "nn_n1n1") xsec = nn_n1n1.Value(0,par);
    if(process == "nn_n1n2") xsec = nn_n1n2.Value(0,par);

    if(process == "b1b1") xsec = b1b1.Value(0,par);
    if(process == "b2b2") xsec = b2b2.Value(0,par);
    if(process == "t1t1") xsec = t1t1.Value(0,par);
    if(process == "t2t2") xsec = t2t2.Value(0,par);

    if(process == "cLg") xsec = cLg.Value(0,par);
    if(process == "cRg") xsec = cRg.Value(0,par);
    if(process == "dLg") xsec = dLg.Value(0,par);
    if(process == "dRg") xsec = dRg.Value(0,par);
    if(process == "sLg") xsec = sLg.Value(0,par);
    if(process == "sRg") xsec = sRg.Value(0,par);
    if(process == "uLg") xsec = uLg.Value(0,par);
    if(process == "uRg") xsec = uRg.Value(0,par);

    if(process == "sb_cLcL") xsec = sb_cLcL.Value(0,par);
    if(process == "sb_cLcR") xsec = sb_cLcR.Value(0,par);
    if(process == "sb_cLdL") xsec = sb_cLdL.Value(0,par);
    if(process == "sb_cLdR") xsec = sb_cLdR.Value(0,par);
    if(process == "sb_cLsL") xsec = sb_cLsL.Value(0,par);
    if(process == "sb_cLsR") xsec = sb_cLsR.Value(0,par);
    if(process == "sb_cLuL") xsec = sb_cLuL.Value(0,par);
    if(process == "sb_cLuR") xsec = sb_cLuR.Value(0,par);
    if(process == "sb_cRcR") xsec = sb_cRcR.Value(0,par);
    if(process == "sb_dLcR") xsec = sb_dLcR.Value(0,par);
    if(process == "sb_dLdL") xsec = sb_dLdL.Value(0,par);
    if(process == "sb_dLdR") xsec = sb_dLdR.Value(0,par);
    if(process == "sb_dLsR") xsec = sb_dLsR.Value(0,par);
    if(process == "sb_dLuR") xsec = sb_dLuR.Value(0,par);
    if(process == "sb_dRcR") xsec = sb_dRcR.Value(0,par);
    if(process == "sb_dRdR") xsec = sb_dRdR.Value(0,par);
    if(process == "sb_dRsR") xsec = sb_dRsR.Value(0,par);
    if(process == "sb_sLcR") xsec = sb_sLcR.Value(0,par);
    if(process == "sb_sLdL") xsec = sb_sLdL.Value(0,par);
    if(process == "sb_sLdR") xsec = sb_sLdR.Value(0,par);
    if(process == "sb_sLsL") xsec = sb_sLsL.Value(0,par);
    if(process == "sb_sLsR") xsec = sb_sLsR.Value(0,par);
    if(process == "sb_sLuL") xsec = sb_sLuL.Value(0,par);
    if(process == "sb_sLuR") xsec = sb_sLuR.Value(0,par);
    if(process == "sb_sRcR") xsec = sb_sRcR.Value(0,par);
    if(process == "sb_sRsR") xsec = sb_sRsR.Value(0,par);
    if(process == "sb_uLcR") xsec = sb_uLcR.Value(0,par);
    if(process == "sb_uLdR") xsec = sb_uLdR.Value(0,par);
    if(process == "sb_uLsR") xsec = sb_uLsR.Value(0,par);
    if(process == "sb_uLuL") xsec = sb_uLuL.Value(0,par);
    if(process == "sb_uLuR") xsec = sb_uLuR.Value(0,par);
    if(process == "sb_uRcR") xsec = sb_uRcR.Value(0,par);
    if(process == "sb_uRdR") xsec = sb_uRdR.Value(0,par);
    if(process == "sb_uRsR") xsec = sb_uRsR.Value(0,par);
    if(process == "sb_uRuR") xsec = sb_uRuR.Value(0,par);

    if(process == "ss_cLcL") xsec = ss_cLcL.Value(0,par);
    if(process == "ss_cLcR") xsec = ss_cLcR.Value(0,par);
    if(process == "ss_cLdL") xsec = ss_cLdL.Value(0,par);
    if(process == "ss_cLdR") xsec = ss_cLdR.Value(0,par);
    if(process == "ss_cLsL") xsec = ss_cLsL.Value(0,par);
    if(process == "ss_cLsR") xsec = ss_cLsR.Value(0,par);
    if(process == "ss_cLuL") xsec = ss_cLuL.Value(0,par);
    if(process == "ss_cLuR") xsec = ss_cLuR.Value(0,par);
    if(process == "ss_cRcR") xsec = ss_cRcR.Value(0,par);
    if(process == "ss_dLcR") xsec = ss_dLcR.Value(0,par);
    if(process == "ss_dLdL") xsec = ss_dLdL.Value(0,par);
    if(process == "ss_dLdR") xsec = ss_dLdR.Value(0,par);
    if(process == "ss_dLsR") xsec = ss_dLsR.Value(0,par);
    if(process == "ss_dLuR") xsec = ss_dLuR.Value(0,par);
    if(process == "ss_dRcR") xsec = ss_dRcR.Value(0,par);
    if(process == "ss_dRdR") xsec = ss_dRdR.Value(0,par);
    if(process == "ss_dRsR") xsec = ss_dRsR.Value(0,par);
    if(process == "ss_sLcR") xsec = ss_sLcR.Value(0,par);
    if(process == "ss_sLdL") xsec = ss_sLdL.Value(0,par);
    if(process == "ss_dLdR") xsec = ss_dLdR.Value(0,par);
    if(process == "ss_sLdR") xsec = ss_sLdR.Value(0,par);
    if(process == "ss_sLsL") xsec = ss_sLsL.Value(0,par);
    if(process == "ss_sLsR") xsec = ss_sLsR.Value(0,par);
    if(process == "ss_sLuL") xsec = ss_sLuR.Value(0,par);
    if(process == "ss_sLuR") xsec = ss_sLuR.Value(0,par);
    if(process == "ss_sRcR") xsec = ss_sRcR.Value(0,par);
    if(process == "ss_sRsR") xsec = ss_sRsR.Value(0,par);
    if(process == "ss_uLcR") xsec = ss_uLcR.Value(0,par);
    if(process == "ss_uLdR") xsec = ss_uLdR.Value(0,par);
    if(process == "ss_uLsR") xsec = ss_uLsR.Value(0,par);
    if(process == "ss_uLuL") xsec = ss_uLuL.Value(0,par);
    if(process == "ss_uLuR") xsec = ss_uLuR.Value(0,par);
    if(process == "ss_uRcR") xsec = ss_uRcR.Value(0,par);
    if(process == "ss_uRdR") xsec = ss_uRdR.Value(0,par);
    if(process == "ss_uRsR") xsec = ss_uRsR.Value(0,par);
    if(process == "ss_uRuR") xsec = ss_uRuR.Value(0,par);
    
    if(process == "gg") xsec = gg.Value(0,par);
    
    // The NN gives log10 or cross section
    xsec = pow(10.,xsec);
    

    cout << process << " got evaluated: " << xsec << " pb" << endl;
    
    if(xsec < 0) cout << "Something went wrong, wrong process?" << endl;
    
    return xsec;
}