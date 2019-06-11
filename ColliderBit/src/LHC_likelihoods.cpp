//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  ColliderBit LHC signal and likelihood functions.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Aldo Saavedra
///
///  \author Andy Buckley
///
///  \author Chris Rogan
///          (crogan@cern.ch)
///  \date 2014 Aug
///  \date 2015 May
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jul
///  \date 2018 Jan
///  \date 2019 Jan
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date   2017 March
///  \date   2018 Jan
///  \date   2018 May
///
///  *********************************************

#include <string>
#include <sstream>

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/ColliderBit/ColliderBit_rollcall.hpp"

#include "Eigen/Eigenvalues"
#include <gsl/gsl_sf_gamma.h>
#include "gambit/ColliderBit/multimin.h"

// #define COLLIDERBIT_DEBUG

namespace Gambit
{

  namespace ColliderBit
  {


    /// Loop over all analyses and fill a map of predicted counts
    void calc_LHC_signals(map_str_dbl& result)
    {
      using namespace Pipes::calc_LHC_signals;

      // Clear the result map
      result.clear();

      std::stringstream summary_line;
      summary_line << "LHC signals per SR: ";

      // Loop over analyses and collect the predicted events into the map
      for (size_t analysis = 0; analysis < Dep::AllAnalysisNumbers->size(); ++analysis)
      {
        // AnalysisData for this analysis
        const AnalysisData& adata = *(Dep::AllAnalysisNumbers->at(analysis));

        summary_line << adata.analysis_name << ": ";

        // Loop over the signal regions inside the analysis, and save the predicted number of events for each.
        for (size_t SR = 0; SR < adata.size(); ++SR)
        {
          // Save SR numbers and absolute uncertainties
          const SignalRegionData srData = adata[SR];
          const str key = adata.analysis_name + "__" + srData.sr_label + "__i" + std::to_string(SR) + "__signal";
          result[key] = srData.n_signal_at_lumi;
          const double abs_uncertainty_s_stat = (srData.n_signal == 0 ? 0 : sqrt(srData.n_signal) * (srData.n_signal_at_lumi/srData.n_signal));
          const double abs_uncertainty_s_sys = srData.signal_sys;
          const double combined_uncertainty = HEPUtils::add_quad(abs_uncertainty_s_stat, abs_uncertainty_s_sys);
          result[key + "_uncert"] = combined_uncertainty;

          summary_line << srData.sr_label + "__i" + std::to_string(SR) << ":" << srData.n_signal_at_lumi << "+-" << combined_uncertainty << ", ";
        }
      }
      logger() << LogTags::debug << summary_line.str() << EOM;
    }




    /// Loglike objective-function wrapper to provide the signature for GSL multimin
    ///
    /// @todo Use this to compute both the b and s+b likelihood, and compute
    /// the b likelihood only once, via s = 0
    void _gsl_calc_Analysis_MinusLogLike(const size_t n, const double* unit_nuisances_dbl,
                                         void* npreds_nobss_sqrtevals_evecs, double* fval) {

      // Convert the array of doubles into an "Eigen view" of the nuisance params
      Eigen::Map<const Eigen::VectorXd> unit_nuisances(&unit_nuisances_dbl[0], n);

      // Convert the linearised array of doubles into "Eigen views" of the fixed params
      double *npreds_nobss_sqrtevals_evecs_dbl = (double*) npreds_nobss_sqrtevals_evecs;
      Eigen::Map<const Eigen::ArrayXd> n_preds_nominal(&npreds_nobss_sqrtevals_evecs_dbl[0], n);
      Eigen::Map<const Eigen::ArrayXd> n_obss(&npreds_nobss_sqrtevals_evecs_dbl[n], n);
      Eigen::Map<const Eigen::ArrayXd> sqrtevals(&npreds_nobss_sqrtevals_evecs_dbl[2*n], n);
      Eigen::Map<const Eigen::MatrixXd> evecs(&npreds_nobss_sqrtevals_evecs_dbl[3*n], n, n);

      // Rotate rate deltas into the SR basis and shift by SR mean rates
      const Eigen::VectorXd n_preds = n_preds_nominal + (evecs*unit_nuisances).array();

      // Calculate each SR's Poisson likelihood and add to composite likelihood calculation
      double loglike_tot = 0;
      for (int j = 0; j < unit_nuisances.size(); ++j) {
        // First the multivariate Gaussian bit (j = nuisance)
        const double pnorm_j = -pow(unit_nuisances(j), 2)/2. - log(1/sqrt(2*M_PI));
        loglike_tot += pnorm_j;
        // Then the Poisson bit (j = SR)
        /// @note We've dropped the log(n_obs!) terms, since they're expensive and cancel in computing DLL
        const double lambda_j = std::max(n_preds(j), 1e-3); //< manually avoid <= 0 rates
        const double logfact_n_obs = 0; // gsl_sf_lngamma(n_obs(j) + 1); //< skipping log(n_obs!) computation
        const double loglike_j = n_obss(j)*log(lambda_j) - lambda_j - logfact_n_obs;
        loglike_tot += loglike_j;
      }

      // Output via argument (invert to return -LL for minimisation)
      *fval = -loglike_tot;
    }


