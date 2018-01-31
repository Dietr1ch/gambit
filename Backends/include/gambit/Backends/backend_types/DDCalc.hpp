//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Helper types for DDCalc backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          p.scott@imperial.ac.uk
///  \date 2016 May
///
///  *************************

#ifndef __DDCalc_types_hpp__
#define __DDCalc_types_hpp__

namespace Gambit
{

  // Container for dark matter - nucleon couplings
  struct DM_nucleon_couplings
  {
    double gps;
    double gns;
    double gpa;
    double gna;
  };

  struct DD_bin_counts
  {
    DD_bin_counts() {}
    DD_bin_counts(const std::vector<double> & counts, const unsigned int & n) : bincounts(counts), nbins(n) {}

    std::vector<double> bincounts;
    int nbins;
  };

}

#endif /* defined __DDCalc_types_hpp__ */
