//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  AnalysisData and SignalRegion structures.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Andy Buckley
///          (andy.buckley@cern.ch)
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date often
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2019 Feb
///
///  *********************************************

#pragma once

#include "Eigen/Core"

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cfloat>
#include <limits>
#include <memory>
#include <iomanip>
#include <algorithm>
#include "gambit/ColliderBit/analyses/EventCounter.hpp"

// #define ANALYSISDATA_DEBUG

#ifdef ANALYSISDATA_DEBUG
#include <iostream>
#endif

namespace Gambit {
  namespace ColliderBit {


    /// A simple container for the result of one signal region from one analysis.
    struct SignalRegionData
    {

      /// Constructor with EventCounter arg for the signal count and SR name
      SignalRegionData(const EventCounter& scounter,
                       double nobs, const std::pair<double,double>& nbkg,
                       double nsigscaled=0)
       : SignalRegionData(scounter.name(), nobs, scounter.weight_sum(), nbkg.first, scounter.weight_sum_err(), nbkg.second, nsigscaled)
      {}

      /// Constructor with EventCounter arg for the signal count, but separate name
      SignalRegionData(const std::string& sr,
                       double nobs, const EventCounter& scounter, const std::pair<double,double>& nbkg,
                       double nsigscaled=0)
       : SignalRegionData(sr, nobs, scounter.weight_sum(), nbkg.first, scounter.weight_sum_err(), nbkg.second, nsigscaled)
      {}

      /// Constructor with {n,nsys} pair args
      SignalRegionData(const std::string& sr,
                       double nobs, const std::pair<double,double>& nsigMC, const std::pair<double,double>& nbkg,
                       double nsigscaled=0)
       : SignalRegionData(sr, nobs, nsigMC.first, nbkg.first, nsigMC.second, nbkg.second, nsigscaled)
      {}

      /// Constructor with separate n & nsys args
      SignalRegionData(const std::string& sr,
                       double nobs, double nsigMC, double nbkg,
                       double nsigMCsys, double nbkgerr, double nsigscaled=0)
       : sr_label(sr),
         n_obs(nobs), n_sig_MC(nsigMC), n_sig_scaled(nsigscaled), n_bkg(nbkg),
         n_sig_MC_sys(nsigMCsys), n_bkg_err(nbkgerr)
      {}

      /// Default constructor
      SignalRegionData() {}

      /// Consistency check
      bool check() const {
        bool consistent = true;
        /// @todo Add SR consistency checks
        return consistent;
      }

      /// Uncertainty calculators
      double scalefactor() const { return n_sig_MC == 0 ? 1 : n_sig_scaled / n_sig_MC; }

      double calc_n_sig_MC_stat() const { return sqrt(n_sig_MC); }

      double calc_n_sig_MC_err() const 
      { 
        double n_sig_MC_stat = calc_n_sig_MC_stat();
        return sqrt( n_sig_MC_stat * n_sig_MC_stat + n_sig_MC_sys * n_sig_MC_sys ); 
      }

      double calc_n_sig_scaled_stat() const { return scalefactor() * calc_n_sig_MC_stat(); }

      double calc_n_sig_scaled_sys() const { return scalefactor() * n_sig_MC_sys; }

      double calc_n_sig_scaled_err() const { return scalefactor() * calc_n_sig_MC_err(); }

      double calc_n_sigbkg_err() const 
      { 
        double n_sig_scaled_err = calc_n_sig_scaled_err();
        return sqrt( n_sig_scaled_err * n_sig_scaled_err + n_bkg_err * n_bkg_err );  
      }

      /// @todo Set up a more complete system of getters/setters and make the member variables private

      /// @name Signal region specification
      //@{
      std::string sr_label; ///< A label for the particular signal region of the analysis
      //@}

      /// @name Signal region data
      //@{
      double n_obs = 0; ///< The number of events passing selection for this signal region as reported by the experiment
      double n_sig_MC = 0; ///< The number of simulated model events passing selection for this signal region
      double n_sig_scaled = 0; ///< n_sig_MC, scaled to luminosity * cross-section
      double n_bkg = 0; ///< The number of standard model events expected to pass the selection for this signal region, as reported by the experiment.
      double n_sig_MC_sys = 0; ///< The absolute systematic error of n_sig_MC
      double n_bkg_err = 0; ///< The absolute error of n_bkg
      //@}

    };


    /// A container for the result of an analysis, potentially with many signal regions and correlations
    ///
    /// @todo Access by name?
    /// @todo Guarantee ordering?
    /// @todo How to combine covariance matrices -- require?
    struct AnalysisData
    {

      /// Default constructor
      AnalysisData()
      {
        #ifdef ANALYSISDATA_DEBUG
          std::cerr << "DEBUG: AnalysisData: " << this << " - Constructed (default ctor)" << std::endl;
        #endif
        clear();
      }

      /// Constructor with analysis name
      AnalysisData(const std::string& name) :
        analysis_name(name)
      {
        #ifdef ANALYSISDATA_DEBUG
          std::cerr << "DEBUG: AnalysisData: " << this << " - Constructed (ctor with analysis name)" << std::endl;
        #endif
        clear();
      }