    std::vector<double> _gsl_mkpackedarray(const Eigen::ArrayXd& n_preds,
                                           const Eigen::ArrayXd& n_obss,
                                           const Eigen::ArrayXd& sqrtevals,
                                           const Eigen::MatrixXd& evecs) {
      const size_t nSR = n_obss.size();
      std::vector<double> fixeds(nSR + nSR + nSR + nSR*nSR, 0.0);
      for (size_t i = 0; i < nSR; ++i) {
        fixeds[0+i] = n_preds(i);
        fixeds[nSR+i] = n_obss(i);
        fixeds[2*nSR+i] = sqrtevals(i);
        for (size_t j = 0; j < nSR; ++j)
          fixeds[3*nSR+i*nSR+j] = evecs(i,j); ///< @todo Double-check ordering... not that it matters
      }
      return fixeds;
    }


    /// Return the best log likelihood
    /// @note Return value is missing the log(n_obs!) terms (n_SR of them) which cancel in LLR calculation
    double profile_loglike_cov(const Eigen::ArrayXd& n_preds,
                               const Eigen::ArrayXd& n_obss,
                               const Eigen::ArrayXd& sqrtevals,
                               const Eigen::MatrixXd& evecs) {

      // Number of signal regions
      const size_t nSR = n_obss.size();

      // Start with nuisances at nominal values
      /// @todo Pick a more informed starting position
      std::vector<double> nuisances(nSR, 0.0);

      // Optimiser parameters
      // Params: step1size, tol, maxiter, epsabs, simplex maxsize, method, verbosity
      // Methods:
      //  0: Fletcher-Reeves conjugate gradient
      //  1: Polak-Ribiere conjugate gradient
      //  2: Vector Broyden-Fletcher-Goldfarb-Shanno method
      //  3: Steepest descent algorithm
      //  4: Nelder-Mead simplex
      //  5: Vector Broyden-Fletcher-Goldfarb-Shanno method ver. 2
      //  6: Simplex algorithm of Nelder and Mead ver. 2
      //  7: Simplex algorithm of Nelder and Mead: random initialization
      using namespace Pipes::calc_LHC_LogLikes;
      static const double INITIAL_STEP = runOptions->getValueOrDef<double>(0.1, "covariance_prof_initstep");
      static const double CONV_TOL = runOptions->getValueOrDef<double>(0.01, "covariance_prof_convtol");
      static const unsigned MAXSTEPS = runOptions->getValueOrDef<unsigned>(10000, "covariance_prof_maxsteps");
      static const double CONV_ACC = runOptions->getValueOrDef<double>(0.01, "covariance_prof_convacc");
      static const double SIMPLEX_SIZE = runOptions->getValueOrDef<double>(1e-5, "covariance_prof_simplexsize");
      static const unsigned METHOD = runOptions->getValueOrDef<unsigned>(6, "covariance_prof_method");
      static const unsigned VERBOSITY = runOptions->getValueOrDef<unsigned>(0, "covariance_prof_verbosity");
      static const struct multimin_params oparams = {INITIAL_STEP, CONV_TOL, MAXSTEPS, CONV_ACC, SIMPLEX_SIZE, METHOD, VERBOSITY};

      // Convert the linearised array of doubles into "Eigen views" of the fixed params
      std::vector<double> fixeds = _gsl_mkpackedarray(n_preds, n_obss, sqrtevals, evecs);

      // Pass to the minimiser
      double minusbestll = 999;
      // _gsl_calc_Analysis_MinusLogLike(nSR, &nuisances[0], &fixeds[0], &minusbestll);
      multimin(nSR, &nuisances[0], &minusbestll,
               nullptr, nullptr, nullptr,
               _gsl_calc_Analysis_MinusLogLike,
               nullptr, nullptr,
               &fixeds[0], oparams);

      return -minusbestll;
    }



