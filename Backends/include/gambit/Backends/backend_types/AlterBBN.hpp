//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Declarations of containers for BBN
///  calculations.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Janina Renk
///          (janina.renk@fysik.su.se)
///  \date 2019 Mar
///
///  \author Pat Scott
///          (pat.scott@uq.edu.au)
///  \date 2020 Apr
///
///  *********************************************

#pragma once

#include "gambit/Utils/util_types.hpp"

#define NNUCREAC 100
#define NTABMAX 1000

/* Different Versions of AlterBBN have different memebers of relicparam and error structures.
    -> Define the strucutre for each version of AlterBBN in namespace of the according verions to avoid clashes */

namespace Gambit
{

  /// Class to store all results from an AlterBBN run
  /// -> element abundances stored in BBN_nuc (length NNUC+1),
  /// -> covariance matrix in BBN_covmat ( dim NNUC+1 x NNUC+1)
  /// -> abund_map maps name of element to position in BBN_abundance vector
  ///    see constructor of BBN_container
  class BBN_container
  {
    public:
      /// Constructor
      BBN_container();

      /// Initialize sizes of vectors (get NNUC, number of computed element abundances, from AlterBBN)
      void init_arr_size(int);

      /// Initialise the translation map from element name to position in abundance vector
      void set_abund_map(map_str_int);

      /// Setter functions for abundance vector
      void set_BBN_abund(int, double);

      /// Setter function for covariance matrix
      void set_BBN_covmat(int, int, double);

      /// Global parameter in AlterBBN; holds number of computed element abundances
      int get_NNUC() const;

      /// Getter for map from isotope names to position in BBN_abundance vector
      const std::map<str,int>& get_abund_map() const;

      /// Getter for abundance vector
      double get_BBN_abund(int) const;

      /// Getter for covariance matrix
      double get_BBN_covmat(int, int) const;

      /// Setter for active isotopes
      void set_active_isotopes(std::set<str>);

      /// Getter for active isotopes
      const std::set<str>& get_active_isotopes() const;

      /// Getter for indices of active isotopes in BBN_abundance vector
      const std::set<int>& get_active_isotope_indices() const;

    private:
      int NNUC;
      std::vector<double> BBN_abund;
      std::vector<std::vector<double>> BBN_covmat;
      std::map<str,int> abund_map;
      std::set<str> active_isotopes;
      std::set<int> active_isotope_indices;
  };


  namespace AlterBBN_2_0
  {
    /* structure containing the cosmological model parameters */
    struct relicparam
    {
      int entropy_model,energy_model;
      double dd0,ndd,Tdend,Tddeq; // dark density
      double sd0,nsd,Tsend; // dark entropy
      double Sigmad0,nSigmad,TSigmadend; // dark entropy injection
      double Sigmarad0,nSigmarad,TSigmaradend; // standard entropy injection
      double nt0,nnt,Tnend; // non-thermal production of relics

      double quintn2,quintn3,quintn4,quintT12,quintT23,quintT34; // effective quintessence model

      int phi_model; // decaying scalar field model switch
      double eta_phi,Gamma_phi,rhot_phi_Tmax,rho_phi; // eta_phi = b / m_phi
      double rhot_phi0,Tphi0;
      double T_RH;
      double Sigmatildestar;
      double Sigmatildestar_max;
      double Tstdstar_max;

      double mgravitino; // gravitino mass

      double relicmass;
      int scalar;

      int solver; // switch for linear or logarithmic differential equation solver

      double T; // Temperature in GeV
      double Y; // Y=n/s
      double Tfo,Tmax; // Freeze out and maximal temperature

      int full_comput; // Switch to deactivate the fast freeze out temperature determination

      double table_eff[276][3];   // Reads values from the SgStar files

      int use_table_rhoPD;
      double table_rhoPD[2][NTABMAX];
      int size_table_rhoPD;

      /*---------------------*/
      /* AlterBBN parameters */
      /*---------------------*/

