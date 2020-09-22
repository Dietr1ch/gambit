//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Fronted header for the DarkSUSY backend
///
///  Compile-time registration of available
///  functions and variables from this backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Torsten Bringmann
///          (torsten.bringmann@fys.uio.no)
///  \date 2018 September, 2019
///
///  \author Joakim Edsjo
///          (edsjo@fysik.su.se)
///  \date 2018 September
///
///  *********************************************

#define BACKENDNAME DarkSUSY_MSSM
#define BACKENDLANG FORTRAN
#define VERSION 6.2.2
#define SAFE_VERSION 6_2_2

// Load the library
LOAD_LIBRARY

BE_ALLOW_MODELS(MSSM63atQ)

// Functions used in DS frontend
BE_FUNCTION(dsinit, void, (), "dsinit_", "dsinit")
BE_FUNCTION(dsSLHAread, void, (const char*, int), "dsgive_model_SLHA_", "dsgive_model_SLHA")
BE_FUNCTION(dsmodelsetup, void, (int&,int&), "dsmodelsetup_", "dsmodelsetup")
BE_FUNCTION(dsmssmzero, void, (), "dsmssmzero_", "dsmssmzero")
BE_FUNCTION(dsvertx, void, (), "dsvertx_", "dsvertx")
BE_FUNCTION(dsorder_flavour, void, (), "dsorder_flavour_", "dsorder_flavour")
BE_FUNCTION(dshigwid, void, (), "dshigwid_", "dshigwid")
BE_FUNCTION(dsspwid, void, (), "dsspwid_", "dsspwid")
BE_FUNCTION(dsfindmtmt, void, (), "dsfindmtmt_", "dsfindmtmt")
BE_FUNCTION(dssuconst_ckm, void, (), "dssuconst_ckm_", "dssuconst_ckm")
BE_FUNCTION(dssuconst_yukawa_running, void, (), "dssuconst_yukawa_running_", "dssuconst_yukawa_running")
BE_FUNCTION(dssuconst_yukawa, void, (), "dssuconst_yukawa_", "dssuconst_yukawa")
BE_FUNCTION(dssuconst_higgs, void, (), "dssuconst_higgs_", "dssuconst_higgs")
BE_FUNCTION(dsmqpole4loop, double, (int&, double&), "dsmqpole4loop_", "dsmqpole4loop")
BE_FUNCTION(dsgf2s2thw, double, (const double&, const double&, const double&, const double&, const int&), "dsgf2s2thw_", "dsgf2s2thw")

 // Functions only needed in debug mode
BE_FUNCTION(dswspectrum, void, (int&), "dswspectrum_", "dswspectrum")
BE_FUNCTION(dswwidth, void, (int&), "dswwidth_", "dswwidth")

// Functions used by convenience functions
BE_FUNCTION(dsseyield_sim_ls, double, (const double&, const double&, const double&, const int&, const int&, const int&, const int&, const int&, const int&, const char*, const int&, const int&, const int&), "dsseyield_sim_ls_", "raw_nuyield_sim")
BE_FUNCTION(dsseyield_ch, double, (const double&, const double&, const double&, const int&, const int&, const char*, const int&, const int&, const int&), "dsseyield_ch_", "raw_nuyield_casc")
BE_FUNCTION(dsddgpgn, void, (Farray<Fcomplex16,1,27,1,2>&, int&), "dsddgpgn_", "dsddgpgn")


// Functions used in MSSM.cpp
BE_FUNCTION(dsgive_model_isasugra, void, (double&,double&,double&,double&,double&), "dsgive_model_isasugra_", "dsgive_model_isasugra")
BE_FUNCTION(dssigmav0, double, (int&,int&), "dssigmav0_", "dssigmav0")
BE_FUNCTION(dssigmav0tot, double, (), "dssigmav0tot_", "dssigmav0tot")
BE_FUNCTION(dsIBffdxdy, double, (int&, double&, double&), "dsibffdxdy_", "dsIBffdxdy")
BE_FUNCTION(dsIBfsrdxdy, double, (int&, double&, double&), "dsibfsrdxdy_", "dsIBfsrdxdy")
BE_FUNCTION(dsIBhhdxdy, double, (int&, double&, double&), "dsibhhdxdy_", "dsIBhhdxdy")
BE_FUNCTION(dsIBwhdxdy, double, (int&, double&, double&), "dsibwhdxdy_", "dsIBwhdxdy")
BE_FUNCTION(dsIBwwdxdy, double, (int&, double&, double&), "dsibwwdxdy_", "dsIBwwdxdy")

// Functions used in RelicDensity.cpp
BE_FUNCTION(dsanwx, double, (double&), "dsanwx_", "dsanwx")
BE_FUNCTION(dsrdcom, void, (), "dsrdcom_", "dsrdcom")
BE_FUNCTION(dsrdstart, void, (int&, double(&)[1000], double(&)[1000], int&, double(&)[1000], double(&)[1000], int&, double(&)[1000]), "dsrdstart_", "dsrdstart")
BE_FUNCTION(dsrdens, void, (double(*)(double&),double&,double&,int&,int&,int&), "dsrdens_", "dsrdens")

