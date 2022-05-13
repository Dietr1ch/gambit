#define BACKENDNAME pbarlike
#define BACKENDLANG Python
#define VERSION 1.0
#define SAFE_VERSION 1_0

LOAD_LIBRARY

#ifdef HAVE_PYBIND11
    // BE_CONV_FUNCTION(c_chi2,double,(std::vector<double>&, double&),"pbar_log_likelihood")
    // BE_CONV_FUNCTION(c_pbar_pred,std::vector<std::vector<double>>,(double&, double&, map_str_dbl&),"antiproton_flux")
    BE_CONV_FUNCTION(c_del_chi2,std::vector<double>,(double&, double&, map_str_dbl&), "pbar_del_chi2")
#endif
#include "gambit/Backends/backend_undefs.hpp"