      // A copy constructor only used for debugging
      #ifdef ANALYSISDATA_DEBUG
      AnalysisData(const AnalysisData& copy) :
        analysis_name(copy.analysis_name),
        srdata(copy.srdata),
        srdata_identifiers(copy.srdata_identifiers),
        srcov(copy.srcov)
      {
          std::cerr << "DEBUG: AnalysisData: " << this << " - Copy-constructed from " << &copy << std::endl;
      }
      #endif

      // A destructor only used for debugging
      #ifdef ANALYSISDATA_DEBUG
      ~AnalysisData()
      {
        std::cerr << "DEBUG: AnalysisData: " << this << " - Destructed" << std::endl;
      }
      #endif

      /// @brief Constructor from a list of SignalRegionData and an optional correlation (or covariance?) matrix
      ///
      /// If corrs is a null matrix (the default), this AnalysisData is to be interpreted as having no correlation
      /// information, and hence the likelihood calculation should use the single best-expected-limit SR.
      AnalysisData(const std::vector<SignalRegionData>& srds, const Eigen::MatrixXd& cov=Eigen::MatrixXd())
        : srdata(srds), srcov(cov)
      {
        #ifdef ANALYSISDATA_DEBUG
          std::cerr << "DEBUG: AnalysisData: " << this << " - Constructed (special ctor)" << std::endl;
        #endif
        check();
      }

      /// Clear the list of SignalRegionData, and nullify the covariance matrix
      /// @todo It'd be good to *not* have to re-enter most of the SRData and the covariance on every point: they don't change
      void clear()
      {
        for (auto& sr : srdata)
        {
          sr.n_sig_MC = 0;
          sr.n_sig_scaled = 0;
          sr.n_sig_MC_sys = 0;
        }
        srcov = Eigen::MatrixXd();
        #ifdef ANALYSISDATA_DEBUG
          std::cerr << "DEBUG: AnalysisData: " << this << " - Cleared" << std::endl;
        #endif
      }

      /// Number of analyses
      size_t size() const
      {
        // check();
        return srdata.size();
      }

      /// Is this container empty of signal regions?
      bool empty() const { return size() == 0; }

      /// Is there non-null correlation data?
      bool hasCorrs() const
      {
        // check(); // bjf> This was wrong! Needs to be !=, not ==
        return srcov.rows() != 0;
      }

      /// @brief Add a SignalRegionData
      /// @todo Allow naming the SRs?
      void add(const SignalRegionData& srd)
      {
        auto loc = srdata_identifiers.find(srd.sr_label);
        if (loc == srdata_identifiers.end())
        {
          // If the signal region doesn't exist in this object yet, add it
          srdata.push_back(srd);
          srdata_identifiers[srd.sr_label] = srdata.size() - 1;
        }
        else
        {
          // If it does, just update the signal count in the existing SignalRegionData object
          srdata[loc->second].n_sig_MC = srd.n_sig_MC;
        }
        check();
      }

      /// Check that the SRData list and the covariance matrix are consistent
      bool check() const
      {
        for (const SignalRegionData& srd : srdata) srd.check();
        assert(srcov.rows() == 0 || srcov.rows() == (int) srdata.size());
        // for (int isr = 0; isr < srcov.rows(); ++isr) {
        //   const double& srbg = srdata[isr].n_bkg_err;
        //   #ifdef ANALYSISDATA_DEBUG
        //     std::cerr << "DEBUG: AnalysisData: isr:" << isr << ", srbg:" << srbg << ", srcov(isr,isr):" << srcov(isr,isr) << std::endl;
        //   #endif
        //   assert(fabs(srcov(isr,isr) - srbg*srbg) < 1e-2);
        // }
        return true;
      }

      /// bjf> Experimental! But already useful for helping me convert the key
      /// numbers from these analyses to Python for the p-value calculuations.
      void pythonize_me() const;

      /// Analysis name
      std::string analysis_name;

      /// Access the i'th signal region's data
      SignalRegionData& operator[] (size_t i) { return srdata[i]; }
      /// Access the i'th signal region's data (const)
      const SignalRegionData& operator[] (size_t i) const { return srdata[i]; }

      /// Iterators (sugar for direct access to this->srdata)
      std::vector<SignalRegionData>::iterator begin() { return srdata.begin(); }
      std::vector<SignalRegionData>::const_iterator begin() const { return srdata.begin(); }
      std::vector<SignalRegionData>::iterator end() { return srdata.end(); }
      std::vector<SignalRegionData>::const_iterator end() const { return srdata.end(); }

      /// List of signal regions' data summaries
      std::vector<SignalRegionData> srdata;

      /// Map of names and indices of all entries in srdata, for easy lookup
      std::map<std::string, int> srdata_identifiers;

      /// Optional covariance matrix between SRs (0x0 null matrix = no correlation info)
      Eigen::MatrixXd srcov;

    };


  }
}