    double marg_loglike_cov(const Eigen::ArrayXd& n_preds,
                            const Eigen::ArrayXd& n_obss,
                            const Eigen::ArrayXd& sqrtevals,
                            const Eigen::MatrixXd& evecs) {

      // Number of signal regions
      const size_t nSR = n_obss.size();

      // Sample correlated SR rates from a rotated Gaussian defined by the covariance matrix and offset by the mean rates
      using namespace Pipes::calc_LHC_LogLikes;
      static const double CONVERGENCE_TOLERANCE_ABS = runOptions->getValueOrDef<double>(0.05, "covariance_marg_convthres_abs");
      static const double CONVERGENCE_TOLERANCE_REL = runOptions->getValueOrDef<double>(0.05, "covariance_marg_convthres_rel");
      static const size_t NSAMPLE_INPUT = runOptions->getValueOrDef<size_t>(100000, "covariance_nsamples_start");

      // Dynamic convergence control & test variables
      size_t nsample = NSAMPLE_INPUT;
      bool first_iteration = true;
      double diff_abs = 9999;
      double diff_rel = 1;

      // Likelihood variables (note use of long double to guard against blow-up of L as opposed to log(L1/L0))
      long double ana_like_prev = 1;
      long double ana_like = 1;
      long double lsum_prev = 0;

      // Sampler for unit-normal nuisances
      std::normal_distribution<double> unitnormdbn(0,1);

      // Log factorial of observed number of events.
      // Currently use the ln(Gamma(x)) function gsl_sf_lngamma from GSL. (Need continuous function.)
      // We may want to switch to using Stirling's approximation: ln(n!) ~ n*ln(n) - n
      Eigen::ArrayXd logfact_n_obss(nSR);
      for (size_t j = 0; j < nSR; ++j)
        logfact_n_obss(j) = gsl_sf_lngamma(n_obss(j) + 1);

      // Check absolute difference between independent estimates
      /// @todo Should also implement a check of relative difference
      while ((diff_abs > CONVERGENCE_TOLERANCE_ABS && diff_rel > CONVERGENCE_TOLERANCE_REL) || 1.0/sqrt(nsample) > CONVERGENCE_TOLERANCE_ABS)
      {
        long double lsum = 0;

        /// @note How to correct negative rates? Discard (scales badly), set to
        /// epsilon (= discontinuous & unphysical pdf), transform to log-space
        /// (distorts the pdf quite badly), or something else (skew term)?
        /// We're using the "set to epsilon" version for now.
        /// Ben: I would vote for 'discard'. It can't be that inefficient, surely?
        /// Andy: For a lot of signal regions, the probability of none having a negative sample is Prod_SR p(non-negative)_SR... which *can* get bad.

        #pragma omp parallel
        {
          // Sample correlated SR rates from a rotated Gaussian defined by the covariance matrix and offset by the mean rates
          double lsum_private  = 0;
          #pragma omp for nowait
          for (size_t i = 0; i < nsample; ++i) {

            Eigen::VectorXd norm_samples(nSR);
            for (size_t j = 0; j < nSR; ++j)
              norm_samples(j) = sqrtevals(j) * unitnormdbn(Random::rng());

            // Rotate rate deltas into the SR basis and shift by SR mean rates
            const Eigen::VectorXd n_pred_samples  = n_preds + (evecs*norm_samples).array();

            // Calculate Poisson likelihood and add to composite likelihood calculation
            double combined_loglike = 0;
            for (size_t j = 0; j < nSR; ++j) {
              const double lambda_j = std::max(n_pred_samples(j), 1e-3); //< manually avoid <= 0 rates
              const double loglike_j  = n_obss(j)*log(lambda_j) - lambda_j - logfact_n_obss(j);
              combined_loglike += loglike_j;
            }
            // Add combined likelihood to running sums (to later calculate averages)
            lsum_private += exp(combined_loglike);
          }

          #pragma omp critical
          {
            lsum  += lsum_private;
          }
        } // End omp parallel

        // Compare convergence to previous independent batch
        if (first_iteration)  // The first round must be generated twice
        {
          lsum_prev = lsum;
          first_iteration = false;
        }
        else
        {
          ana_like_prev = lsum_prev / (double)nsample;
          ana_like = lsum / (double)nsample;
          diff_abs = fabs(ana_like_prev - ana_like);
          diff_rel = diff_abs/ana_like;

          // Update variables
          lsum_prev += lsum;  // Aggregate result. This doubles the effective batch size for lsum_prev.
          nsample *=2;  // This ensures that the next batch for lsum is as big as the current batch size for lsum_prev, so they can be compared directly.
        }

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix()
             << "diff_rel: " << diff_rel << endl
             << "   diff_abs: " << diff_abs << endl
             << "   logl: " << log(ana_like) << endl;
        cout << debug_prefix() << "nsample for the next iteration is: " << nsample << endl;
        cout << debug_prefix() << endl;
        #endif
      }
      // End convergence while-loop

      // Combine the independent estimates ana_like and ana_like_prev.
      // Use equal weights since the estimates are based on equal batch sizes.
      ana_like = 0.5*(ana_like + ana_like_prev);
      const double ana_margll = log(ana_like);
      #ifdef COLLIDERBIT_DEBUG
      cout << debug_prefix() << "Combined estimate: ana_loglike: " << ana_margll << "   (based on 2*nsample=" << 2*nsample << " samples)" << endl;
      #endif

      return ana_margll;
    }




