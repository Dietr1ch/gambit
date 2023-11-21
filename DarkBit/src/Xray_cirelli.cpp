//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  X ray likelihoods.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Chris Cappiello
///          (cvc1@queensu.ca)
///  \date 2023 Aug
///
///  *********************************************

#include <gsl/gsl_odeiv2.h>

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/DarkBit/DarkBit_utils.hpp"
#include "gambit/DarkBit/DarkBit_types.hpp"
#include "gambit/Utils/interp_collection.hpp"
#include "gambit/Utils/numerical_constants.hpp"
#include "gambit/Utils/util_functions.hpp"

namespace Gambit
{
  namespace DarkBit
  {
    void Xrayloglikescirelli(double &result)
    {
      using Interpolator2D = Utils::interp2d_gsl_collection;
      using namespace Pipes::Xrayloglikescirelli;
      std::string DM_ID = Dep::WIMP_properties->name;
      std::string DMbar_ID = Dep::WIMP_properties->conjugate;
      double DM_mass = Dep::WIMP_properties->mass;
      double suppression = *Dep::ID_suppression;
//      double suppression=1000.0;
      std::cout <<"at a mass of "<< DM_mass << " suppression is equal to " << suppression;
      TH_Process process = Dep::TH_ProcessCatalog->getProcess(DM_ID, DMbar_ID);

      double logchisqre;
      double logchisqrmu;
      double logchisqrpi;
      double logchisqr;
//      double sigmavcms = -28.0;   // Placeholder cross section

      static Interpolator2D xraygride(
        "xraygride",
        GAMBIT_DIR "/DarkBit/src/xraylikelihoodse.dat",
        { "me","sigmave","loglikee"}
      );
      static Interpolator2D xraygridmu(
        "xraygridmu",
        GAMBIT_DIR "/DarkBit/src/xraylikelihoodsmu.dat",
        { "mmu","sigmavmu","loglikemu"}
      );
      static Interpolator2D xraygridpi(
        "xraygridpi",
        GAMBIT_DIR "/DarkBit/src/xraylikelihoodspi.dat",
        { "mpi","sigmavpi","loglikepi"}
      );

      double m_mine = pow(10,xraygride.x_min-3);
      double m_maxe = pow(10,xraygride.x_max-3);
      double sigmav_mine = pow(10,xraygride.y_min);
      double sigmav_maxe = pow(10,xraygride.y_max);
      double m_minmu = pow(10,xraygridmu.x_min-3);
      double m_maxmu = pow(10,xraygridmu.x_max-3);
      double sigmav_minmu = pow(10,xraygridmu.y_min);
      double sigmav_maxmu = pow(10,xraygridmu.y_max);
      double m_minpi = pow(10,xraygridpi.x_min-3);
      double m_maxpi = pow(10,xraygridpi.x_max-3);
      double sigmav_minpi = pow(10,xraygridpi.y_min);
      double sigmav_maxpi = pow(10,xraygridpi.y_max);

      double sve=0;
      double svmu=0;
      double svpi=0;
      std::vector<std::string> fs;
      std::string finalStates;
      double rate=0;

      for (std::vector<TH_Channel>::iterator it = process.channelList.begin(); it!=process.channelList.end();it++)
            {
              fs = it->finalStateIDs;
              finalStates = fs[0] + " " + fs[1];
              rate = it->genRate->bind("v")->eval(0.);
              if(fs[0]=="e+_1")
              {
                    sve += rate*suppression;
              }
              else if(fs[0]=="e+_2")
              {
                    svmu += rate*suppression;
              }
              else if(fs[0]=="pi+")
              {
                    svpi += rate*suppression;
              }
            }

      if (DM_mass > m_maxe || DM_mass < m_mine)
      {
            logchisqre=-3;
      }
      else if(sve > sigmav_maxe)
      {
            logchisqre = xraygride.eval(log10(DM_mass)+3, log10(sigmav_maxe));
      }
      else if(sve < sigmav_mine)
      {
            logchisqre=-3;
      }
      else
      {
            logchisqre = xraygride.eval(log10(DM_mass)+3, log10(sve));
      }
//
      if (DM_mass > m_maxmu || DM_mass < m_minmu)
      {     
            logchisqrmu=-3;
      }
      else if(svmu > sigmav_maxmu)
      {
            logchisqrmu = xraygridmu.eval(log10(DM_mass)+3, log10(sigmav_maxmu));
      }
      else if(svmu < sigmav_minmu)
      {
            logchisqrmu=-3;
      }
      else
      {
            logchisqrmu = xraygridmu.eval(log10(DM_mass)+3, log10(svmu));
      }
//
      if (DM_mass > m_maxpi || DM_mass < m_minpi)
      {
            logchisqrpi=-3;
      } 
      else if(svpi > sigmav_maxpi)
      {
            logchisqrpi = xraygridpi.eval(log10(DM_mass)+3, log10(sigmav_maxpi));
      }
      else if(svpi < sigmav_minpi)
      {
            logchisqrpi=-3;
      }
      else
      {
            logchisqrpi = xraygridpi.eval(log10(DM_mass)+3, log10(svpi));
      }
      logchisqr = std::max(logchisqrpi,std::max(logchisqre,logchisqrmu));
//      logchisqr = logchisqrmu;
//      logchisqr = std::max(logchisqrpi,logchisqrmu);
      result = -pow(10,logchisqr)/2;
//      result = suppression;
    }
  }
}
