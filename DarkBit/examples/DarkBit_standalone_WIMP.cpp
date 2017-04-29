//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Example of GAMBIT DarkBit standalone
///  main program.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Christoph Weniger
///  \date 2016 Feb
///  \author Jonathan Cornell
///  \date 2016 July
///  \author Sebastian Wild
///  \date 2016 Aug
///
///  *********************************************

#include <iostream>
#include <fstream>

#include "gambit/Elements/standalone_module.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/Elements/spectrum_factories.hpp"
#include "gambit/Utils/util_functions.hpp"

#include <boost/multi_array.hpp>

using namespace DarkBit::Functown;     // Functors wrapping the module's actual module functions
using namespace DarkBit::Accessors;    // Helper functions that provide some info about the module
using namespace BackendIniBit::Functown;    // Functors wrapping the backend initialisation functions

QUICK_FUNCTION(DarkBit, TH_ProcessCatalog, OLD_CAPABILITY, TH_ProcessCatalog_WIMP, DarkBit::TH_ProcessCatalog, ())
QUICK_FUNCTION(DarkBit, DarkMatter_ID, OLD_CAPABILITY, DarkMatter_ID_WIMP, std::string, ())
QUICK_FUNCTION(DarkBit, DD_couplings, OLD_CAPABILITY, DD_couplings_WIMP, DM_nucleon_couplings, ())


void dump_array_to_file(const std::string & filename, const
    boost::multi_array<double, 2> & a, const std::vector<double> & x, const
    std::vector<double> & y)
{
  std::fstream file;
  file.open(filename, std::ios_base::out);
  file << "0.0 ";
  for (size_t i = 0; i < x.size(); i++)
    file << x[i] << " ";
  file << std::endl;
  for (size_t j = 0; j < y.size(); j++)
  {
    file << y[j] << " ";
    for (size_t i = 0; i < x.size(); i++)
    {
      file << a[i][j] << " ";
    }
    file << std::endl;
  }
  file.close();
}

void dumpSpectrum(std::string filename, double mWIMP, double sv, std::vector<double> brList, double mPhi = -1)
{
  DarkMatter_ID_WIMP.reset_and_calculate();
  TH_ProcessCatalog_WIMP.setOption<std::vector<double>>("brList", brList);
  TH_ProcessCatalog_WIMP.setOption<double>("mWIMP", mWIMP);
  TH_ProcessCatalog_WIMP.setOption<double>("sv", sv);
  if (mPhi != -1)
    TH_ProcessCatalog_WIMP.setOption<double>("mPhi", mPhi);
  TH_ProcessCatalog_WIMP.reset_and_calculate();
  RD_fraction_one.reset_and_calculate();
  SimYieldTable_DarkSUSY.reset_and_calculate();
  SimYieldTable_MicrOmegas.reset_and_calculate();
  GA_missingFinalStates.reset_and_calculate();
  cascadeMC_FinalStates.reset_and_calculate();
  cascadeMC_DecayTable.reset_and_calculate();
  cascadeMC_LoopManager.reset_and_calculate();
  cascadeMC_gammaSpectra.reset_and_calculate();
  GA_AnnYield_General.reset_and_calculate();
  dump_GammaSpectrum.setOption<std::string>("filename", filename);
  dump_GammaSpectrum.reset_and_calculate();
}

namespace Gambit
{
  namespace DarkBit
  {