      int err;
      int failsafe;
      double eta0;                // Initial Baryon to photon ratio
      double Nnu;                 // Number of Neutrinos (e+- included)
      double dNnu;                // Number of extra neutrinos (delta N_nu)
      double life_neutron,life_neutron_error;     // neutron lifetime
      double xinu1,xinu2,xinu3;   // [e-,neutrino], [muon,neutrino], [tau,neutrino] respectively (degeneracy parameters)
      double m_chi;               // Mass of WIMP
      double g_chi;
      double Tinit;

      int wimp;                   // Switch to enable (1) / disable (0) wimps
      int SMC_wimp;               // wimp coupling to SM particles. 1 for EM, 2 for neutrino, 3 for neut. and eq. neut.
      int selfConjugate;          // 1/0 for self-conjugate/non-self-conjugate WIMP
      int fermion;
      int EM_coupled, neut_coupled, neuteq_coupled;
      double chi2;
      int nobs;
    };

    struct errorparam
    /* structure containing the cosmological model parameters */
    {
      int failsafe;                // failsafe mode
      int errnumber;              // process number for error calculation
      double random[NNUCREAC+2];  // random numbers for Monte Carlo
      double life_neutron;
    };
  }

  namespace AlterBBN_2_1
  {
    /* structure containing the cosmological model parameters */
    struct relicparam
    {
      int entropy_model,energy_model;
      double dd0,ndd,Tdend,Tddeq; // dark density
      double sd0,nsd,Tsend; // dark entropy
      double Sigmad0,nSigmad,TSigmadend; // dark entropy injection
      double Sigmarad0,nSigmarad,TSigmaradend; // standard entropy injection
      double nt0,nnt,Tnend; // non-thermal production of relics
      int coupd; // dark fluid coupling to plasma

      double quintn2,quintn3,quintn4,quintT12,quintT23,quintT34; // effective quintessence model

      int phi_model; // decaying scalar field model switch
      double eta_phi,Gamma_phi,rhot_phi_Tmax,n_phi; // eta_phi = b / m_phi
      double rhot_phi0,Tphi0;
      double T_RH;
      double Sigmatildestar;
      double Sigmatildestar_max;
      double Tstdstar_max;

      double mgravitino; // gravitino mass

      double relicmass;
      int scalar;

      int solver; // switch for linear or logarithmic differential equation solver
      int beta_samples;

      double Tfo,Tmax; // Freeze out and maximal temperature

      int full_comput; // Switch to deactivate the fast freeze out temperature determination

      double table_eff[276][3];   // Reads values from the SgStar files

      int use_table_rhoPD;
      double table_rhoPD[2][NTABMAX];
      int size_table_rhoPD;

      /*---------------------*/
      /* AlterBBN parameters */
      /*---------------------*/

      int err;
      int failsafe;                // Switch for the integration method
      double eta0;                // Initial Baryon to photon ratio
      double Nnu;                 // Number of Neutrinos (e+- included)
      double dNnu;                // Number of extra neutrinos (delta N_nu)
      double life_neutron,life_neutron_error;      // neutron lifetime
      double xinu1,xinu2,xinu3;    // [e-,neutrino], [muon,neutrino], [tau,neutrino] respectively (degeneracy parameters)
      double m_chi;               // Mass of WIMP
      double g_chi;
      double Tinit;                // Initial temperature
      double Tnudec;              // Neutrino decoupling temperature
      int wimp;                   // Switch to enable (1) / disable (0) wimps
      int SMC_wimp;               // wimp coupling to SM particles. 1 for EM, 2 for neutrino, 3 for neutrino and equivalent neutrino
      int selfConjugate;          // 1/0 for self-conjugate/non-self-conjugate WIMP
      int fermion;
      int EM_coupled, neut_coupled, neuteq_coupled;
      double chi2;
      int nobs;
      double fierz;               // Fierz interference term from LQ sector
      double B_chi;               // branching ratio of WIMP DM of mass m_p < m_chi < m_n to explain tau_n anomaly
      double rhob0;               // current baryon density
      double b_cdm_ratio;
    };

    struct errorparam
    /* structure containing the cosmological model parameters */
    {
      int errnumber;              // process number for error calculation
      double random[NNUCREAC+2];  // random numbers for Monte Carlo
      double life_neutron;
    };
  }

