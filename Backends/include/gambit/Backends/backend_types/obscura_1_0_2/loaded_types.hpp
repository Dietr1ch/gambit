#ifndef __loaded_types_obscura_1_0_2_hpp__
#define __loaded_types_obscura_1_0_2_hpp__ 1

#include "identification.hpp"

// Indicate which types are provided by this backend, and what the symbols of their factories are.
#define obscura_1_0_2_all_data \


// If the default version has been loaded, set it as default.
#if ALREADY_LOADED(CAT_3(BACKENDNAME,_,CAT(Default_,BACKENDNAME)))
  SET_DEFAULT_VERSION_FOR_LOADING_TYPES(BACKENDNAME,SAFE_VERSION,CAT(Default_,BACKENDNAME))
#endif

// Undefine macros to avoid conflict with other backends.
#include "gambit/Backends/backend_undefs.hpp"

#endif /* __loaded_types_obscura_1_0_2_hpp__ */