    /// Loop over all analyses and fill a map of AnalysisLogLikes objects
    void calc_LHC_LogLikes(map_str_AnalysisLogLikes& result)
    {
      // Read options
      using namespace Pipes::calc_LHC_LogLikes;
      // Use covariance matrices if available?
      static const bool USE_COVAR = runOptions->getValueOrDef<bool>(true, "use_covariances");
      // Use marginalisation rather than profiling (probably less stable)?
      static const bool USE_MARG = runOptions->getValueOrDef<bool>(false, "use_marginalising");

      // Fix the profiling/marginalising function according to the option
      auto marg_prof_fn = USE_MARG ? marg_loglike_cov : profile_loglike_cov;


      // Clear the result map
      result.clear();

      // Main loop over all analyses to compute DLL = LL_sb - LL_b
      for (size_t analysis = 0; analysis < Dep::AllAnalysisNumbers->size(); ++analysis)
      {


        // AnalysisData for this analysis
        const AnalysisData& adata = *(Dep::AllAnalysisNumbers->at(analysis));
        const std::string ananame = adata.analysis_name;
        const size_t nSR = adata.size();
        const bool has_covar = adata.srcov.rows() > 0;

        #ifdef COLLIDERBIT_DEBUG
        std::streamsize stream_precision = cout.precision();  // get current precision
        cout.precision(2);  // set precision
        cout << debug_prefix() << "calc_LHC_LogLikes: " << "Will print content of " << ananame << " signal regions:" << endl;
        for (size_t SR = 0; SR < adata.size(); ++SR)
        {
          const SignalRegionData& srData = adata[SR];
          cout << std::fixed << debug_prefix()
                                 << "calc_LHC_LogLikes: " << ananame
                                 << ", " << srData.sr_label
                                 << ",  n_b = " << srData.n_background << " +/- " << srData.background_sys
                                 << ",  n_obs = " << srData.n_observed
                                 << ",  excess = " << srData.n_observed - srData.n_background << " +/- " << srData.background_sys
                                 << ",  n_s = " << srData.n_signal_at_lumi
                                 << ",  (excess-n_s) = " << (srData.n_observed-srData.n_background) - srData.n_signal_at_lumi << " +/- " << srData.background_sys
                                 << ",  n_s_MC = " << srData.n_signal
                                 << endl;
        }
        cout.precision(stream_precision); // restore previous precision
        #endif


        // Shortcut #1
        //
        // If no events have been generated (xsec veto) or too many events have
        // failed, short-circut the loop and return delta log-likelihood = 0 for
        // every SR in each analysis.
        //
        /// @todo Needs more sophistication once we add analyses that don't use event generation.
        if (not Dep::RunMC->event_generation_began || Dep::RunMC->exceeded_maxFailedEvents)
        {
          // If this is an analysis with covariance info, only add a single 0-entry in the map
          if (USE_COVAR && has_covar)
          {
            result[ananame].combination_sr_label = "none";
            result[ananame].combination_loglike = 0.0;
          }
          // If this is an analysis without covariance info, add 0-entries for all SRs plus
          // one for the combined LogLike
          else
          {
            for (size_t SR = 0; SR < adata.size(); ++SR)
            {
              result[ananame].sr_indices[adata[SR].sr_label] = SR;
              result[ananame].sr_loglikes[adata[SR].sr_label] = 0.0;
            }
            result[ananame].combination_sr_label = "none";
            result[ananame].combination_loglike = 0.0;
          }

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLikes: " << ananame << "_LogLike : " << 0.0 << " (No events predicted / successfully generated. Skipped full calculation.)" << endl;
          #endif

          // Continue to next analysis
          continue;
        }


        // Shortcut #2
        //
        // If all SRs have 0 signal prediction, we know the delta log-likelihood is 0.
        bool all_zero_signal = true;
        for (size_t SR = 0; SR < nSR; ++SR)
        {
          if (adata[SR].n_signal != 0)
          {
            all_zero_signal = false;
            break;
          }
        }
        if (all_zero_signal)
        {
          // Store result
          result[ananame].combination_sr_label = "all";
          result[ananame].combination_sr_index = -1;
          result[ananame].combination_loglike = 0.0;

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLikes: " << ananame << "_LogLike : " << 0.0 << " (No signal predicted. Skipped full calculation.)" << endl;
          #endif

          // Continue to next analysis
          continue;
        }


        // Work out the total (delta) log likelihood for this analysis, with correlations as available/instructed
        double ana_dll = NAN;
        if (USE_COVAR && has_covar)
        {


          /// If (simplified) SR-correlation info is available, so use the
          /// covariance matrix to construct composite marginalised likelihood
          /// Despite initial thoughts, we can't just do independent LL
          /// calculations in a rotated basis, but have to sample from the
          /// covariance matrix.
          ///
          /// @note This means we can't use the nulike LL functions, which
          /// operate in 1D only.  Also, log-normal sampling in the diagonal
          /// basis is not helpful, since the rotation will re-generate negative
          /// rates.
          ///
          /// @todo Support NSL, i.e. skewness correction
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLikes: Analysis " << analysis << " has a covariance matrix: computing composite loglike." << endl;
          #endif


          // Construct vectors of SR numbers
          /// @todo Unify this for both cov and no-cov, feeding in one-element Eigen blocks as Ref<>s for the latter
          Eigen::ArrayXd n_obs(adata.size()); // logfact_n_obs(adata.size());
          Eigen::ArrayXd n_pred_b(adata.size()), n_pred_sb(adata.size()), abs_unc_s(adata.size());
          for (size_t SR = 0; SR < adata.size(); ++SR)
          {
            const SignalRegionData& srData = adata[SR];

            // Actual observed number of events
            n_obs(SR) = srData.n_observed;

            // Log factorial of observed number of events.
            // Currently use the ln(Gamma(x)) function gsl_sf_lngamma from GSL. (Need continuous function.)
            // We may want to switch to using Stirling's approximation: ln(n!) ~ n*ln(n) - n
            //logfact_n_obs(SR) = gsl_sf_lngamma(n_obs(SR) + 1.);

            // A contribution to the predicted number of events that is not known exactly
            n_pred_b(SR) = srData.n_background;
            n_pred_sb(SR) = srData.n_signal_at_lumi + srData.n_background;

            // Absolute errors for n_predicted_uncertain_*
            const double abs_uncertainty_s_stat = (srData.n_signal == 0 ? 0 : sqrt(srData.n_signal) * (srData.n_signal_at_lumi/srData.n_signal));
            const double abs_uncertainty_s_sys = srData.signal_sys;
            abs_unc_s(SR) = HEPUtils::add_quad(abs_uncertainty_s_stat, abs_uncertainty_s_sys);
          }

          // Diagonalise the background-only covariance matrix, extracting the rotation matrix
          /// @todo Compute the background-only covariance decomposition and likelihood only once
          const Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig_b(adata.srcov);
          const Eigen::ArrayXd Eb = eig_b.eigenvalues();
          const Eigen::ArrayXd sqrtEb = Eb.sqrt();
          const Eigen::MatrixXd Vb = eig_b.eigenvectors();

          // Construct and diagonalise the s+b covariance matrix, adding the diagonal signal uncertainties in quadrature
          const Eigen::MatrixXd srcov_s = abs_unc_s.array().square().matrix().asDiagonal();
          const Eigen::MatrixXd srcov_sb = adata.srcov + srcov_s;
          const Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig_sb(srcov_sb);
          const Eigen::ArrayXd Esb = eig_sb.eigenvalues();
          const Eigen::ArrayXd sqrtEsb = Esb.sqrt();
          const Eigen::MatrixXd Vsb = eig_sb.eigenvectors();


          // Compute the single, correlated analysis-level DLL as the difference of s+b and b (partial) LLs
          /// @todo Only compute this once per run
          const double ll_b = marg_prof_fn(n_pred_b, n_obs, sqrtEb, Vb);
          const double ll_sb = marg_prof_fn(n_pred_sb, n_obs, sqrtEsb, Vsb);
          ana_dll = ll_sb - ll_b;


          // Store result
          result[ananame].combination_sr_label = "all";
          result[ananame].combination_sr_index = -1;
          result[ananame].combination_loglike = ana_dll;

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLikes: " << ananame << "_LogLike : " << ana_dll << endl;
          #endif


        } else { // NO SR-CORRELATION INFO, OR USER CHOSE NOT TO USE IT:


          // We either take the result from the SR *expected* to be most
          // constraining under the s=0 assumption (default), or naively combine
          // the loglikes for all SRs (if combine_SRs_without_covariances=true).
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLikes: Analysis " << analysis << " has no covariance matrix: computing single best-expected loglike." << endl;
          #endif

          double bestexp_dll_exp = 0, bestexp_dll_obs = 0;
          str bestexp_sr_label;
          int bestexp_sr_index;
          double nocovar_srsum_dll_obs = 0;

          for (size_t SR = 0; SR < adata.size(); ++SR)
          {
            const SignalRegionData& srData = adata[SR];

            // A contribution to the predicted number of events that is not known exactly
            const double n_pred_b = srData.n_background;
            const double n_pred_sb = n_pred_b + srData.n_signal_at_lumi;

            // Actual observed number of events and predicted background, as integers cf. Poisson stats
            const double n_obs = round(srData.n_observed);
            const double n_pred_b_int = round(n_pred_b);

            // Absolute errors for n_predicted_uncertain_*
            const double abs_uncertainty_s_stat = (srData.n_signal == 0 ? 0 : sqrt(srData.n_signal) * (srData.n_signal_at_lumi/srData.n_signal));
            const double abs_uncertainty_s_sys = srData.signal_sys;
            const double abs_uncertainty_b = srData.background_sys;
            const double abs_uncertainty_sb = HEPUtils::add_quad(abs_uncertainty_s_stat, abs_uncertainty_s_sys, abs_uncertainty_b);

            // Construct dummy 1-element Eigen objects for passing to the general likelihood calculator
            /// @todo Use newer (?) one-step Eigen constructors for (const) single-element arrays
            Eigen::ArrayXd n_obss(1);        n_obss(0) = n_obs;
            Eigen::ArrayXd n_preds_b_int(1); n_preds_b_int(0) = n_pred_b_int;
            Eigen::ArrayXd n_preds_b(1);     n_preds_b(0) = n_pred_b;
            Eigen::ArrayXd n_preds_sb(1);    n_preds_sb(0) = n_pred_sb;
            Eigen::ArrayXd sqrtevals_b(1);   sqrtevals_b(0) = abs_uncertainty_b;
            Eigen::ArrayXd sqrtevals_sb(1);  sqrtevals_sb(0) = abs_uncertainty_sb;
            Eigen::MatrixXd evecs_dummy(1,1); evecs_dummy(0,0) = 1.0;


            // Compute this SR's DLLs as the differences of s+b and b (partial) LLs
            /// @todo Or compute all the exp DLLs first, then only the best-expected SR's obs DLL?
            /// @todo Only compute this once per run
            const double ll_b_exp = marg_prof_fn(n_preds_b, n_preds_b_int, sqrtevals_b, evecs_dummy);
            /// @todo Only compute this once per run
            const double ll_b_obs = marg_prof_fn(n_preds_b, n_obss, sqrtevals_b, evecs_dummy);
            const double ll_sb_exp = marg_prof_fn(n_preds_sb, n_preds_b_int, sqrtevals_sb, evecs_dummy);
            const double ll_sb_obs = marg_prof_fn(n_preds_sb, n_obss, sqrtevals_sb, evecs_dummy);

            /// @todo Why the flipped sign convention? Remove?
            const double dll_exp = -( ll_sb_exp - ll_b_exp );
            const double dll_obs = -( ll_sb_obs - ll_b_obs );


            // // Calculate the expected dll and set the bestexp values for exp and obs dll if this one is the best so far
            #ifdef COLLIDERBIT_DEBUG
            cout << debug_prefix() << ananame << ", " << srData.sr_label << ",  llsb_exp-llb_exp = " << -dll_exp << ",  llsb_obs-llb_obs= " << -dll_obs << endl;
            #endif

            if (dll_exp > bestexp_dll_exp || SR == 0)
            {
              bestexp_dll_exp = dll_exp;
              bestexp_dll_obs = dll_obs;
              bestexp_sr_label = srData.sr_label;
              bestexp_sr_index = SR;
              // #ifdef COLLIDERBIT_DEBUG
              // cout << debug_prefix() << "Setting bestexp_sr_label to: " << bestexp_sr_label << ", LogL_exp = " << -bestexp_dll_exp << ", LogL_obs = " << -bestexp_dll_obs << endl;
              // #endif
            }

            // Store "observed LogLike" result for this SR
            result[ananame].sr_indices[srData.sr_label] = SR;
            result[ananame].sr_loglikes[srData.sr_label] = -dll_obs; ///< @todo Argh, sign convention mess!

            // Add loglike to the no-correlations loglike sum over SRs
            nocovar_srsum_dll_obs += -dll_obs; ///< @todo Argh, sign convention mess!
          }

          // Set total analysis DLL
          ana_dll = bestexp_dll_obs;


          // Set this analysis' total obs dLL to that from the best-expected SR (with conversion to more negative dll = more exclusion convention)
          // result[ananame] = -bestexp_dll_obs;
          result[ananame].combination_sr_label = bestexp_sr_label;
          result[ananame].combination_sr_index = bestexp_sr_index;
          result[ananame].combination_loglike = -ana_dll; ///< @todo Argh, sign convention mess!

          // Should we use the naive sum of SR loglikes (without correlations), instead of the best-expected SR?
          static const bool combine_nocovar_SRs = runOptions->getValueOrDef<bool>(false, "combine_SRs_without_covariances");
          if (combine_nocovar_SRs)
          {
            result[ananame].combination_loglike = nocovar_srsum_dll_obs;
          }

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLikes: " << ananame << "_" << bestexp_sr_label << "_LogLike : " << -ana_dll << endl; ///< @todo Argh, sign convention mess!
          #endif

        } // end cov/no-cov


        // Check for problems with the result
        if (Utils::isnan(ana_dll))
        {
          std::stringstream msg;
          msg << "Computation of composite loglike for analysis " << ananame << " returned NaN" << endl;
          msg << "Will now print the signal region data for this analysis:" << endl;
          for (size_t SR = 0; SR < adata.size(); ++SR)
          {
            const SignalRegionData& srData = adata[SR];
            msg << srData.sr_label
                << ",  n_background = " << srData.n_background
                << ",  background_sys = " << srData.background_sys
                << ",  n_observed = " << srData.n_observed
                << ",  n_signal_at_lumi = " << srData.n_signal_at_lumi
                << ",  n_signal = " << srData.n_signal
                << ",  signal_sys = " << srData.signal_sys
                << endl;
          }
          invalid_point().raise(msg.str());
        }


      }

    }


