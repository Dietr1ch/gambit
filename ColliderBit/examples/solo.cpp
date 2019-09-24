//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///
///  ColliderBit Solo: an event-based LHC recast
///  tool using the GAMBIT ColliderBit module.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///  (p.scott@imperial.ac.uk)
///  \date May 2019
///
///  *********************************************

#include "gambit/Elements/standalone_module.hpp"
#include "gambit/ColliderBit/ColliderBit_rollcall.hpp"
#include "gambit/Utils/cats.hpp"

#define NULIKE_VERSION "1.0.7"
#define NULIKE_SAFE_VERSION 1_0_7

using namespace ColliderBit::Functown;
using namespace BackendIniBit::Functown;
using namespace CAT(Backends::nulike_,NULIKE_SAFE_VERSION)::Functown;

/// ColliderBit Solo main program
int main(int argc, char* argv[])
{

  try
  {

    // Check the number of command line arguments
    if (argc < 2)
    {
      // Tell the user how to run the program and exit
      cerr << endl << "Usage: " << argv[0] << " <your CBS yaml file>" << endl << endl;
      return 1;
    }

    // Make sure that nulike is present.
    if (not Backends::backendInfo().works[str("nulike")+NULIKE_VERSION]) backend_error().raise(LOCAL_INFO, str("nulike ")+NULIKE_VERSION+" is missing!");

    // Print the banner (if you could call it that)
    cout << endl;
    cout << "==================================" << endl;
    cout << "||                              ||" << endl;
    cout << "||    CBS: ColliderBit Solo     ||" << endl;
    cout << "||  GAMBIT Collider Workgroup   ||" << endl;
    cout << "||                              ||" << endl;
    cout << "==================================" << endl;
    cout << endl;

    // Read input file name
    const std::string filename_in = argv[1];

    // Read the settings in the input file
    YAML::Node infile;
    std::vector<str> analyses;
    Options settings;
    try
    {
      // Load up the file
      infile = YAML::LoadFile(filename_in);
      // Retrieve the analyses
      if (infile["analyses"]) analyses = infile["analyses"].as<std::vector<str>>();
      else throw std::runtime_error("Analyses list not found in "+filename_in+".  Quitting...");
      // Retrieve the other settings
      if (infile["settings"]) settings = Options(infile["settings"]);
      else throw std::runtime_error("Settings section not found in "+filename_in+".  Quitting...");
    }
    catch (YAML::Exception &e)
    {
      throw std::runtime_error("YAML error in "+filename_in+".\n(yaml-cpp error: "+std::string(e.what())+" )");
    }

    // Translate relevant settings into appropriate variables
    bool debug = settings.getValueOrDef<bool>(false, "debug");
    bool use_lnpiln = settings.getValueOrDef<bool>(false, "use_lognormal_distribution_for_1d_systematic");
    str lhef_filename = settings.getValue<str>("event_file");
    if (debug) cout << "Reading LHEF file: " << lhef_filename << endl;

    // Initialise logs
    logger().set_log_debug_messages(debug);
    initialise_standalone_logs("CBS_logs/");
    logger()<<"Running CBS"<<LogTags::info<<EOM;

    // Initialise the random number generator, using a hardware seed if no seed is given in the input file.
    int seed = settings.getValueOrDef<int>(-1, "seed");
    Random::create_rng_engine("default", seed);

    // Pass options to the main event loop
    YAML::Node CBS(infile["settings"]);
    CBS["analyses"] = analyses;
    CBS["min_nEvents"] = (long long)(1000);
    CBS["max_nEvents"] = (long long)(1000000000);
    operateLHCLoop.setOption<YAML::Node>("CBS", CBS);
    operateLHCLoop.setOption<bool>("silenceLoop", not debug);

    // Pass the filename to the LHEF reader function
    getLHEvent.setOption<str>("lhef_filename", lhef_filename);

    // Pass options to the cross-section function
    if (settings.hasKey("cross_section_pb"))
    {
      getYAMLxsec.setOption<double>("cross_section_pb", settings.getValue<double>("cross_section_pb"));
      if (settings.hasKey("cross_section_fractional_uncert")) { getYAMLxsec.setOption<double>("cross_section_fractional_uncert", settings.getValue<double>("cross_section_fractional_uncert")); }
      else {getYAMLxsec.setOption<double>("cross_section_uncert_pb", settings.getValue<double>("cross_section_uncert_pb")); }
    }
    else // <-- must have option "cross_section_fb"
    {
      getYAMLxsec.setOption<double>("cross_section_fb", settings.getValue<double>("cross_section_fb"));
      if (settings.hasKey("cross_section_fractional_uncert")) { getYAMLxsec.setOption<double>("cross_section_fractional_uncert", settings.getValue<double>("cross_section_fractional_uncert")); }
      else { getYAMLxsec.setOption<double>("cross_section_uncert_fb", settings.getValue<double>("cross_section_uncert_fb")); }
    }

    // Pass options to the likelihood function
    calc_LHC_LogLikes.setOption<int>("covariance_nsamples_start", settings.getValue<int>("covariance_nsamples_start"));
    calc_LHC_LogLikes.setOption<double>("covariance_marg_convthres_abs", settings.getValue<double>("covariance_marg_convthres_abs"));
    calc_LHC_LogLikes.setOption<double>("covariance_marg_convthres_rel", settings.getValue<double>("covariance_marg_convthres_rel"));

    // Resolve ColliderBit dependencies and backend requirements
    calc_combined_LHC_LogLike.resolveDependency(&get_LHC_LogLike_per_analysis);
    calc_combined_LHC_LogLike.resolveDependency(&operateLHCLoop);
    get_LHC_LogLike_per_analysis.resolveDependency(&calc_LHC_LogLikes);
    calc_LHC_LogLikes.resolveDependency(&CollectAnalyses);
    calc_LHC_LogLikes.resolveDependency(&operateLHCLoop);
    calc_LHC_LogLikes.resolveBackendReq(use_lnpiln ? &nulike_lnpiln : &nulike_lnpin);
    CollectAnalyses.resolveDependency(&runATLASAnalyses);
    CollectAnalyses.resolveDependency(&runCMSAnalyses);
    CollectAnalyses.resolveDependency(&runIdentityAnalyses);
    runATLASAnalyses.resolveDependency(&getATLASAnalysisContainer);
    runATLASAnalyses.resolveDependency(&smearEventATLAS);
    runCMSAnalyses.resolveDependency(&getCMSAnalysisContainer);
    runCMSAnalyses.resolveDependency(&smearEventCMS);
    runIdentityAnalyses.resolveDependency(&getIdentityAnalysisContainer);
    runIdentityAnalyses.resolveDependency(&copyEvent);
    getATLASAnalysisContainer.resolveDependency(&getYAMLxsec);
    getCMSAnalysisContainer.resolveDependency(&getYAMLxsec);
    getIdentityAnalysisContainer.resolveDependency(&getYAMLxsec);
    smearEventATLAS.resolveDependency(&getBuckFastATLAS);
    smearEventATLAS.resolveDependency(&getLHEvent);
    smearEventCMS.resolveDependency(&getBuckFastCMS);
    smearEventCMS.resolveDependency(&getLHEvent);
    copyEvent.resolveDependency(&getBuckFastIdentity);
    copyEvent.resolveDependency(&getLHEvent);

    // Resolve loop manager for ColliderBit event loop
    getLHEvent.resolveLoopManager(&operateLHCLoop);
    getBuckFastATLAS.resolveLoopManager(&operateLHCLoop);
    getBuckFastCMS.resolveLoopManager(&operateLHCLoop);
    getBuckFastIdentity.resolveLoopManager(&operateLHCLoop);
    getATLASAnalysisContainer.resolveLoopManager(&operateLHCLoop);
    getCMSAnalysisContainer.resolveLoopManager(&operateLHCLoop);
    getIdentityAnalysisContainer.resolveLoopManager(&operateLHCLoop);
    smearEventATLAS.resolveLoopManager(&operateLHCLoop);
    smearEventCMS.resolveLoopManager(&operateLHCLoop);
    copyEvent.resolveLoopManager(&operateLHCLoop);
    getYAMLxsec.resolveLoopManager(&operateLHCLoop);
    runATLASAnalyses.resolveLoopManager(&operateLHCLoop);
    runCMSAnalyses.resolveLoopManager(&operateLHCLoop);
    runIdentityAnalyses.resolveLoopManager(&operateLHCLoop);
    std::vector<functor*> nested_functions = initVector<functor*>(&getLHEvent,
                                                                  &getBuckFastATLAS,
                                                                  &getBuckFastCMS,
                                                                  &getBuckFastIdentity,
                                                                  &getYAMLxsec,
                                                                  &getATLASAnalysisContainer,
                                                                  &getCMSAnalysisContainer,
                                                                  &getIdentityAnalysisContainer,
                                                                  &smearEventATLAS,
                                                                  &smearEventCMS,
                                                                  &copyEvent,
                                                                  &runATLASAnalyses,
                                                                  &runCMSAnalyses,
                                                                  &runIdentityAnalyses);
    operateLHCLoop.setNestedList(nested_functions);

    // Call the initialisation function for nulike
    nulike_1_0_7_init.reset_and_calculate();

    // Run the detector sim and selected analyses on all the events read in.
    operateLHCLoop.reset_and_calculate();
    CollectAnalyses.reset_and_calculate();
    calc_LHC_LogLikes.reset_and_calculate();
    get_LHC_LogLike_per_analysis.reset_and_calculate();
    calc_combined_LHC_LogLike.reset_and_calculate();

    // Retrieve and print the predicted + observed counts and likelihoods for the individual SRs and analyses, as well as the total likelihood.
    long long n_events = getYAMLxsec(0).num_events();
    std::stringstream summary_line;
    for (size_t analysis = 0; analysis < CollectAnalyses(0).size(); ++analysis)
    {
      const Gambit::ColliderBit::AnalysisData& adata = *(CollectAnalyses(0).at(analysis));
      const str& analysis_name = adata.analysis_name;
      const Gambit::ColliderBit::AnalysisLogLikes& analysis_loglikes = calc_LHC_LogLikes(0).at(analysis_name);
      summary_line << "  " << analysis_name << ": " << endl;
      for (size_t sr_index = 0; sr_index < adata.size(); ++sr_index)
      {
        const Gambit::ColliderBit::SignalRegionData srData = adata[sr_index];
        const double abs_uncertainty_s_stat = (srData.n_signal == 0 ? 0 : sqrt(srData.n_signal) * (srData.n_signal_at_lumi/srData.n_signal));
        const double abs_uncertainty_s_sys = srData.signal_sys;
        const double combined_s_uncertainty = HEPUtils::add_quad(abs_uncertainty_s_stat, abs_uncertainty_s_sys);
        const double abs_uncertainty_bg_stat = (srData.n_background == 0 ? 0 : sqrt(srData.n_background));
        const double abs_uncertainty_bg_sys = srData.background_sys;
        const double combined_bg_uncertainty = HEPUtils::add_quad(abs_uncertainty_bg_stat, abs_uncertainty_bg_sys);
        summary_line << "    Signal region " << srData.sr_label << " (SR index " << sr_index << "):" << endl;
        summary_line << "      Observed events: " << srData.n_observed << endl;
        summary_line << "      SM prediction: " << srData.n_background << " +/- " << combined_bg_uncertainty << endl;
        summary_line << "      Signal prediction: " << srData.n_signal_at_lumi << " +/- " << combined_s_uncertainty << endl;
        auto loglike_it = analysis_loglikes.sr_loglikes.find(srData.sr_label);
        if (loglike_it != analysis_loglikes.sr_loglikes.end())
        {
          summary_line << "      Log-likelihood: " << loglike_it->second << endl;
        }
      }
      summary_line << "    Selected signal region: " << analysis_loglikes.combination_sr_label << endl;
      summary_line << "    Total log-likelihood for analysis:" << analysis_loglikes.combination_loglike << endl << endl;
    }
    double loglike = calc_combined_LHC_LogLike(0);

    cout.precision(5);
    cout << endl;
    cout << "Read and analysed " << n_events << " events from LHE file." << endl << endl;
    cout << "Analysis details:" << endl << endl << summary_line.str() << endl;
    cout << std::scientific << "Total combined ATLAS+CMS log-likelihood: " << loglike << endl;
    cout << endl;

    // No more to see here folks, go home.
    return 0;
  }

  catch (std::exception& e)
  {
    cerr << "CBS has exited with fatal exception: " << e.what() << endl;
  }

  // Finished, but an exception was raised.
  return 1;

}