  namespace AlterBBN_2_2
  {
    /* structure containing the cosmological model parameters */
    struct relicparam
    {
      int entropy_model,energy_model;
      double dd0,ndd,Tdend,Tddeq; // dark density
      double sd0,nsd,Tsend; // dark entropy
      double Sigmad0,nSigmad,TSigmadend; // dark entropy injection
      double Sigmarad0,nSigmarad,TSigmaradend; // standard entropy injection
      double nt0,nnt,Tnend; // non-thermal production of relics
      int coupd; // dark fluid coupling to plasma

      double quintn2,quintn3,quintn4,quintT12,quintT23,quintT34; // effective quintessence model

      int phi_model; // decaying scalar field model switch
      double eta_phi,Gamma_phi,rhot_phi_Tmax,n_phi; // eta_phi = b / m_phi
      double rhot_phi0,Tphi0;
      double T_RH;
      double Sigmatildestar;
      double Sigmatildestar_max;
      double Tstdstar_max;

      double mgravitino; // gravitino mass

      double relicmass;
      int scalar;

      int solver; // switch for linear or logarithmic differential equation solver
      int beta_samples;

      double Tfo,Tmax; // Freeze out and maximal temperature

      int full_comput; // Switch to deactivate the fast freeze out temperature determination

      double table_eff[276][3];   // Reads values from the SgStar files

      int use_table_rhoPD;
      double table_rhoPD[2][NTABMAX];
      int size_table_rhoPD;

      /*---------------------*/
      /* AlterBBN parameters */
      /*---------------------*/

      int err;
      int failsafe;               // Switch for the integration method
      double eta0;                // Initial Baryon to photon ratio
      double Nnu;                 // Number of Neutrinos (e+- included)
      double dNnu;                // Number of extra neutrinos (delta N_nu)
      double life_neutron,life_neutron_error;      // neutron lifetime
      double xinu1,xinu2,xinu3;   // [e-,neutrino], [muon,neutrino], [tau,neutrino] respectively (degeneracy parameters)
      double m_chi;               // Mass of WIMP
      double g_chi;               // dof of WIMP
      double Tinit;               // Initial temperature
      double Tnudec;              // Neutrino decoupling temperature
      int wimp;                   // Switch to enable (1) / disable (0) wimps
      int SMC_wimp;               // wimp coupling to SM particles. 1 for EM, 2 for neutrino, 3 for neutrino and equivalent neutrino
      int selfConjugate;          // 1/0 for self-conjugate/non-self-conjugate WIMP
      int fermion;
      int EM_coupled, neut_coupled, neuteq_coupled;
      double fierz;               // Fierz interference term from LQ sector
      double B_chi;               // branching ratio of WIMP DM of mass m_p < m_chi < m_n to explain tau_n anomaly
      double rhob0;               // current baryon density
      double b_cdm_ratio;         // current ratio of baryon density to cold dark matter density
      int constraints;            // 1=Yp, 2=+H2/H, 3=+Li7/H, 4=+He3/H
    };

    struct errorparam
    /* structure containing the cosmological model parameters */
    {
      int errnumber;              // process number for error calculation
      double random[NNUCREAC+2];  // random numbers for Monte Carlo
      double life_neutron;
    };
  }


  // Make sure that the frontend macros don't get confused by the two
  // different version-specific namespaces when trying to find the types.
  namespace Backends
  {
    namespace AlterBBN_2_0
    {
      typedef Gambit::AlterBBN_2_0::relicparam relicparam;
      typedef Gambit::AlterBBN_2_0::errorparam errorparam;
    }
    namespace AlterBBN_2_1
    {
      typedef Gambit::AlterBBN_2_1::relicparam relicparam;
      typedef Gambit::AlterBBN_2_1::errorparam errorparam;
    }
    namespace AlterBBN_2_2
    {
      typedef Gambit::AlterBBN_2_2::relicparam relicparam;
      typedef Gambit::AlterBBN_2_2::errorparam errorparam;
    }
  }


}

