//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend for SUSY-HIT 1.5 backend
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
/// \author Pat Scott
/// \date 2015 Jan-May
///
///  *********************************************

#include <sstream>

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/SUSY-HIT.hpp"
#include "gambit/Elements/mssm_slhahelp.hpp"
#include "gambit/Elements/slhaea_helpers.hpp"


// Convenience functions (definitions)
BE_NAMESPACE
{

  /// Simple helper function for initialisation function, for adding missing SLHA1 2x2 family mixing matrices.
  void attempt_to_add(const str& block, SLHAstruct& slha, const str& type, const SubSpectrum* spec, double tol, str& s1, str& s2, bool pterror)
  {
    if (slha.find(block) == slha.end())
    {
      std::vector<double> matmix = slhahelp::family_state_mix_matrix(type, 3, s1, s2, spec, tol, LOCAL_INFO, pterror);
      SLHAea_add_matrix(slha, block, matmix, 2, 2);
    }
    else
    {
      s1 = slhahelp::mass_es_closest_to_family(s1, spec, tol, LOCAL_INFO, pterror);
      s2 = slhahelp::mass_es_closest_to_family(s2, spec, tol, LOCAL_INFO, pterror);
    }
  }

  /// Some SUSY-HIT-specific shortcuts for dealing with SLHA blocks
  /// @{
  void optional_block(const str& name, SLHAea::Block& block, const SLHAstruct& slha)
  {
    if (slha.find(name) != slha.end()) block = slha.at(name);
    else block.push_back ("BLOCK " + name);
  }

  void required_block(const str& name, SLHAea::Block& block, const SLHAea::Coll& slha)
  {
    if (slha.find(name) != slha.end()) block = slha.at(name);
    else backend_error().raise(LOCAL_INFO, "Sorry, SUSY-HIT needs SLHA block: " + name + ".\n"
    "If you tried to read in a debug SLHA file with missing entries (e.g. STOPMIX, STAUMIX,\n"
    "etc), then sort out your SLHA file so that it is readable by SUSY-HIT!  If you can't  \n"
    "do that, then depending on what blocks your file has, you may be able to instead use  \n"
    "it to initialise either                                                               \n"
    " 1. a GAMBIT Spectrum object (see SpecBit::get_MSSM_spectrum_from_SLHAfile), or       \n"
    " 2. a GAMBIT DecayTable object (see DecayBit::all_decays_from_SLHA).                  \n"
    "In the first case, the resulting Spectrum object will get passed through to the SUSY- \n"
    "HIT initialisation routine.  GAMBIT will then use the file to create the family mixing\n"
    "matrices, using its own spectrum manipulation routines. In the second case, GAMBIT    \n"
    "will bypass SUSY-HIT, taking the decays directly from the SLHA file that you provide.");
  }
  /// @}

  /// Runs actual SUSY-HIT decay calculations.
  /// Inputs: m_s_1GeV_msbar    strange mass in GeV, in MSbar scheme at an energy of 1GeV
  ///         W_width, Z_width  EW gauge boson total widths in GeV
  void run_susy_hit(SLHAstruct slha, double W_width, double Z_width, int pdg_codes[35])
  {
    using SLHAea::to;

    // Zero all SLHA2 Q values
    for (int i=1; i<=22; ++i) sd_leshouches2->qvalue(i) = 0.0;

    // Get SLHA2 blocks
    SLHAea::Block sminputs, vckm, msoft, mass, nmix, vmix, umix;
    SLHAea::Block stopmix, sbotmix, staumix, alpha, hmix, gauge;
    SLHAea::Block au, ad, ae, yu, yd, ye, msq2, msd2, msu2, td, tu;
    SLHAea::Block usqmix, dsqmix, selmix, spinfo;
    required_block("SMINPUTS", sminputs, slha);
    required_block("VCKMIN",   vckm,     slha);
    required_block("MSOFT",    msoft,    slha);
    required_block("MASS",     mass,     slha);
    required_block("NMIX",     nmix,     slha);
    required_block("VMIX",     vmix,     slha);
    required_block("UMIX",     umix,     slha);
    required_block("ALPHA",    alpha,    slha);
    required_block("HMIX",     hmix,     slha);
    required_block("GAUGE",    gauge,    slha);
    required_block("SPINFO",   spinfo,   slha);
    required_block("STOPMIX",  stopmix,  slha);
    required_block("SBOTMIX",  sbotmix,  slha);
    required_block("STAUMIX",  staumix,  slha);
    required_block("YU",       yu,       slha);
    required_block("YD",       yd,       slha);
    required_block("YE",       ye,       slha);
    optional_block("AU",       au,       slha);
    optional_block("AD",       ad,       slha);
    optional_block("AE",       ae,       slha);
    optional_block("MSQ2",     msq2,     slha);
    optional_block("MSD2",     msd2,     slha);
    optional_block("MSU2",     msu2,     slha);
    optional_block("TD",       td,       slha);
    optional_block("TU",       tu,       slha);
    optional_block("USQMIX",   usqmix,   slha);
    optional_block("DSQMIX",   dsqmix,   slha);
    optional_block("SELMIX",   selmix,   slha);

    // SMINPUTS
    for (int i=1; i<=14; ++i)
    {
      sd_leshouches2->smval(i) = (sminputs[i].is_data_line()) ? to<double>(sminputs[i][1]) : 0.0;
    }
    for (int i=15; i<=20; ++i) sd_leshouches2->smval(i) = 0.0;     // zeroing

    // SUSY-HIT and HDecay non-SLHA inputs
    susyhitin->amsin = to<double>(sminputs.at(23).at(1));          // MSBAR(1): HDECAY claims it wants ms(1GeV)^MSbar, but we don't believe it, and give it m_s(2GeV)^MSBar
    susyhitin->amcin = to<double>(sminputs.at(24).at(1));          // MC: HDECAY claims it wants the c pole mass, but that is not well defined, so we give it mc(mc)^MSBar
    susyhitin->ammuonin = sd_leshouches2->smval(11);               // MMUON: mmu(pole)
    susyhitin->alphin = sd_leshouches2->smval(1);                  // ALPHA: alpha_em^-1(M_Z)^MSbar (scheme and scale not specified in SUSYHIT or HDECAY documentation)
    susyhitin->gamwin = W_width;                                   // GAMW: W width (GeV)
    susyhitin->gamzin = Z_width;                                   // GAMZ: Z width (GeV)
    double lambda = to<double>(vckm.at(1).at(1));                  // Wolfenstein lambda
    double A = to<double>(vckm.at(2).at(1));                       // Wolfenstein A
    double rhobar = to<double>(vckm.at(3).at(1));                  // Wolfenstein rhobar
    double etabar = to<double>(vckm.at(4).at(1));                  // Wolfenstein etabar
    susyhitin->vusin = Spectrum::Wolf2V_us(lambda, A, rhobar, etabar); // VUS: |CKM V_us|
    susyhitin->vcbin = Spectrum::Wolf2V_cb(lambda, A, rhobar, etabar); // VCB: |CKM V_cb|
    susyhitin->rvubin = std::abs(Spectrum::Wolf2V_ub(lambda, A, rhobar, etabar)) / susyhitin->vcbin; // VUB/VCB: Ratio of CKM |V_UB|/|V_CB|

    // MODSEL
    sd_leshouches2->imod(1) = 1;    // 1, x = SUSY model info.
    sd_leshouches2->imod(2) = 0;    // 0 = general MSSM, 1 = SUGRA => do not calculate metastable NLSP decays to gravitinos.
    //sd_leshouches2->imod(2) = 2;  // 2 = GMSB => calculate metastable NLSP decays to gravitinos.  Not yet implemented in GAMBIT.
    checkfavvio->imodfav(1) = 6;    // 6, x =  Flavour violation info.
    checkfavvio->imodfav(2) = 0;    // 0 = no FV, 1 = q FV, 2 = l FV, 3 = both.  For FV decays, must be set non-zero later before calling sdecay() again.  Not yet implemented in GAMBIT.

    // MSOFT
    for (int i=1; i<=100; ++i) sd_leshouches2->msoftval(i) = unlikely(); // indicate undefined
    if (msoft.find_block_def()->size() >= 4) sd_leshouches2->qvalue(3) = to<double>(msoft.find_block_def()->at(3)); // Q(GeV)
    int msoft_indices[20] = {1, 2, 3, 21, 22, 31, 32, 33, 34, 35, 36, 41, 42, 43, 44, 45, 46, 47, 48, 49};
    // M_1, M_2, M_3, M^2_Hd, M^2_Hu, M_eL, M_muL, M_tauL, M_eR, M_muR, M_tauR, M_q1L, M_q2L, M_q3L, M_uR, M_cR, M_tR, M_dR, M_sR, M_bR
    for (int i : msoft_indices)
    {
      if (msoft[i].is_data_line()) sd_leshouches2->msoftval(i) = to<double>(msoft.at(i).at(1));
    }

    // SLHA2 block EXTPAR
    sd_leshouches2->extval(0) = sd_leshouches2->qvalue(3);         // EWSB scale (set to SUSY scale as per MSOFT).  Not used by SUSY-HIT anymore.

    // SLHA2 block MASS
    for (int i=1; i<=35; ++i)
    {
      sd_leshouches2->massval(i) = (mass[pdg_codes[i-1]].is_data_line()) ? to<double>(mass[pdg_codes[i-1]][1]) : unlikely();
    }
    for (int i=36; i<=50; ++i) sd_leshouches2->massval(i) = 0.0; // zeroing

    // NMIX
    for (int i=1; i<=4; ++i) for (int j=1; j<=4; ++j) sd_leshouches2->nmixval(i,j) = (nmix[initVector<int>(i,j)].is_data_line()) ? to<double>(nmix.at(i,j)[2]) : 0.0;

    // VMIX, UMIX, STOPMIX, SBOTMIX, STAUMIX
    for (int i=1; i<=2; ++i)
    {
      for (int j=1; j<=2; ++j)
      {
        std::vector<int> ij = initVector<int>(i,j);
        sd_leshouches2->vmixval(i,j) = (vmix[ij].is_data_line()) ? to<double>(vmix.at(i,j)[2]) : 0.0;
        sd_leshouches2->umixval(i,j) = (umix[ij].is_data_line()) ? to<double>(umix.at(i,j)[2]) : 0.0;
        sd_leshouches2->stopmixval(i,j) = (stopmix[ij].is_data_line()) ? to<double>(stopmix.at(i,j)[2]) : 0.0;
        sd_leshouches2->sbotmixval(i,j) = (sbotmix[ij].is_data_line()) ? to<double>(sbotmix.at(i,j)[2]) : 0.0;
        sd_leshouches2->staumixval(i,j) = (staumix[ij].is_data_line()) ? to<double>(staumix.at(i,j)[2]) : 0.0;
      }
    }

    // ALPHA (value is spectrum generator's "best choice" => can be on-shell, DRbar at a give scale, whatever)
    sd_leshouches2->alphaval = to<double>(alpha.back().at(0));             // Mixing angle in the neutral Higgs boson sector.

    // HMIX
    if (hmix.find_block_def()->size() >= 4) sd_leshouches2->qvalue(1) = to<double>(hmix.find_block_def()->at(3));  // Q(GeV)
    for (int i=1; i<=10; ++i) sd_leshouches2->hmixval(i) = (hmix[i].is_data_line()) ? to<double>(hmix[i][1]) : unlikely();

    // GAUGE
    if (gauge.find_block_def()->size() >= 4) sd_leshouches2->qvalue(2) = to<double>(gauge.find_block_def()->at(3));// Q(GeV)
    sd_leshouches2->gaugeval(1) = to<double>(gauge.at(1).at(1));                                                   // gprime(Q) DRbar
    sd_leshouches2->gaugeval(2) = to<double>(gauge.at(2).at(1));                                                   // g(Q) DRbar
    sd_leshouches2->gaugeval(3) = to<double>(gauge.at(3).at(1));                                                   // g_3(Q) DRbar

    // AU, AD, AE, YU, YD, YE, MSQ2, MSD2, MSU2, TD, TU, VCKM
    if (au.find_block_def()->size() >= 4) sd_leshouches2->qvalue(4)  = to<double>(au.find_block_def()->at(3));     // Q(GeV)
    if (ad.find_block_def()->size() >= 4) sd_leshouches2->qvalue(5)  = to<double>(ad.find_block_def()->at(3));     // Q(GeV)
    if (ae.find_block_def()->size() >= 4) sd_leshouches2->qvalue(6)  = to<double>(ae.find_block_def()->at(3));     // Q(GeV)
    if (yu.find_block_def()->size() >= 4) sd_leshouches2->qvalue(7)  = to<double>(yu.find_block_def()->at(3));     // Q(GeV)
    if (yd.find_block_def()->size() >= 4) sd_leshouches2->qvalue(8)  = to<double>(yd.find_block_def()->at(3));     // Q(GeV)
    if (ye.find_block_def()->size() >= 4) sd_leshouches2->qvalue(9)  = to<double>(ye.find_block_def()->at(3));     // Q(GeV)
    if (msq2.find_block_def()->size() >= 4) sd_leshouches2->qvalue(17) = to<double>(msq2.find_block_def()->at(3)); // Q(GeV) corrects minor bug in SUSY-HIT SLHA reader
    if (msd2.find_block_def()->size() >= 4) sd_leshouches2->qvalue(10) = to<double>(msd2.find_block_def()->at(3)); // Q(GeV)
    if (msu2.find_block_def()->size() >= 4) sd_leshouches2->qvalue(11) = to<double>(msu2.find_block_def()->at(3)); // Q(GeV)
    if (td.find_block_def()->size() >= 4) sd_leshouches2->qvalue(12) = to<double>(td.find_block_def()->at(3));     // Q(GeV)
    if (tu.find_block_def()->size() >= 4) sd_leshouches2->qvalue(13) = to<double>(tu.find_block_def()->at(3));     // Q(GeV)
    if (vckm.find_block_def()->size() >= 4) sd_leshouches2->qvalue(21) = to<double>(vckm.find_block_def()->at(3)); // Q(GeV)
    for (int i=1; i<=3; ++i)
    {
      for (int j=1; j<=3; ++j)
      {
        std::vector<int> ij = initVector<int>(i,j);
        sd_leshouches2->auval(i,j) = (au[ij].is_data_line()) ? to<double>(au.at(i,j)[2]) : unlikely();
        sd_leshouches2->adval(i,j) = (ad[ij].is_data_line()) ? to<double>(ad.at(i,j)[2]) : unlikely();
        sd_leshouches2->aeval(i,j) = (ae[ij].is_data_line()) ? to<double>(ae.at(i,j)[2]) : unlikely();
        sd_leshouches2->yuval(i,j) = (yu[ij].is_data_line()) ? to<double>(yu.at(i,j)[2]) : 0.0;
        sd_leshouches2->ydval(i,j) = (yd[ij].is_data_line()) ? to<double>(yd.at(i,j)[2]) : 0.0;
        sd_leshouches2->yeval(i,j) = (ye[ij].is_data_line()) ? to<double>(ye.at(i,j)[2]) : 0.0;
        flavviolation->msq2(i,j) = (msq2[ij].is_data_line()) ? to<double>(msq2.at(i,j)[2]) : 0.0;
        flavviolation->msd2(i,j) = (msd2[ij].is_data_line()) ? to<double>(msd2.at(i,j)[2]) : 0.0;
        flavviolation->msu2(i,j) = (msu2[ij].is_data_line()) ? to<double>(msu2.at(i,j)[2]) : 0.0;
        flavviolation->td(i,j) = (td[ij].is_data_line()) ? to<double>(td.at(i,j)[2]) : 0.0;
        flavviolation->tu(i,j) = (tu[ij].is_data_line()) ? to<double>(tu.at(i,j)[2]) : 0.0;
        flavviolation->vckm(i,j) = 0.0;
      }
    }
    // The following is only relevant if considering FV stop decays.  Code below is tested and ready to be used in future.
    //flavviolation->vckm(1,1) =          Spectrum::Wolf2V_ud(lambda, A, rhobar, etabar);
    //flavviolation->vckm(1,2) =          Spectrum::Wolf2V_us(lambda, A, rhobar, etabar);
    //flavviolation->vckm(1,3) = std::abs(Spectrum::Wolf2V_ud(lambda, A, rhobar, etabar));
    //flavviolation->vckm(2,1) = std::abs(Spectrum::Wolf2V_cb(lambda, A, rhobar, etabar));
    //flavviolation->vckm(2,2) = std::abs(Spectrum::Wolf2V_cs(lambda, A, rhobar, etabar));
    //flavviolation->vckm(2,3) =          Spectrum::Wolf2V_cb(lambda, A, rhobar, etabar);
    //flavviolation->vckm(3,1) = std::abs(Spectrum::Wolf2V_td(lambda, A, rhobar, etabar));
    //flavviolation->vckm(3,2) = std::abs(Spectrum::Wolf2V_ts(lambda, A, rhobar, etabar));
    //flavviolation->vckm(3,3) =          Spectrum::Wolf2V_tb(lambda, A, rhobar, etabar);

    // USQMIX, DSQMIX, SELMIX
    if (usqmix.find_block_def()->size() >= 4) sd_leshouches2->qvalue(14) = to<double>(usqmix.find_block_def()->at(3)); // Q(GeV)
    if (dsqmix.find_block_def()->size() >= 4) sd_leshouches2->qvalue(15) = to<double>(dsqmix.find_block_def()->at(3)); // Q(GeV) corrects minor bug in SUSY-HIT SLHA reader.
    if (selmix.find_block_def()->size() >= 4) sd_leshouches2->qvalue(16) = to<double>(selmix.find_block_def()->at(3)); // Q(GeV) corrects minor bug in SUSY-HIT SLHA reader.
    for (int i=1; i<=6; ++i)
    {
      for (int j=1; j<=6; ++j)
      {
        std::vector<int> ij = initVector<int>(i,j);
        flavviolation->usqmix(i,j) = (usqmix[ij].is_data_line()) ? to<double>(usqmix.at(i,j)[2]) : 0.0;
        flavviolation->dsqmix(i,j) = (dsqmix[ij].is_data_line()) ? to<double>(dsqmix.at(i,j)[2]) : 0.0;
        sd_selectron->selmix(i,j)  = (selmix[ij].is_data_line()) ? to<double>(selmix.at(i,j)[2]) : 0.0;
      }
    }

    // SLHA1 block SPINFO
    sd_leshouches1->spinfo1 = (spinfo[1].is_data_line()) ? spinfo[1][1] : ""; // RGE +Spectrum calculator
    sd_leshouches1->spinfo2 = (spinfo[2].is_data_line()) ? spinfo[2][1] : ""; // version number

    // Tell SUSY-HIT not to bother calculating the b pole mass from mb(mb)_MSbar, just use the value we pass it.
    sd_mbmb->i_sd_mbmb = 1;

    // Do calculation without flavour-violating light stop decays.
    flavviolation->ifavvio = 0;

    // Note to developers interested in expanding this to use the v1.5 flavor-violating stop decay features
    // ------------
    // In order to turn FV on, some time after the initial call to sdecay() at the end of this function, you
    // need to set
    //  flavviolation->ifavvio = 1;
    //  checkfavvio->imodfav(1) = 6;
    //  checkfavvio->imodfav(2) = 1;
    // and then call sdecay() again.  You might do that somehow in this function, or in a module function.
    // There may be a smart way to order this so that it happens automatically if and when you want it.
    // However, before spending time automating this, it still needs to be tested that running first without
    // flavour violation and then re-running with flavour violation actually works, i.e. it does not break
    // the non-FV results.  The SUSY-HIT authors think it should be OK.

    // Set equivalent SLHA common blocks for HDecay.  Only differences are dimension of qvalues and zero vs unlikely for au, ad & ae.
    *slha_leshouches1_hdec = *sd_leshouches1;                       // SLHA1 block is identical in SDECAY and HDECAY.
    slha_leshouches2_hdec->imod = sd_leshouches2->imod;             // model; 1, 1 = SUGRA.
    slha_leshouches2_hdec->smval = sd_leshouches2->smval;           // SMINPUTS
    slha_leshouches2_hdec->extval = sd_leshouches2->extval;         // EXTPAR
    slha_leshouches2_hdec->massval = sd_leshouches2->massval;       // MASS
    slha_leshouches2_hdec->nmixval = sd_leshouches2->nmixval;       // NMIX
    slha_leshouches2_hdec->vmixval = sd_leshouches2->vmixval;       // VMIX
    slha_leshouches2_hdec->umixval = sd_leshouches2->umixval;       // UMIX
    slha_leshouches2_hdec->stopmixval = sd_leshouches2->stopmixval; // STOPMIX
    slha_leshouches2_hdec->sbotmixval = sd_leshouches2->sbotmixval; // SBOTMIX
    slha_leshouches2_hdec->staumixval = sd_leshouches2->staumixval; // STAUMIX
    slha_leshouches2_hdec->alphaval = sd_leshouches2->alphaval;     // ALPHA
    slha_leshouches2_hdec->hmixval = sd_leshouches2->hmixval;       // HMIX
    slha_leshouches2_hdec->gaugeval = sd_leshouches2->gaugeval;     // GAUGE
    slha_leshouches2_hdec->msoftval = sd_leshouches2->msoftval;     // MSOFT
    slha_leshouches2_hdec->yuval = sd_leshouches2->yuval;           // YU
    slha_leshouches2_hdec->ydval = sd_leshouches2->ydval;           // YD
    slha_leshouches2_hdec->yeval = sd_leshouches2->yeval;           // YE
    for (int i=1; i<=3; ++i) for (int j=1; j<=3; ++j)
    {
      if (sd_leshouches2->auval(i,j) == unlikely())                 // AU
      {
        slha_leshouches2_hdec->auval(i,j) = 0.0;
      }
      else
      {
       slha_leshouches2_hdec->auval(i,j) = sd_leshouches2->auval(i,j);
      }
      if (sd_leshouches2->adval(i,j) == unlikely())                 // AD
      {
        slha_leshouches2_hdec->adval(i,j) = 0.0;
      }
      else
      {
       slha_leshouches2_hdec->adval(i,j) = sd_leshouches2->adval(i,j);
      }
      if (sd_leshouches2->aeval(i,j) == unlikely())                 // AE
      {
        slha_leshouches2_hdec->aeval(i,j) = 0.0;
      }
      else
      {
       slha_leshouches2_hdec->aeval(i,j) = sd_leshouches2->aeval(i,j);
      }
    }
    for (int i=1; i<=20; ++i) slha_leshouches2_hdec->qvalue(i) = sd_leshouches2->qvalue(i); // Q(GeV)

    // Run SUSY-HIT
    sdecay();

  }

}
END_BE_NAMESPACE