    void TH_ProcessCatalog_WIMP(DarkBit::TH_ProcessCatalog& result)
    {
      using namespace Pipes::TH_ProcessCatalog_WIMP;
      using std::vector;
      using std::string;

      // Initialize empty catalog and main annihilation process
      TH_ProcessCatalog catalog;
      TH_Process process_ann("WIMP", "WIMP");
      TH_Process process_dec("phi");
      TH_Process process_dec1("phi1");
      TH_Process process_dec2("phi2");

      ///////////////////////////////////////
      // Import particle masses and couplings
      ///////////////////////////////////////

#define addParticle(Name, Mass, spinX2)                                        \
      catalog.particleProperties.insert(std::pair<string, TH_ParticleProperty> \
      (Name , TH_ParticleProperty(Mass, spinX2)));

      /// Option mWIMP<double>: WIMP mass in GeV (required)
      double mWIMP = runOptions->getValue<double>("mWIMP");
      /// Option sv<double>: Cross-section in cm3/s (required)
      double sv = runOptions->getValue<double>("sv");
      double b = 0;  // defined as sv(v) = sv(v=0) + b*(sv=0)*v**2
      /// Option brList<std::vector<double>>: List of branching ratios (required)
      auto brList = runOptions->getValue<std::vector<double>>("brList");
      /// Option mWIMP<double>: WIMP mass in GeV (required)
      double mPhi = runOptions->getValueOrDef<double>(59.0, "mPhi");

      // FIXME: Use various channels include 3-body and complicated cascade
      // decay
      // FIXME: Check stability of codes w.r.t. extreme parameters
      // FIXME: Test all input possible for this function
      addParticle("gamma", 0.0,  2)
      addParticle("Z0", 91.2,  2)
      // FIXME: Jonathan: Do we really need the flavour and mass eigenstates?
      addParticle("tau+", 1.8,  1)
      addParticle("tau-", 1.8,  1)
      addParticle("e+_3", 1.8,  1)
      addParticle("e-_3", 1.8,  1)
      addParticle("e+_1", 0.00051, 1)
      addParticle("e-_1", 0.00051, 1)
      addParticle("b", 4.9,  1)
      addParticle("bbar", 4.9,  1)
      addParticle("d_3", 4.9,  1)
      addParticle("dbar_3", 4.9,  1)


      addParticle("WIMP", mWIMP,  0)
      addParticle("phi",  mPhi,  0)
      addParticle("phi1", 99.99,  0)
      addParticle("phi2", 99.99,  0)
#undef addParticle

      TH_Channel dec_channel(daFunk::vec<string>("gamma", "gamma"), daFunk::cnst(1.));
      process_dec.channelList.push_back(dec_channel);

      TH_Channel dec_channel1(daFunk::vec<string>("tau+", "tau-"), daFunk::cnst(1.));
      process_dec1.channelList.push_back(dec_channel1);

      TH_Channel dec_channel2(daFunk::vec<string>("d_3", "dbar_3"), daFunk::cnst(1.));
      process_dec2.channelList.push_back(dec_channel2);

      process_ann.resonances_thresholds.threshold_energy.push_back(2*mWIMP);
      auto p1 = daFunk::vec<string>("d_3", "gamma", "gamma", "e-_3", "e-_1", "phi");
      auto p2 = daFunk::vec<string>("dbar_3", "Z0", "gamma", "e+_3", "e+_1", "phi2");
      {
        for ( unsigned int i = 0; i < brList.size()-1; i++ )
        {
          double mtot_final =
            catalog.getParticleProperty(p1[i]).mass +
            catalog.getParticleProperty(p2[i]).mass;
          if ( mWIMP*2 > mtot_final * 0 && brList[i]!= 0.) //FIXME: Jonathan: Why is the zero here?
          {
            std::cout << p1[i] << " " << p2[i] << " " << brList[i] << std::endl;
            daFunk::Funk kinematicFunction = (daFunk::one("v")+pow(daFunk::var("v"), 2)*b)*sv*brList[i]; //FIXME: Jonathan:
            TH_Channel new_channel(
                daFunk::vec<string>(p1[i], p2[i]), kinematicFunction
                );
            process_ann.channelList.push_back(new_channel);
          }
          else
          {
            process_ann.resonances_thresholds.threshold_energy.
              push_back(mtot_final);
          }
        }
      }

      if ( brList[6] > 0. )
      {
        auto E = daFunk::var("E");
        daFunk::Funk kinematicFunction = daFunk::one("v", "E1")/(pow(E-50, 4)+1)*sv*brList[6];
        // FIXME: Include second gamma in AnnYield (currently ignored)
        TH_Channel new_channel(daFunk::vec<string>("gamma", "gamma", "Z0"), kinematicFunction);
        process_ann.channelList.push_back(new_channel);
      }

      catalog.processList.push_back(process_ann);
      catalog.processList.push_back(process_dec);
      catalog.processList.push_back(process_dec1);
      catalog.processList.push_back(process_dec2);

      catalog.validate();

      result = catalog;
    } // function TH_ProcessCatalog_WIMP

    void DarkMatter_ID_WIMP(std::string& result)
    {
      result = "WIMP";
    }

