//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Function definitions of EWPOBit.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2014 Nov
///
///  \author Chris Rogan
///          (crogan@cern.ch)
///  \date 2014 Aug
///
///  *********************************************

/// Data to include: $  M_W (BW mass parameter ~ pole) = 80.385  +/- 0.015  GeV (1 sigma), Gaussian.
///                  #  M_Z (BW mass parameter ~ pole) = 91.1876 +/- 0.0021 GeV (1 sigma), Gaussian.  
///                  %  m_c (mc)^MSbar                 = 1.275   +/- 0.025  GeV (1 sigma), Gaussian.
///                  ** m_b (mb)^MSbar                 = 4.18    +/- 0.03   GeV (1 sigma), Gaussian.
///                  *  m_t (pole)                     = 173.34  +/- 0.76   GeV (1 sigma), Gaussian.
///                  @  alpha^{-1}(mZ)^MSbar           = 127.940 +/- 0.014      (1 sigma), Gaussian.  FIXME RECHECK
///                  !  alpha_s(mZ)^MSbar              = 0.1185  +/- 0.0006     (1 sigma), Gaussian.
///                  @  sin^2theta_W^leptonic_effective~ sin^2theta_W(mZ)^MSbar + 0.00029
///                  @  sin^2theta_W^leptonic_effective= 0.23155 +/- 0.00005    (1 sigma), Gaussian.  (PDG global fit)
///                  @  deltarho                       = 0.00040 +/- 0.00024    (1 sigma), Gaussian.  (PDG global fit)
/// $  from http://pdg.lbl.gov/2014/listings/rpp2014-list-w-boson.pdf = K.A. Olive et al. (Particle Data Group), Chin. Phys. C38, 090001 (2014)
/// #  from http://pdg.lbl.gov/2014/listings/rpp2014-list-z-boson.pdf = K.A. Olive et al. (Particle Data Group), Chin. Phys. C38, 090001 (2014)
/// %  from http://pdg.lbl.gov/2014/listings/rpp2014-list-c-quark.pdf = K.A. Olive et al. (Particle Data Group), Chin. Phys. C38, 090001 (2014)
/// ** from http://pdg.lbl.gov/2014/listings/rpp2014-list-b-quark.pdf = K.A. Olive et al. (Particle Data Group), Chin. Phys. C38, 090001 (2014)
/// @  from http://pdg.lbl.gov/2014/reviews/rpp2014-rev-standard-model.pdf = K.A. Olive et al. (Particle Data Group), Chin. Phys. C38, 090001 (2014)
/// !  from http://pdg.lbl.gov/2014/reviews/rpp2014-rev-qcd.pdf = K.A. Olive et al. (Particle Data Group), Chin. Phys. C38, 090001 (2014)
/// *  from http://arxiv.org/abs/1403.4427

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/EWPOBit/EWPOBit_rollcall.hpp"

namespace Gambit
{

  namespace EWPOBit
  {

    using namespace LogTags;

    // Module functions

    void FH_PrecisionObs(fh_PrecisionObs &result) 
    {
      using namespace Pipes::FH_PrecisionObs;

      cout << "****** calling FH_PrecisionObs ******" << endl;

      fh_real gm2;        // g_{mu}-2
      fh_real Deltarho;   // deltaRho
      fh_real MWMSSM;     // W pole mass in MSSM
      fh_real MWSM;       // W pole mass in SM
      fh_real SW2MSSM;    // sin^2theta_W^leptonic_effective in MSSM
      fh_real SW2SM;      // sin^2theta_W^leptonic_effective in SM
      fh_real edmeTh;     // electron EDM
      fh_real edmn;       // neutron EDM
      fh_real edmHg;      // mercury EDM
      int ccb;            // ?

      int error = 1;
      BEreq::FHConstraints(error, gm2, Deltarho, 
         MWMSSM, MWSM, SW2MSSM, SW2SM,
         edmeTh, edmn, edmHg, ccb);

      fh_PrecisionObs PrecisionObs;
      PrecisionObs.gmu2 = gm2;       
      PrecisionObs.deltaRho = Deltarho;   
      PrecisionObs.MW_MSSM = MWMSSM;     
      PrecisionObs.MW_SM = MWSM;      
      PrecisionObs.sinW2_MSSM = SW2MSSM; 
      PrecisionObs.sinW2_SM = SW2SM;  
      PrecisionObs.edm_ele = edmeTh;    
      PrecisionObs.edm_neu = edmn;    
      PrecisionObs.edm_Hg = edmHg;     
      PrecisionObs.ccb = ccb;           

      result = PrecisionObs;
    }

     
    void SP_PrecisionObs(double &result) 
    {
      using namespace Pipes::SP_PrecisionObs;
      int error = 0;
      Farray<Fdouble,1,35> SM_Obs;
      Farray<Fdouble,1,35> MSSM_Obs;
      
         
      BEreq::CalcObs_SUSYPOPE(error, SM_Obs, MSSM_Obs);
      if(error != 0)
         {
            std::cout << "something went wrong" << std::endl;
         }
      else 
         {
            std::cout << " MW in SM = " << SM_Obs(1) << std::endl;
            std::cout << " MW in MSSM = " << MSSM_Obs(1) << std::endl;
            
         }
      result = 0.1;
      return;

    }
     
  }
}