    /// Extract the combined log likelihood for each analysis
    void get_LHC_LogLike_per_analysis(map_str_dbl& result)
    {
      using namespace Pipes::get_LHC_LogLike_per_analysis;

      std::stringstream summary_line;
      summary_line << "LHC loglikes per analysis: ";

      for (const std::pair<str,AnalysisLogLikes>& pair : *Dep::LHC_LogLikes)
      {
        const str& analysis_name = pair.first;
        const AnalysisLogLikes& analysis_loglikes = pair.second;

        result[analysis_name] = analysis_loglikes.combination_loglike;

        summary_line << analysis_name << ":" << analysis_loglikes.combination_loglike << ", ";
      }
      logger() << LogTags::debug << summary_line.str() << EOM;
    }


    /// Extract the log likelihood for each SR
    void get_LHC_LogLike_per_SR(map_str_dbl& result)
    {
      using namespace Pipes::get_LHC_LogLike_per_SR;

      std::stringstream summary_line;
      summary_line << "LHC loglikes per SR: ";

      for (const std::pair<str,AnalysisLogLikes>& pair_i : *Dep::LHC_LogLikes)
      {
        const str& analysis_name = pair_i.first;
        const AnalysisLogLikes& analysis_loglikes = pair_i.second;

        summary_line << analysis_name << ": ";

        for (const std::pair<str,double>& pair_j : analysis_loglikes.sr_loglikes)
        {
          const str& sr_label = pair_j.first;
          const double& sr_loglike = pair_j.second;
          const int sr_index = analysis_loglikes.sr_indices.at(sr_label);

          const str key = analysis_name + "__" + sr_label + "__i" + std::to_string(sr_index) + "__LogLike";
          result[key] = sr_loglike;

          summary_line << sr_label + "__i" + std::to_string(sr_index) << ":" << sr_loglike << ", ";
        }

        result[analysis_name + "__combined_LogLike"] = analysis_loglikes.combination_loglike;

        summary_line << "combined_LogLike:" << analysis_loglikes.combination_loglike << ", ";
      }
      logger() << LogTags::debug << summary_line.str() << EOM;
    }