// Functions used in GamYields.cpp
BE_FUNCTION(dsanyield_sim, double, (double&,double&,int&,char*,int&,int&,int&), "dsanyield_sim_", "dsanyield_sim")

// Functions used in SunNeutrinos.cpp
BE_FUNCTION(dssenu_capsuntab, double, (const double&, const double&, const double&, const double&), "dssenu_capsuntab_", "cap_Sun_v0q0_isoscalar_DS")

// Halo model common blocks
BE_VARIABLE(dshmcom, DS_HMCOM, "dshmcom_", "dshmcom")
BE_VARIABLE(dshmframevelcom, DS_HMFRAMEVELCOM, "dshmframevelcom_", "dshmframevelcom")
BE_VARIABLE(dshmisodf, DS_HMISODF, "dshmisodf_", "dshmisodf")
BE_VARIABLE(dshmnoclue, DS_HMNOCLUE, "dshmnoclue_", "dshmnoclue")

// Common blocks in the DarkSUSY core library
BE_VARIABLE(ddcomlegacy, DS_DDCOMLEGACY, "ddcomlegacy_", "ddcomlegacy") //DD
BE_VARIABLE(rdtime, DS_RDTIME,     "rdtime_",    "rdtime")    // RD timeout
BE_VARIABLE(rdpars, DS_RDPARS,     "rdpars_",    "rdpars")    // gRD Parameters

// Common blocks in the MSSM module library
BE_VARIABLE(smquarkmasses, DS_SMQUARKMASSES, "smquarkmasses_", "smquarkmasses")
BE_VARIABLE(smcoupling, DS_SMCOUPLING, "smcoupling_", "smcoupling")
BE_VARIABLE(pmasses, DS_PMASSES, "pmasses_", "pmasses")
BE_VARIABLE(pwidths, DS_PWIDTHS, "pwidths_", "pwidths")
BE_VARIABLE(mixing, DS_SCKM, "sckm_", "mixing")
// The part that is genuinely MSSM-specific (and not SM/generic) only starts here
BE_VARIABLE(pacodes_mssm, DS_PACODES_MSSM, "pacodes_mssm_", "pacodes_mssm")
BE_VARIABLE(mssmmixing, DS_MSSMMIXING, "mssmmixing_", "mssmmixing")
BE_VARIABLE(couplingconstants, DS_COUPLINGCONSTANTS, "couplingconstants_", "couplingconstants")
BE_VARIABLE(mssmiuseful, DS_MSSMIUSEFUL, "mssmiuseful_", "mssmiuseful")
BE_VARIABLE(mssmtype, DS_MSSMTYPE, "mssmtype_", "mssmtype")
BE_VARIABLE(mssmpar, DS_MSSMPAR, "mssmpar_", "mssmpar")
BE_VARIABLE(mssmwidths, DS_MSSMWIDTHS, "mssmwidths_", "mssmwidths")
BE_VARIABLE(IBintvars, DS_IBINTVARS,"ibintvars_", "IBintvars")
BE_VARIABLE(anbranch,DS_NUCOM, "anbranch_", "nu_common_block") // BRs
BE_VARIABLE(ddmssmcom, DS_DDMSSMCOM, "ddmssmcom_", "ddmssmcom") //DD
BE_VARIABLE(dsancoann, DS_DSANCOANN, "dsancoann_", "dsancoann") // RD

// Convenience functions (registration)
BE_CONV_FUNCTION(DD_couplings, std::vector<double>, (), "get_DD_couplings")
BE_CONV_FUNCTION(DSparticle_code, int, (const str&), "DSparticle_code")
BE_CONV_FUNCTION(DS_neutral_h_decay_channels, std::vector<std::vector<str>>, (), "get_DS_neutral_h_decay_channels")
BE_CONV_FUNCTION(DS_charged_h_decay_channels, std::vector<std::vector<str>>, (), "get_DS_charged_h_decay_channels")
BE_CONV_FUNCTION(neutrino_yield, double, (const double&, const int&, void*&), "nuyield")
BE_CONV_FUNCTION(dsgenericwimp_nusetup, void, (const double(&)[29], const double(&)[29][3], const double(&)[15], const double(&)[3], const double&, const double&), "DS_nuyield_setup")
BE_CONV_FUNCTION(init_diskless, int, (const SLHAstruct&, const DecayTable&), "initFromSLHAeaAndDecayTable")

// Model-conditional dependencies of BE_INI 
BE_INI_CONDITIONAL_DEPENDENCY(MSSM_spectrum, Spectrum, MSSM63atQ, CMSSM)
BE_INI_CONDITIONAL_DEPENDENCY(decay_rates, DecayTable, MSSM63atQ, CMSSM)

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
