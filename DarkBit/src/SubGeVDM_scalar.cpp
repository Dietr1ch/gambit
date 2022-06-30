//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Implementation of SubGeVDM_scalar
///  DarkBit routines.
///
///  Authors (add name and date if you modify):    
///
///  \author Felix Kahlhoefer
///         (kahlhoefer@kit.edu)
///  \date May 2022
///                                                  
///  ********************************************* 
///
/// - Tell DarkSUSY about resonances in cross section ratio 
/// - Implement DecayBit routine for dark photon decays
/// - Tons of bug testing

#include "boost/make_shared.hpp"

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/Elements/virtual_photon.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/Utils/ascii_table_reader.hpp"
#include "boost/make_shared.hpp"
#include "gambit/DarkBit/DarkBit_utils.hpp"

namespace Gambit
{

  namespace DarkBit
  {

    class SubGeVDM_scalar
    {
      public:
      /// Initialize SubGeVDM_scalar object (branching ratios etc)
      SubGeVDM_scalar(TH_ProcessCatalog* const catalog, double gammaAp)
        : Gamma_Ap(gammaAp)
        {
          mAp   = catalog->getParticleProperty("Ap").mass;
          mb   = catalog->getParticleProperty("d_3").mass;
          mtau = catalog->getParticleProperty("e-_3").mass;
          mmu = catalog->getParticleProperty("e-_2").mass;
          me = catalog->getParticleProperty("e-_1").mass;
          mpi   = catalog->getParticleProperty("pi+").mass;

        };
      ~SubGeVDM_scalar() {};
            
      /// Helper function (Breit-Wigner)
      double DAp2 (double s)
      {
        return 1/((s-mAp*mAp)*(s-mAp*mAp)+mAp*mAp*Gamma_Ap*Gamma_Ap);
      }
      
      double sv(std::string channel, double gDM, double gSM, double mass, double v)
      {
          double s = 4*mass*mass/(1-v*v/4);
          double sqrt_s = sqrt(s);

          if ( channel == "bb" and sqrt_s < mb*2 ) return 0;
          if ( channel == "ee" and sqrt_s < me*2  ) return 0;
          if ( channel == "mumu" and sqrt_s < mmu*2  ) return 0;
          if ( channel == "tautau" and sqrt_s < mtau*2 ) return 0;
          if ( channel == "pipi" and sqrt_s < mpi*2 ) return 0;
          if ( channel == "ApAp" and sqrt_s < mAp*2) return 0;

          if ( channel == "bb" ) return sv_ff(gDM, gSM, mass, v, mb, -1/3., 3);
          if ( channel == "ee" ) return sv_ff(gDM, gSM, mass, v, me, -1., 1);
          if ( channel == "mumu" ) return sv_ff(gDM, gSM, mass, v, mmu, -1., 1);
          if ( channel == "tautau" ) return sv_ff(gDM, gSM, mass, v, mtau, -1., 1);
          if ( channel == "pipi" )
          {
            if (sqrt_s < mb*2 ) return hadronic_cross_section_ratio(sqrt_s) * sv_ff(gDM, gSM, mass, v, mmu, -1., 1);
            else return hadronic_cross_section_ratio(sqrt_s) * sv_ff(gDM, gSM, mass, v, mmu, -1., 1) - sv_ff(gDM, gSM, mass, v, mb, -1/3., 3); //Avoid double-counting of bb final state
          }
          if ( channel == "ApAp" ) return sv_ApAp(gDM, mass, v);
          
          return 0;
      }

      // Annihilation into fermions
      double sv_ff(
            double gDM, double gSM, double mass, double v, double mf, double charge, int colour)
      {
          double s = 4*mass*mass/(1-v*v/4);
          double vf = sqrt(1-4*pow(mf,2)/s);
          double y = s/pow(mass, 2);
          double GeV2tocm3s1 = gev2cm2*s2cm;
          return colour*pow(gDM*gSM*charge,2)*pow(v,2)*vf*(2*pow(mass,2)+pow(mf,2))/12/M_PI*DAp2(s)*GeV2tocm3s1; // See eq. (28) of arXiv:1707.03835 and eq. (9) of arXiv:2010.02954.
      }