    /// Extract the labels for the SRs used in the analysis loglikes
    void get_LHC_LogLike_SR_labels(map_str_str& result)
    {
      using namespace Pipes::get_LHC_LogLike_per_SR;
      for (const std::pair<str,AnalysisLogLikes>& pair_i : *Dep::LHC_LogLikes)
      {
        const str& analysis_name = pair_i.first;
        const AnalysisLogLikes& analysis_loglikes = pair_i.second;

        result[analysis_name] = analysis_loglikes.combination_sr_label;
      }
    }


    /// Extract the indices for the SRs used in the analysis loglikes
    /// @todo Switch result type to map_str_int once we have implemented a printer for this type
    void get_LHC_LogLike_SR_indices(map_str_dbl& result)
    {
      using namespace Pipes::get_LHC_LogLike_per_SR;

      std::stringstream summary_line;
      summary_line << "LHC loglike SR indices: ";

      // Loop over analyses
      for (const std::pair<str,AnalysisLogLikes>& pair_i : *Dep::LHC_LogLikes)
      {
        const str& analysis_name = pair_i.first;
        const AnalysisLogLikes& analysis_loglikes = pair_i.second;

        result[analysis_name] = (double) analysis_loglikes.combination_sr_index;

        summary_line << analysis_name << ":" << analysis_loglikes.combination_sr_index << ", ";
      }
      logger() << LogTags::debug << summary_line.str() << EOM;
    }