    void DD_couplings_WIMP(DM_nucleon_couplings& result)
    {
      using namespace Pipes::DD_couplings_WIMP;
      /// Option gps<double>: gps (default 0)
      result.gps = runOptions->getValueOrDef<double>(0., "gps");
      /// Option gns<double>: gns (default 0)
      result.gns = runOptions->getValueOrDef<double>(0., "gns");
      /// Option gpa<double>: gpa (default 0)
      result.gpa = runOptions->getValueOrDef<double>(0., "gpa");
      /// Option gna<double>: gna (default 0)
      result.gna = runOptions->getValueOrDef<double>(0., "gna");
      std::cout << "DD_coupling says" << std::endl;
      std::cout << result.gps << std::endl;
    }
  }
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc==1)
    {
      std::cout << "Please select test mode>=0" << std::endl;
      exit(1);
    }
    int mode = std::stoi((std::string)argv[1]);
    std::cout << "Starting with mode " << mode << std::endl;


    // ---- Initialise logging and exceptions ----

    initialise_standalone_logs("runs/DarkBit_standalone_WIMP/logs/");
    logger()<<"Running DarkBit standalone example"<<LogTags::info<<EOM;
    model_warning().set_fatal(true);


    // ---- Check that required backends are present ----

    if (not Backends::backendInfo().works["DarkSUSY5.1.3"]) backend_error().raise(LOCAL_INFO, "DarkSUSY 5.1.3 is missing!");
    if (not Backends::backendInfo().works["gamLike1.0.0"]) backend_error().raise(LOCAL_INFO, "gamLike 1.0.0 is missing!");
    if (not Backends::backendInfo().works["DDCalc1.0.0"]) backend_error().raise(LOCAL_INFO, "DDCalc 1.0.0 is missing!");
    if (not Backends::backendInfo().works["MicrOmegas_MSSM3.6.9.2"]) backend_error().raise(LOCAL_INFO, "MicrOmegas 3.6.9.2 for MSSM is missing!");

    // ---- Initialize models ----

    // Initialize halo model
    ModelParameters* Halo_primary_parameters = Models::Halo_Einasto::Functown::primary_parameters.getcontentsPtr();
    Halo_primary_parameters->setValue("rho0", 0.4);
    Halo_primary_parameters->setValue("rhos", 0.08);
    Halo_primary_parameters->setValue("vrot", 235.);
    Halo_primary_parameters->setValue("v0", 235.);
    Halo_primary_parameters->setValue("vesc", 550.);
    Halo_primary_parameters->setValue("rs", 20.);
    Halo_primary_parameters->setValue("r_sun", 8.5);
    Halo_primary_parameters->setValue("alpha", 0.17);


    // --- Resolve halo dependencies ---
    ExtractLocalMaxwellianHalo.notifyOfModel("Halo_Einasto");
    ExtractLocalMaxwellianHalo.resolveDependency(&Models::Halo_Einasto::Functown::primary_parameters);
    ExtractLocalMaxwellianHalo.reset_and_calculate();

    GalacticHalo_Einasto.notifyOfModel("Halo_Einasto");
    GalacticHalo_Einasto.resolveDependency(&Models::Halo_Einasto::Functown::primary_parameters);
    GalacticHalo_Einasto.reset_and_calculate();

    // ---- Initialize backends ----

    // Set up DDCalc backend initialization
    Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple.setStatus(2);
    Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment.setStatus(2);
    Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood.setStatus(2);
    DDCalc_1_0_0_init.resolveDependency(&ExtractLocalMaxwellianHalo);
    DDCalc_1_0_0_init.resolveDependency(&RD_fraction_one);
    DDCalc_1_0_0_init.resolveDependency(&mwimp_generic);
    DDCalc_1_0_0_init.resolveDependency(&DD_couplings_WIMP);


    // Initialize gamLike backend
    gamLike_1_0_0_init.reset_and_calculate();

    // Initialize DarkSUSY backend
    DarkSUSY_5_1_3_init.reset_and_calculate();

    // Initialize MicrOmegas backend
    MicrOmegas_MSSM_3_6_9_2_init.notifyOfModel("Halo_Einasto");  // FIXME: Just a hack to get MicrOmegas initialized without specifying an MSSM model
    MicrOmegas_MSSM_3_6_9_2_init.reset_and_calculate();

    // ---- Set up basic internal structures for direct & indirect detection ----

    // Set identifier for DM particle
    // FIXME: Needed?

    // Set up process catalog based on DarkSUSY annihilation rates

    // Assume for direct and indirect detection likelihoods that dark matter
    // density is always the measured one (despite relic density results)

    // ---- Gamma-ray yields ----

    // Initialize tabulated gamma-ray yields
    // FIXME: Use three different simyieldtables
    SimYieldTable_DarkSUSY.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::dshayield);
    SimYieldTable_MicrOmegas.resolveBackendReq(&Backends::MicrOmegas_MSSM_3_6_9_2::Functown::dNdE);
    SimYieldTable_DarkSUSY.setOption<bool>("allow_yield_extrapolation", true);
    SimYieldTable_MicrOmegas.setOption<bool>("allow_yield_extrapolation", true);

    // Select SimYieldTable
    //auto SimYieldTablePointer = &SimYieldTable_MicrOmegas;
    auto SimYieldTablePointer = &SimYieldTable_DarkSUSY;

    // Collect missing final states for simulation in cascade MC
    GA_missingFinalStates.resolveDependency(&TH_ProcessCatalog_WIMP);
    GA_missingFinalStates.resolveDependency(SimYieldTablePointer);
    GA_missingFinalStates.resolveDependency(&DarkMatter_ID_WIMP);

    // Infer for which type of final states particles MC should be performed
    cascadeMC_FinalStates.setOption<std::vector<std::string>>("cMC_finalStates", daFunk::vec((std::string)"gamma"));

    // Collect decay information for cascade MC
    cascadeMC_DecayTable.resolveDependency(&TH_ProcessCatalog_WIMP);
    cascadeMC_DecayTable.resolveDependency(SimYieldTablePointer);

    // Set up MC loop manager for cascade MC
    cascadeMC_LoopManager.setOption<int>("cMC_maxEvents", 20000);
    cascadeMC_Histograms.setOption<double>("cMC_endCheckFrequency", 25);
    cascadeMC_Histograms.setOption<double>("cMC_gammaRelError", .05);
    cascadeMC_Histograms.setOption<int>("cMC_numSpecSamples", 25);
    cascadeMC_Histograms.setOption<int>("cMC_NhistBins", 300);
    cascadeMC_LoopManager.resolveDependency(&GA_missingFinalStates);
    std::vector<functor*> nested_functions = initVector<functor*>(
        &cascadeMC_InitialState, &cascadeMC_GenerateChain, &cascadeMC_Histograms, &cascadeMC_EventCount);
    cascadeMC_LoopManager.setNestedList(nested_functions);

    // Set up initial state for cascade MC step
    cascadeMC_InitialState.resolveDependency(&GA_missingFinalStates);
    cascadeMC_InitialState.resolveLoopManager(&cascadeMC_LoopManager);
    //cascadeMC_InitialState.reset_and_calculate();

    // Perform MC step for cascade MC
    cascadeMC_GenerateChain.resolveDependency(&cascadeMC_InitialState);
    cascadeMC_GenerateChain.resolveDependency(&cascadeMC_DecayTable);
    cascadeMC_GenerateChain.resolveLoopManager(&cascadeMC_LoopManager);
    //cascadeMC_GenerateChain.reset_and_calculate();

    // Generate histogram for cascade MC
    cascadeMC_Histograms.resolveDependency(&cascadeMC_InitialState);
    cascadeMC_Histograms.resolveDependency(&cascadeMC_GenerateChain);
    cascadeMC_Histograms.resolveDependency(&TH_ProcessCatalog_WIMP);
    cascadeMC_Histograms.resolveDependency(SimYieldTablePointer);
    cascadeMC_Histograms.resolveDependency(&cascadeMC_FinalStates);
    cascadeMC_Histograms.resolveLoopManager(&cascadeMC_LoopManager);
    //cascadeMC_Histograms.reset_and_calculate();

    // Check convergence of cascade MC
    // FIXME: Test dynamic convergence criteria for cascade routines
    cascadeMC_EventCount.resolveDependency(&cascadeMC_InitialState);
    cascadeMC_EventCount.resolveLoopManager(&cascadeMC_LoopManager);
    //cascadeMC_EventCount.reset_and_calculate();

    // Start cascade MC loop

    // Infer gamma-ray spectra for recorded MC results
    cascadeMC_gammaSpectra.resolveDependency(&GA_missingFinalStates);
    cascadeMC_gammaSpectra.resolveDependency(&cascadeMC_FinalStates);
    cascadeMC_gammaSpectra.resolveDependency(&cascadeMC_Histograms);
    cascadeMC_gammaSpectra.resolveDependency(&cascadeMC_EventCount);

    // Calculate total gamma-ray yield (cascade MC + tabulated results)
    GA_AnnYield_General.resolveDependency(&TH_ProcessCatalog_WIMP);
    GA_AnnYield_General.resolveDependency(SimYieldTablePointer);
    GA_AnnYield_General.resolveDependency(&DarkMatter_ID_WIMP);
    GA_AnnYield_General.resolveDependency(&cascadeMC_gammaSpectra);

    // FIXME: Extend existing gamma-ray spectrum dumper
    dump_GammaSpectrum.resolveDependency(&GA_AnnYield_General);

    // Calculate Fermi LAT dwarf likelihood
    // FIXME: Check whether Fermi lat limits can be reproduced
    lnL_FermiLATdwarfs_gamLike.resolveDependency(&GA_AnnYield_General);
    lnL_FermiLATdwarfs_gamLike.resolveDependency(&RD_fraction_one);
    lnL_FermiLATdwarfs_gamLike.resolveBackendReq(&Backends::gamLike_1_0_0::Functown::lnL);


    // -- Calculate relic density --
    RD_eff_annrate_from_ProcessCatalog.notifyOfModel("SingletDM");
    RD_eff_annrate_from_ProcessCatalog.resolveDependency(&TH_ProcessCatalog_WIMP);
    RD_eff_annrate_from_ProcessCatalog.resolveDependency(&DarkMatter_ID_WIMP);

    RD_spectrum_from_ProcessCatalog.resolveDependency(&TH_ProcessCatalog_WIMP);
    RD_spectrum_from_ProcessCatalog.resolveDependency(&DarkMatter_ID_WIMP);

    RD_spectrum_ordered_func.resolveDependency(&RD_spectrum_from_ProcessCatalog);

    RD_oh2_general.resolveDependency(&RD_spectrum_ordered_func);
    RD_oh2_general.resolveDependency(&RD_eff_annrate_from_ProcessCatalog);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::dsrdthlim);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::dsrdtab);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::dsrdeqn);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::dsrdwintp);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::widths);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rdmgev);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rdpth);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rdpars);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rdswitch);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rdlun);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rdpadd);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rddof);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::rderrors);
    RD_oh2_general.resolveBackendReq(&Backends::DarkSUSY_5_1_3::Functown::DSparticle_code);


    // ---- Calculate direct detection constraints ----

    // Calculate direct detection rates for LUX 2016, PandaX 2016, LUX 2013, XENON100 2012, SIMPLE and PICO
    LUX_2016_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    LUX_2016_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    PandaX_2016_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PandaX_2016_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    LUX_2013_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    LUX_2013_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    XENON100_2012_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    XENON100_2012_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    PICO_60_F_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PICO_60_F_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    PICO_60_I_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PICO_60_I_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    SIMPLE_2014_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    SIMPLE_2014_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    PICO_2L_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PICO_2L_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);
    SuperCDMS_2014_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    SuperCDMS_2014_Calc.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_CalcRates_simple);

    // Calculate direct detection likelihood for LUX 2016, PandaX 2016, LUX 2013, XENON100 2012, SIMPLE and PICO
    LUX_2016_GetLogLikelihood.resolveDependency(&LUX_2016_Calc);
    LUX_2016_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    LUX_2016_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    PandaX_2016_GetLogLikelihood.resolveDependency(&PandaX_2016_Calc);
    PandaX_2016_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PandaX_2016_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    LUX_2013_GetLogLikelihood.resolveDependency(&LUX_2013_Calc);
    LUX_2013_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    LUX_2013_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    XENON100_2012_GetLogLikelihood.resolveDependency(&XENON100_2012_Calc);
    XENON100_2012_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    XENON100_2012_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    PICO_60_F_GetLogLikelihood.resolveDependency(&PICO_60_F_Calc);
    PICO_60_F_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PICO_60_F_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    PICO_60_I_GetLogLikelihood.resolveDependency(&PICO_60_I_Calc);
    PICO_60_I_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PICO_60_I_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    SIMPLE_2014_GetLogLikelihood.resolveDependency(&SIMPLE_2014_Calc);
    SIMPLE_2014_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    SIMPLE_2014_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    PICO_2L_GetLogLikelihood.resolveDependency(&PICO_2L_Calc);
    PICO_2L_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    PICO_2L_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);
    SuperCDMS_2014_GetLogLikelihood.resolveDependency(&SuperCDMS_2014_Calc);
    SuperCDMS_2014_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_Experiment);
    SuperCDMS_2014_GetLogLikelihood.resolveBackendReq(&Backends::DDCalc_1_0_0::Functown::DDCalc_LogLikelihood);

    // Set generic WIMP mass object
    mwimp_generic.resolveDependency(&TH_ProcessCatalog_WIMP);
    mwimp_generic.resolveDependency(&DarkMatter_ID_WIMP);
    sigma_SI_p_simple.resolveDependency(&DD_couplings_WIMP);
    sigma_SI_p_simple.resolveDependency(&mwimp_generic);

    // Generate gamma-ray spectra for various final states
    if ( (mode >= 0) and (mode < 6) )
    {
      std::cout << "Producing test spectra." << std::endl;
      double mass = 100.;
      double sv = 3e-26;
      if (mode==0) dumpSpectrum("dNdE0.dat", mass, sv, daFunk::vec<double>(1., 0., 0., 0., 0., 0.,0.));
      if (mode==1) dumpSpectrum("dNdE1.dat", mass, sv, daFunk::vec<double>(0., 1., 0., 0., 0., 0.,0.));
      if (mode==2) dumpSpectrum("dNdE2.dat", mass, sv, daFunk::vec<double>(0., 0., 1., 0., 0., 0.,0.));
      if (mode==3) dumpSpectrum("dNdE3.dat", mass, sv, daFunk::vec<double>(0., 0., 0., 1., 0., 0.,0.));
      if (mode==4) dumpSpectrum("dNdE4.dat", mass, sv, daFunk::vec<double>(0., 0., 0., 0., 0., 1., 0.));
      if (mode==5) dumpSpectrum("dNdE5.dat", mass, sv*0.1, daFunk::vec<double>(0., 0., 0., 0., 0., 0., 1.));
    }

    // Generate gamma-ray spectra for various masses
    if (mode >= 10)
    {
      std::cout << "Producing test spectra." << std::endl;
      double mass = 100.;
      double sv = 3e-26;
      std::string filename = "dNdE_FCMC_" + std::to_string(mode) + ".dat";
      dumpSpectrum(filename, mass, sv, daFunk::vec<double>(0., 0., 0., 0., 0., 1., 0.), mode);
    }

    // Generate gamma-ray likelihood maps
    if (mode==6)
    {
      // Systematic parameter maps annihilation
      std::cout << "Producing test maps." << std::endl;
      int mBins = 60;
      int svBins = 40;
      double oh2, lnL;
      std::vector<double> sv_list, m_list;
      //std::vector<double> m_list = daFunk::logspace(1.0, 4.0, mBins);

      boost::multi_array<double, 2> lnL_b_array{boost::extents[mBins][svBins]},
          lnL_tau_array{boost::extents[mBins][svBins]};
      boost::multi_array<double, 2> oh2_array{boost::extents[mBins][svBins]};

      sv_list = daFunk::logspace(-28.0, -22.0, svBins);

      m_list = daFunk::logspace(log10(5.), 4., mBins-10);
      for (size_t i = 0; i < m_list.size(); i++)
      {
        for (size_t j = 0; j < sv_list.size(); j++)
        {
          TH_ProcessCatalog_WIMP.setOption<double>("mWIMP", m_list[i]);
          TH_ProcessCatalog_WIMP.setOption<double>("sv", sv_list[j]);
          std::cout << "Parameters: " << m_list[i] << " " << sv_list[j] << std::endl;

          TH_ProcessCatalog_WIMP.setOption<std::vector<double>>("brList", daFunk::vec<double>(1., 0., 0., 0., 0., 0.,0.));
          DarkMatter_ID_WIMP.reset_and_calculate();
          TH_ProcessCatalog_WIMP.reset_and_calculate();
          RD_fraction_one.reset_and_calculate();
          SimYieldTable_DarkSUSY.reset_and_calculate();
          SimYieldTable_MicrOmegas.reset_and_calculate();
          GA_missingFinalStates.reset_and_calculate();
          cascadeMC_FinalStates.reset_and_calculate();
          cascadeMC_DecayTable.reset_and_calculate();
          cascadeMC_LoopManager.reset_and_calculate();
          cascadeMC_gammaSpectra.reset_and_calculate();
          GA_AnnYield_General.reset_and_calculate();
          //dump_GammaSpectrum.reset_and_calculate();
          lnL_FermiLATdwarfs_gamLike.reset_and_calculate();
          lnL = lnL_FermiLATdwarfs_gamLike(0);
          std::cout << "Fermi LAT likelihood: " << lnL << std::endl;
          lnL_b_array[i][j] = lnL;
        }
      }

      dump_array_to_file("Fermi_b_table.dat", lnL_b_array, m_list, sv_list);

      m_list = daFunk::logspace(log10(1.9), 4., mBins-10);
      for (size_t i = 0; i < m_list.size(); i++)
      {
        for (size_t j = 0; j < sv_list.size(); j++)
        {
          TH_ProcessCatalog_WIMP.setOption<double>("mWIMP", m_list[i]);
          TH_ProcessCatalog_WIMP.setOption<double>("sv", sv_list[j]);
          std::cout << "Parameters: " << m_list[i] << " " << sv_list[j] << std::endl;

          TH_ProcessCatalog_WIMP.setOption<std::vector<double>>("brList", daFunk::vec<double>(0., 0., 0., 1., 0., 0., 0.));
          DarkMatter_ID_WIMP.reset_and_calculate();
          TH_ProcessCatalog_WIMP.reset_and_calculate();
          RD_fraction_one.reset_and_calculate();
          SimYieldTable_DarkSUSY.reset_and_calculate();
          SimYieldTable_MicrOmegas.reset_and_calculate();
          GA_missingFinalStates.reset_and_calculate();
          cascadeMC_FinalStates.reset_and_calculate();
          cascadeMC_DecayTable.reset_and_calculate();
          cascadeMC_LoopManager.reset_and_calculate();
          cascadeMC_gammaSpectra.reset_and_calculate();
          GA_AnnYield_General.reset_and_calculate();
          //dump_GammaSpectrum.reset_and_calculate();
          lnL_FermiLATdwarfs_gamLike.reset_and_calculate();
          lnL = lnL_FermiLATdwarfs_gamLike(0);
          std::cout << "Fermi LAT likelihood: " << lnL << std::endl;
          lnL_tau_array[i][j] = lnL;
        }
      }

      dump_array_to_file("Fermi_tau_table.dat", lnL_tau_array, m_list, sv_list);

      m_list = daFunk::logspace(-1.0, 4., mBins);
      for (size_t i = 0; i < m_list.size(); i++)
      {
        for (size_t j = 0; j < sv_list.size(); j++)
        {
          TH_ProcessCatalog_WIMP.setOption<double>("mWIMP", m_list[i]);
          TH_ProcessCatalog_WIMP.setOption<double>("sv", sv_list[j]);
          std::cout << "Parameters: " << m_list[i] << " " << sv_list[j] << std::endl;

          TH_ProcessCatalog_WIMP.setOption<std::vector<double>>("brList", daFunk::vec<double>(0., 0., 0., 0., 1., 0., 0.));
          DarkMatter_ID_WIMP.reset_and_calculate();
          TH_ProcessCatalog_WIMP.reset_and_calculate();
          RD_eff_annrate_from_ProcessCatalog.reset_and_calculate();
          RD_spectrum_from_ProcessCatalog.reset_and_calculate();
          RD_spectrum_ordered_func.reset_and_calculate();
          RD_oh2_general.reset_and_calculate();
          oh2 = RD_oh2_general(0);
          oh2_array[i][j] = oh2;
        }
      }

      dump_array_to_file("oh2_table.dat", oh2_array, m_list, sv_list);
    }

    // Generate direct detection likelihood maps
    if (mode==7)
    {
      // Systematic parameter maps scattering
      std::cout << "Producing test maps." << std::endl;
      double lnL1, lnL2, lnL3, lnL4, lnL5;
      double g, reduced_mass;
      //int mBins = 300;
      //int sBins = 200;
      int mBins = 120;
      int sBins = 80;
      const double mN = (m_proton + m_neutron) / 2;
      std::vector<double> m_list = daFunk::logspace(0.0, 4.0, mBins);
      std::vector<double> s_list;
      boost::multi_array<double, 2> lnL_array1{boost::extents[mBins][sBins]},
          lnL_array2{boost::extents[mBins][sBins]}, lnL_array3{boost::extents[mBins][sBins]},
          lnL_array4{boost::extents[mBins][sBins]}, lnL_array5{boost::extents[mBins][sBins]};
      TH_ProcessCatalog_WIMP.setOption<double>("sv", 0.);
      TH_ProcessCatalog_WIMP.setOption<std::vector<double>>("brList", daFunk::vec<double>(1., 0., 0., 0., 0., 0., 0.));

      s_list = daFunk::logspace(-46., -39., sBins);
      // Calculate array of sigma_SI and lnL values for LUX 2016 and 2013, Xenon100,
      // and PandaX, assuming gps=gns
      for (size_t i = 0; i < m_list.size(); i++)
      {
        for (size_t j = 0; j < s_list.size(); j++)
        {
          // Re-initialize DDCalc with LUX 2013/2016 and PandaX-I halo parameters
          Halo_primary_parameters->setValue("rho0", 0.3);
          Halo_primary_parameters->setValue("vrot", 232.7); // v_Earth = 245 km/s
          Halo_primary_parameters->setValue("v0", 220.);
          Halo_primary_parameters->setValue("vesc", 544.);
          ExtractLocalMaxwellianHalo.reset_and_calculate();

          TH_ProcessCatalog_WIMP.setOption<double>("mWIMP", m_list[i]);
          std::cout << "Parameters: " << m_list[i] << " " << s_list[j] << std::endl;
          reduced_mass = (m_list[i] * mN) / (mN + m_list[i]);
          g = sqrt(s_list[j]*pi/gev2cm2) / (reduced_mass);
          DarkMatter_ID_WIMP.reset_and_calculate();
          TH_ProcessCatalog_WIMP.reset_and_calculate();
          RD_fraction_one.reset_and_calculate();
          DD_couplings_WIMP.setOption<double>("gps", g);
          DD_couplings_WIMP.setOption<double>("gns", g);
          DD_couplings_WIMP.setOption<double>("gpa", 0.);
          DD_couplings_WIMP.setOption<double>("gna", 0.);
          DD_couplings_WIMP.reset_and_calculate();
          mwimp_generic.reset_and_calculate();
          DDCalc_1_0_0_init.reset_and_calculate();

          DDCalc_1_0_0_init.reset_and_calculate();
          LUX_2016_Calc.reset_and_calculate();
          LUX_2016_GetLogLikelihood.reset_and_calculate();
          LUX_2013_Calc.reset_and_calculate();
          LUX_2013_GetLogLikelihood.reset_and_calculate();
          PandaX_2016_Calc.reset_and_calculate();
          PandaX_2016_GetLogLikelihood.reset_and_calculate();
          lnL1 = LUX_2016_GetLogLikelihood(0);
          lnL2 = PandaX_2016_GetLogLikelihood(0);
          lnL3 = LUX_2013_GetLogLikelihood(0);

          // Re-initialize DDCalc with Xenon100 2012 halo parameters
          Halo_primary_parameters->setValue("rho0", 0.3);
          Halo_primary_parameters->setValue("vrot", 220.);
          Halo_primary_parameters->setValue("v0", 220.);
          Halo_primary_parameters->setValue("vesc", 544.);
          ExtractLocalMaxwellianHalo.reset_and_calculate();

          DDCalc_1_0_0_init.reset_and_calculate();
          XENON100_2012_Calc.reset_and_calculate();
          XENON100_2012_GetLogLikelihood.reset_and_calculate();
          lnL4 = XENON100_2012_GetLogLikelihood(0);

          // Re-initialize DDCalc with SUPER-CDMS halo parameters
          Halo_primary_parameters->setValue("rho0", 0.3);
          Halo_primary_parameters->setValue("vrot", 231.7); // v_Earth = 244 km/s
          Halo_primary_parameters->setValue("v0", 220.);
          Halo_primary_parameters->setValue("vesc", 544.);
          ExtractLocalMaxwellianHalo.reset_and_calculate();

          DDCalc_1_0_0_init.reset_and_calculate();
          SuperCDMS_2014_Calc.reset_and_calculate();
          SuperCDMS_2014_GetLogLikelihood.reset_and_calculate();
          lnL5 = SuperCDMS_2014_GetLogLikelihood(0);

          std::cout << "LUX2016 SI lnL = " << lnL1 << std::endl;
          std::cout << "PandaX lnL = " << lnL2 << std::endl;
          std::cout << "LUX2013 lnL = " << lnL3 << std::endl;
          std::cout << "XENON100_2012 lnL = " << lnL4 << std::endl;
          std::cout << "SuperCDMS_2014 lnL = " << lnL5 << std::endl;

          lnL_array1[i][j] = lnL1;
          lnL_array2[i][j] = lnL2;
          lnL_array3[i][j] = lnL3;
          lnL_array4[i][j] = lnL4;
          lnL_array5[i][j] = lnL5;
        }
      }

      //dump_array_to_file("sigmaSIp_table.dat", sigma_array, m_list, s_list);
      dump_array_to_file("LUX_2016_table.dat", lnL_array1, m_list, s_list);
      dump_array_to_file("PandaX_2016_table.dat", lnL_array2, m_list, s_list);
      dump_array_to_file("LUX_2013_SI_table.dat", lnL_array3, m_list, s_list);
      dump_array_to_file("XENON100_2012_table.dat", lnL_array4, m_list, s_list);
      dump_array_to_file("SuperCDMS_2014_table.dat", lnL_array5, m_list, s_list);

      s_list = daFunk::logspace(-41., -35., sBins);
      // Calculate array of sigma_SD,p and lnL values for PICO-60, LUX 2013,
      // SIMPLE 2014, and PandaX assuming gna=0
      for (size_t i = 0; i < m_list.size(); i++)
      {
        for (size_t j = 0; j < s_list.size(); j++)
        {
          // Set LocalHalo Model parameters to PICO-60 values
          Halo_primary_parameters->setValue("rho0", 0.3);
          Halo_primary_parameters->setValue("vrot", 220.);
          Halo_primary_parameters->setValue("v0", 220.);
          Halo_primary_parameters->setValue("vesc", 544.);
          ExtractLocalMaxwellianHalo.reset_and_calculate();

          TH_ProcessCatalog_WIMP.setOption<double>("mWIMP", m_list[i]);
          std::cout << "Parameters: " << m_list[i] << " " << s_list[j] << std::endl;
          reduced_mass = (m_list[i] * m_proton) / (m_proton + m_list[i]);
          g = sqrt(s_list[j]*pi/(3*gev2cm2)) / (reduced_mass);
          DarkMatter_ID_WIMP.reset_and_calculate();
          TH_ProcessCatalog_WIMP.reset_and_calculate();
          RD_fraction_one.reset_and_calculate();
          DD_couplings_WIMP.setOption<double>("gps", 0.);
          DD_couplings_WIMP.setOption<double>("gns", 0.);
          DD_couplings_WIMP.setOption<double>("gpa", g);
          DD_couplings_WIMP.setOption<double>("gna", 0.);
          DD_couplings_WIMP.reset_and_calculate();
          mwimp_generic.reset_and_calculate();

          DDCalc_1_0_0_init.reset_and_calculate();
          PICO_60_F_Calc.reset_and_calculate();
          PICO_60_F_GetLogLikelihood.reset_and_calculate();
          PICO_60_I_Calc.reset_and_calculate();
          PICO_60_I_GetLogLikelihood.reset_and_calculate();
          lnL1 = PICO_60_F_GetLogLikelihood(0) + PICO_60_I_GetLogLikelihood(0);
          PICO_2L_Calc.reset_and_calculate();
          PICO_2L_GetLogLikelihood.reset_and_calculate();
          lnL2 = PICO_2L_GetLogLikelihood(0);

          // Set LocalHalo Model parameters to SIMPLE 2014 values
          Halo_primary_parameters->setValue("rho0", 0.3);
          Halo_primary_parameters->setValue("vrot", 231.7); // v_Earth = 244 km/s
          Halo_primary_parameters->setValue("v0", 230.);
          Halo_primary_parameters->setValue("vesc", 600.);
          ExtractLocalMaxwellianHalo.reset_and_calculate();

          DDCalc_1_0_0_init.reset_and_calculate();
          SIMPLE_2014_Calc.reset_and_calculate();
          SIMPLE_2014_GetLogLikelihood.reset_and_calculate();
          lnL3 = SIMPLE_2014_GetLogLikelihood(0);
          SIMPLE_2014_Calc.reset_and_calculate();
          SIMPLE_2014_GetLogLikelihood.reset_and_calculate();
          lnL3 = SIMPLE_2014_GetLogLikelihood(0);

          // Re-initialize DDCalc with LUX 2013 halo parameters (and PandaX?)
          Halo_primary_parameters->setValue("rho0", 0.3);
          Halo_primary_parameters->setValue("vrot", 232.7); // v_Earth = 245 km/s
          Halo_primary_parameters->setValue("v0", 220.);
          Halo_primary_parameters->setValue("vesc", 544.);
          ExtractLocalMaxwellianHalo.reset_and_calculate();

          DDCalc_1_0_0_init.reset_and_calculate();
          LUX_2013_Calc.reset_and_calculate();
          LUX_2013_GetLogLikelihood.reset_and_calculate();
          lnL4 = LUX_2013_GetLogLikelihood(0);
          PandaX_2016_Calc.reset_and_calculate();
          PandaX_2016_GetLogLikelihood.reset_and_calculate();
          lnL5 = PandaX_2016_GetLogLikelihood(0);

          lnL_array1[i][j] = lnL1;
          lnL_array2[i][j] = lnL2;
          lnL_array3[i][j] = lnL3;
          lnL_array4[i][j] = lnL4;
          lnL_array5[i][j] = lnL5;

          std::cout << "PICO_60 lnL = " << lnL1 << std::endl;
          std::cout << "PICO_2L lnL = " << lnL2 << std::endl;
          std::cout << "SIMPLE_2014 lnL = " << lnL3 << std::endl;
          std::cout << "LUX_2013 SD,p lnL = " << lnL4 << std::endl;
          std::cout << "PandaX_2016_SD,p lnL = " << lnL5 << std::endl;
        }
      }

      // Reset halo parameters to DarkBit defaults.
      Halo_primary_parameters->setValue("rho0", 0.4);
      Halo_primary_parameters->setValue("vrot", 235.);
      Halo_primary_parameters->setValue("v0", 235.);
      Halo_primary_parameters->setValue("vesc", 550.);
      ExtractLocalMaxwellianHalo.reset_and_calculate();
      GalacticHalo_Einasto.reset_and_calculate();

      dump_array_to_file("PICO_60_table.dat", lnL_array1, m_list, s_list);
      dump_array_to_file("PICO_2L_table.dat", lnL_array2, m_list, s_list);
      dump_array_to_file("SIMPLE_2014_table.dat", lnL_array3, m_list, s_list);
      dump_array_to_file("LUX_2013_SDp_table.dat", lnL_array4, m_list, s_list);
      dump_array_to_file("PandaX_2016_SDp_table.dat", lnL_array5, m_list, s_list);
    }
  }

  catch (std::exception& e)
  {
    std::cout << "DarkBit_standalone_WIMP has exited with fatal exception: " << e.what() << std::endl;
  }

  return 0;
}