      /// Annihilation into ApAp
      double sv_ApAp(double gDM, double mass, double v)
      {
          double GeV2tocm3s1 = gev2cm2*s2cm;
          double s = 4*mass*mass/(1-v*v/4);  // v is relative velocity
          
          return pow(gDM,4) / (M_PI * s) * sqrt(1 - pow(2 * mAp, 2) / s) * GeV2tocm3s1; // See eq. (6) of arXiv:0810.1502.
      }

      private:
        double Gamma_Ap, mb, me, mmu, mtau, mAp, mpi;      
    };

    void DarkMatter_ID_SubGeVDM_scalar(std::string & result) { result = "S"; }
    void DarkMatterConj_ID_SubGeVDM_scalar(std::string & result) { result = "S~"; }

    void TH_ProcessCatalog_SubGeVDM_scalar(TH_ProcessCatalog &result)
    {
      using namespace Pipes::TH_ProcessCatalog_SubGeVDM_scalar;
      using std::vector;
      using std::string;
      
      // Initialize empty catalog, main annihilation process
      TH_ProcessCatalog catalog;
      TH_Process process_ann("S", "S~");
      
      // Explicitly state that complex scalar DM is not self-conjugate to add extra 
      // factors of 1/2 where necessary
      process_ann.isSelfConj = false;      
      
      // Import particle masses 
      
      // Convenience macros
      #define getSMmass(Name, spinX2) catalog.particleProperties.insert(std::pair<string, TH_ParticleProperty> (Name, TH_ParticleProperty(SM.get(Par::Pole_Mass,Name), spinX2)));
      #define addParticle(Name, Mass, spinX2) catalog.particleProperties.insert(std::pair<string, TH_ParticleProperty> (Name, TH_ParticleProperty(Mass, spinX2)));
      
      // Import Spectrum objects
      const Spectrum& spec = *Dep::SubGeVDM_scalar_spectrum;
      const SubSpectrum& he = spec.get_HE();
      const SubSpectrum& SM = spec.get_LE();
      const SMInputs& SMI   = spec.get_SMInputs();
      
      // Import couplings
      double gDM = he.get(Par::dimensionless,"gDM");
      double kappa = he.get(Par::dimensionless,"kappa");
      double v = he.get(Par::mass1,"vev");

      double alpha_em = 1.0 / SMI.alphainv;
      double e = pow( 4*pi*( alpha_em ),0.5) ;

      // Get SM pole masses
      getSMmass("e-_1",     1)
      getSMmass("e+_1",     1)
      getSMmass("e-_2",     1)
      getSMmass("e+_2",     1)
      getSMmass("e-_3",     1)
      getSMmass("e+_3",     1)
      getSMmass("Z0",       2)
      getSMmass("W+",       2)
      getSMmass("W-",       2)
      getSMmass("g",        2)
      getSMmass("gamma",    2)
      getSMmass("u_3",      1)
      getSMmass("ubar_3",   1)
      getSMmass("d_3",      1)
      getSMmass("dbar_3",   1)
      
      // Pole masses not available for the light quarks.
      addParticle("u_1"   , SMI.mU,  1) // mu(2 GeV)^MS-bar
      addParticle("ubar_1", SMI.mU,  1) // mu(2 GeV)^MS-bar
      addParticle("d_1"   , SMI.mD,  1) // md(2 GeV)^MS-bar
      addParticle("dbar_1", SMI.mD,  1) // md(2 GeV)^MS-bar
      addParticle("u_2"   , SMI.mCmC,1) // mc(mc)^MS-bar
      addParticle("ubar_2", SMI.mCmC,1) // mc(mc)^MS-bar
      addParticle("d_2"   , SMI.mS,  1) // ms(2 GeV)^MS-bar
      addParticle("dbar_2", SMI.mS,  1) // ms(2 GeV)^MS-bar
      
      // Masses for neutrino flavour eigenstates. Set to zero.
      // (presently not required)
      addParticle("nu_e",     0.0, 1)
      addParticle("nubar_e",  0.0, 1)
      addParticle("nu_mu",    0.0, 1)
      addParticle("nubar_mu", 0.0, 1)
      addParticle("nu_tau",   0.0, 1)
      addParticle("nubar_tau",0.0, 1)
      
      // Meson masses
      addParticle("pi0",   meson_masses.pi0,       0)
      addParticle("pi+",   meson_masses.pi_plus,   0)
      addParticle("pi-",   meson_masses.pi_minus,  0)
      addParticle("eta",   meson_masses.eta,       0)
      addParticle("rho0",  meson_masses.rho0,      1)
      addParticle("rho+",  meson_masses.rho_plus,  1)
      addParticle("rho-",  meson_masses.rho_minus, 1)
      addParticle("omega", meson_masses.omega,     1)
      addParticle("K0",    meson_masses.kaon0,     1)

      
      // SubGeVDM-specific masses
      double mDM = spec.get(Par::Pole_Mass, "S");
      addParticle("S", mDM, 1);
      addParticle("S~", mDM, 1);
      addParticle("Ap", spec.get(Par::Pole_Mass, "Ap"), 2);
      
      // Get rid of convenience macros
      #undef getSMmass
      #undef addParticle
      
      // Import decay table from DecayBit
      const DecayTable* tbl = &(*Dep::decay_rates);
      
      std::cout << "OK\n";
      
      // Save dark photon width for later
      double gammaAp = tbl->at("Ap").width_in_GeV;

      std::cout << "Still OK\n";


      // Set of imported decays
      std::set<string> importedDecays;
      
      // Minimum branching ratio to include
      double minBranching = 0;
      
      // Import relevant decays
      using DarkBit_utils::ImportDecays;
      
      auto excludeDecays = daFunk::vec<std::string>("Z0", "W+", "W-", "u_3", "ubar_3", "e+_2", "e-_2", "e+_3", "e-_3","pi0","pi+","pi-","eta","rho0","rho+","rho-","omega","K0");
      
      ImportDecays("Ap", catalog, importedDecays, tbl, minBranching, excludeDecays);
      
      // Instantiate new SubGeVDM object.
      auto pc = boost::make_shared<SubGeVDM_scalar>(&catalog, gammaAp);
          
      // Populate annihilation channel list and add thresholds to threshold list.
      process_ann.resonances_thresholds.threshold_energy.push_back(2*mDM);
      auto channels = 
        daFunk::vec<string>("tautau", "mumu", "ee", "pipi", "bb", "ApAp");
      auto p1 = 
        daFunk::vec<string>("e+_3", "e+_2", "e+_1", "pi+", "dbar_3", "Ap");
      auto p2 = 
        daFunk::vec<string>("e-_3", "e-_2", "e-_1", "pi-", "d_3", "Ap");
      
      for (unsigned int i = 0; i < channels.size(); ++i)
      {
        double mtot_final = 
        catalog.getParticleProperty(p1[i]).mass + 
        catalog.getParticleProperty(p2[i]).mass;  
        if (mDM*2 > mtot_final*0.5)
        {
          daFunk::Funk kinematicFunction = daFunk::funcM(pc, &SubGeVDM_scalar::sv, channels[i], 
          gDM, e*kappa, mDM, daFunk::var("v"));
          TH_Channel new_channel(daFunk::vec<string>(p1[i], p2[i]), kinematicFunction);
          process_ann.channelList.push_back(new_channel);
        }
        if (mDM*2 < mtot_final)
        {
          process_ann.resonances_thresholds.threshold_energy.
          push_back(mtot_final);
        }
      }
      
      // Tell DarkSUSY about dark photon resonance
      if (spec.get(Par::Pole_Mass, "Ap") >= 2*mDM) process_ann.resonances_thresholds.resonances.
          push_back(TH_Resonance(spec.get(Par::Pole_Mass, "Ap"), tbl->at("Ap").width_in_GeV));

      // Tell DarkSUSY about Phi resonance
      double mPhi = 1.02;
      double GammaPhi = 4.25e-3;

      if (mPhi >= 2*mDM) process_ann.resonances_thresholds.resonances.push_back(TH_Resonance(mPhi, GammaPhi));
            
      catalog.processList.push_back(process_ann);
      
      // Validate
      catalog.validate();
      
      result = catalog;
    } // function TH_ProcessCatalog



    /// Direct detection couplings
    void DD_couplings_SubGeVDM_scalar(DM_nucleon_couplings &result)
    {
      using namespace Pipes::DD_couplings_SubGeVDM_scalar;

      double mAp = *Param["mAp"];
      double effective_coupling = sqrt(4*pi*alpha_EM)*(*Param["gDM"])*(*Param["kappa"]);

      result.gps = effective_coupling/pow(mAp,2);
      result.gns = 0;
      result.gpa = 0;
      result.gna = 0;
    }


  }
}