    /// Compute the total likelihood combining all analyses
    void calc_combined_LHC_LogLike(double& result)
    {
      using namespace Pipes::calc_combined_LHC_LogLike;
      result = 0.0;

      // Read analysis names from the yaml file
      std::vector<str> default_skip_analyses;  // The default is empty lists of analyses to skip
      static const std::vector<str> skip_analyses = runOptions->getValueOrDef<std::vector<str> >(default_skip_analyses, "skip_analyses");

      // If too many events have failed, do the conservative thing and return delta log-likelihood = 0
      if (Dep::RunMC->exceeded_maxFailedEvents)
      {
        #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_combined_LHC_LogLike: Too many failed events. Will be conservative and return a delta log-likelihood of 0." << endl;
        #endif
        return;
      }

      // Loop over analyses and calculate the total observed dLL
      for (auto const& analysis_loglike_pair : *Dep::LHC_LogLike_per_analysis)
      {
        const str& analysis_name = analysis_loglike_pair.first;
        const double& analysis_loglike = analysis_loglike_pair.second;

        // If the analysis name is in skip_analyses, don't add its loglike to the total loglike.
        if (std::find(skip_analyses.begin(), skip_analyses.end(), analysis_name) != skip_analyses.end())
        {
          #ifdef COLLIDERBIT_DEBUG
            cout.precision(5);
            cout << debug_prefix() << "calc_combined_LHC_LogLike: Leaving out analysis " << analysis_name << " with LogL = " << analysis_loglike << endl;
          #endif
          continue;
        }

        // Add analysis loglike.
        // If using capped likelihood for each individual analysis, set analysis_loglike = min(analysis_loglike,0)
        static const bool use_cap_loglike_individual = runOptions->getValueOrDef<bool>(false, "cap_loglike_individual_analyses");
        if (use_cap_loglike_individual)
        {
          result += std::min(analysis_loglike, 0.0);
        }
        else
        {
          result += analysis_loglike;
        }

        #ifdef COLLIDERBIT_DEBUG
          cout.precision(5);
          cout << debug_prefix() << "calc_combined_LHC_LogLike: Analysis " << analysis_name << " contributes with a LogL = " << analysis_loglike << endl;
        #endif
      }

      #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "calc_combined_LHC_LogLike: LHC_Combined_LogLike = " << result << endl;
      #endif

      // If using a "global" capped likelihood, set result = min(result,0)
      static const bool use_cap_loglike = runOptions->getValueOrDef<bool>(false, "cap_loglike");
      if (use_cap_loglike)
      {
        result = std::min(result, 0.0);
      }

      std::stringstream summary_line;
      summary_line << "LHC combined loglike:" << result;
      logger() << LogTags::debug << summary_line.str() << EOM;
    }

  }

}
