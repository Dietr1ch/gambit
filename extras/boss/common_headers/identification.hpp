// Identify backend and set macro flags

#include "cats.hpp"

#ifdef BACKENDRENAME
  #define BACKENDNAME BACKENDRENAME
#else
  #define BACKENDNAME __BACKEND_NAME__
#endif

#define LIBPATH "Backends/lib/__SHARED_LIB_FILE_NAME__"

#define VERSION __BACKEND_VERSION__
#define SAFE_VERSION __BACKEND_SAFE_VERSION__

#undef DO_CLASSLOADING
#define DO_CLASSLOADING 1