// Initialisation function (definition)
BE_INI_FUNCTION
{
  SLHAstruct slha;
  int pdg_codes[35];
  pdg_codes[0]  = 24;      // W+
  pdg_codes[1]  = 25;      // h
  pdg_codes[2]  = 35;      // H
  pdg_codes[3]  = 36;      // A
  pdg_codes[4]  = 37;      // H+
  pdg_codes[26] = 1000021; // ~g
  pdg_codes[27] = 1000022; // ~chi_10
  pdg_codes[28] = 1000023; // ~chi_20
  pdg_codes[29] = 1000025; // ~chi_30
  pdg_codes[30] = 1000035; // ~chi_40
  pdg_codes[31] = 1000024; // ~chi_1+
  pdg_codes[32] = 1000037; // ~chi_2+
  pdg_codes[33] = 5;       // b pole
  pdg_codes[34] = 1000039; // ~G

  // If the user provides a file list, just read in SLHA files for debugging and ignore the MSSM_spectrum dependency.
  if (runOptions->hasKey("debug_SLHA_filenames"))
  {
    static unsigned int counter = 0;
    std::vector<str> filenames = runOptions->getValue<std::vector<str> >("debug_SLHA_filenames");
    logger() << "Reading SLHA file: " << filenames[counter] << std::endl;
    std::ifstream ifs(filenames[counter]);
    if(!ifs.good()) backend_error().raise(LOCAL_INFO, "SLHA file not found.");
    ifs >> slha;
    ifs.close();
    counter++;
    if (counter >= filenames.size()) counter = 0;
    pdg_codes[5]  = 1000001; // ~d_L
    pdg_codes[6]  = 2000001; // ~d_R
    pdg_codes[7]  = 1000002; // ~u_L
    pdg_codes[8]  = 2000002; // ~u_R
    pdg_codes[9]  = 1000003; // ~s_L
    pdg_codes[10] = 2000003; // ~s_R
    pdg_codes[11] = 1000004; // ~c_L
    pdg_codes[12] = 2000004; // ~c_R
    pdg_codes[13] = 1000005; // ~b_1
    pdg_codes[14] = 2000005; // ~b_2
    pdg_codes[15] = 1000006; // ~t_1
    pdg_codes[16] = 2000006; // ~t_2
    pdg_codes[17] = 1000011; // ~e_L
    pdg_codes[18] = 2000011; // ~e_R
    pdg_codes[19] = 1000012; // ~nu_eL
    pdg_codes[20] = 1000013; // ~mu_L
    pdg_codes[21] = 2000013; // ~mu_R
    pdg_codes[22] = 1000014; // ~nu_muL
    pdg_codes[23] = 1000015; // ~tau_1
    pdg_codes[24] = 2000015; // ~tau_2
    pdg_codes[25] = 1000016; // ~nu_tauL
    // If the CKM entries are missing from the SLHA file, fill them in with defaults.
    SLHAea_add(slha, "VCKMIN", 1, 0.22537, "lambda", false);
    SLHAea_add(slha, "VCKMIN", 2, 0.814, "A", false);
    SLHAea_add(slha, "VCKMIN", 3, 0.117, "rhobar", false);
    SLHAea_add(slha, "VCKMIN", 4, 0.353, "etabar", false);
    // If the b pole mass is missing from the SLHA file, fill it in with a default.
    SLHAea_add(slha, "MASS",   5, 4.87878, "mb (pole)", false);
  }
  else // Use the actual spectrum object.
  {
    // Make sure the spectrum object is at the SUSY scale
    // FIXME uncomment when at_SUSY_scale is available
    //if (not Dep::MSSM_spectrum->at_SUSY_scale())
    //{
    //  backend_error().raise(LOCAL_INFO, "MSSM_spectrum dependency is not at the SUSY scale.");
    //}

    slha = (*Dep::MSSM_spectrum)->getSLHAea();
    const SubSpectrum* mssm = (*Dep::MSSM_spectrum)->get_HE();

    // Retrieve the tolerances for off-diagonal sfermion mixing
    const static double gtol = runOptions->getValueOrDef<double>(1e-2, "gauge_mixing_tolerance");
    const static bool gpterror = runOptions->getValueOrDef<bool>(true, "gauge_mixing_tolerance_invalidates_point_only");
    const static double ftol = runOptions->getValueOrDef<double>(1e-2, "family_mixing_tolerance");
    const static bool fpterror = runOptions->getValueOrDef<bool>(true, "family_mixing_tolerance_invalidates_point_only");

    // Add the STOPMIX, SBOTMIX and STAUMIX blocks to the SLHAea object if they aren't present already.
    str stop1 = "~t_1", stop2 = "~t_2", sbottom1 = "~b_1", sbottom2 = "~b_2", stau1 = "~tau_1", stau2 = "~tau_2";
    attempt_to_add("STOPMIX", slha, "~u", mssm, ftol, stop1, stop2, fpterror);
    attempt_to_add("SBOTMIX", slha, "~d", mssm, ftol, sbottom1, sbottom2, fpterror);
    attempt_to_add("STAUMIX", slha, "~e-", mssm, ftol, stau1, stau2, fpterror);

    // Set out the PDG codes of the mass eigenstates best corresponding to the
    // gauge eigenstates for which SUSY-HIT wants masses from the SLHA MASS block.
    pdg_codes[5]  = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~d_L",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[6]  = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~d_R",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[7]  = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~u_L",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[8]  = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~u_R",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[9]  = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~s_L",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[10] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~s_R",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[11] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~c_L",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[12] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~c_R",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[13] = Models::ParticleDB().pdg_pair(sbottom1).first,
    pdg_codes[14] = Models::ParticleDB().pdg_pair(sbottom2).first,
    pdg_codes[15] = Models::ParticleDB().pdg_pair(stop1).first,
    pdg_codes[16] = Models::ParticleDB().pdg_pair(stop2).first,
    pdg_codes[17] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~e_L",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[18] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~e_R",      mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[19] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~nu_e_L",   mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[20] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~mu_L",     mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[21] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~mu_R",     mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[22] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~nu_mu_L",  mssm, gtol, LOCAL_INFO, gpterror)).first;
    pdg_codes[23] = Models::ParticleDB().pdg_pair(stau1).first,
    pdg_codes[24] = Models::ParticleDB().pdg_pair(stau2).first,
    pdg_codes[25] = Models::ParticleDB().pdg_pair(slhahelp::mass_es_from_gauge_es("~nu_tau_L", mssm, gtol, LOCAL_INFO, gpterror)).first;
  }

  // Get the W and Z widths.
  double W_width = 0.5*(Dep::W_plus_decay_rates->width_in_GeV + Dep::W_minus_decay_rates->width_in_GeV);
  double Z_width = Dep::Z_decay_rates->width_in_GeV;

  // Calculate decay rates
  run_susy_hit(slha, W_width, Z_width, &(pdg_codes[0]));

}
END_BE_INI_FUNCTION
