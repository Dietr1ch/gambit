#define BACKENDNAME pbarlike
#define BACKENDLANG Python
#define VERSION 1.0
#define SAFE_VERSION 1_0

LOAD_LIBRARY

#ifdef HAVE_PYBIND11
    BE_CONV_FUNCTION(c_del_chi2,double,(double&,  map_str_dbl&, double&, std::string& ), "pbar_del_chi2")
#endif
#include "gambit/Backends/backend_undefs.hpp"
