# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  Cmake configuration script to look for optional
#  things for GAMBIT.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Antje Putze
#          (antje.putze@lapth.cnrs.fr)
#  \date 2014 Sep, Oct, Nov
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2014 Nov, Dec
#
#  \author Ben Farmer
#          (benjamin.farmer@fysik.su.se)
#  \date 2015 May
#
#  \author Will Handley
#          (wh260@cam.ac.uk)
#  \date 2018 May, Dec
#
#************************************************

# Check for MPI libraries; enable manually with "cmake -DWITH_MPI=ON .."
option(WITH_MPI "Compile with MPI enabled" OFF)
if(WITH_MPI)
  find_package(MPI)
  # Do things for GAMBIT itself
  if(MPI_C_FOUND OR MPI_CXX_FOUND)
    message("${BoldYellow}-- MPI C/C++ libraries found. GAMBIT will be MPI-enabled.${ColourReset}")
    add_definitions(-DWITH_MPI)
    if(MPI_CXX_FOUND)
      include_directories(${MPI_CXX_INCLUDE_PATH})
      add_definitions(${MPI_CXX_COMPILE_FLAGS})
    endif()
    if(MPI_C_FOUND)
      include_directories(${MPI_C_INCLUDE_PATH})
      add_definitions(${MPI_C_COMPILE_FLAGS})
      if (NOT MPI_CXX_FOUND)
        message("${Red}-- Warning: C MPI libraries found, but not C++ MPI libraries.  Usually that's OK, but")
        message("   if you experience MPI linking errors, please install C++ MPI libraries as well.${CoulourReset}")
      endif()
    endif()
  else()
    message("${BoldRed}   Missing C MPI installation.  GAMBIT will not be MPI-enabled.${ColourReset}")
  endif()
  # Do things for Fortran backends and scanners
  if(MPI_Fortran_FOUND)
    if(MPI_C_FOUND)
      message("${BoldYellow}-- MPI Fortran libraries found. Fortran scanners will be MPI-enabled.${ColourReset}")
      # Includes
      foreach(dir ${MPI_Fortran_INCLUDE_PATH})
        set(GAMBIT_MPI_F_INC "${GAMBIT_MPI_F_INC} -I${dir}")
      endforeach()
      string(STRIP "${GAMBIT_MPI_F_INC}" GAMBIT_MPI_F_INC)
      set(BACKEND_Fortran_FLAGS_PLUS_MPI "${MPI_Fortran_COMPILE_FLAGS} ${BACKEND_Fortran_FLAGS} -DMPI ${GAMBIT_MPI_F_INC}")
      string(STRIP "${BACKEND_Fortran_FLAGS_PLUS_MPI}" BACKEND_Fortran_FLAGS_PLUS_MPI)
      # Libraries
      foreach(lib ${MPI_Fortran_LIBRARIES})
        set(GAMBIT_MPI_F_LIB "${GAMBIT_MPI_F_LIB} ${lib}")
      endforeach()
      if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(GAMBIT_MPI_F_LIB "-Wl,--no-as-needed ${GAMBIT_MPI_F_LIB}")
      endif()
      string(STRIP "${GAMBIT_MPI_F_LIB}" GAMBIT_MPI_F_LIB)
      set(CMAKE_Fortran_MPI_SO_LINK_FLAGS "${MPI_Fortran_LINK_FLAGS} ${GAMBIT_MPI_F_LIB}")
      string(STRIP "${CMAKE_Fortran_MPI_SO_LINK_FLAGS}" CMAKE_Fortran_MPI_SO_LINK_FLAGS)
    endif()
  else()
    message("${BoldRed}   Missing Fortran MPI installation.  Fortran scanners will not be MPI-enabled.${ColourReset}")
  endif()

  # Do things for C++ backends and scanners
  if(MPI_CXX_FOUND)
    if(MPI_C_FOUND)
      message("${BoldYellow}-- MPI C++ libraries found. C++ scanners will be MPI-enabled.${ColourReset}")
      # Includes
      foreach(dir ${MPI_CXX_INCLUDE_PATH})
        set(GAMBIT_MPI_CXX_INC "${GAMBIT_MPI_CXX_INC} -I${dir}")
      endforeach()
      string(STRIP "${GAMBIT_MPI_CXX_INC}" GAMBIT_MPI_CXX_INC)
      set(BACKEND_CXX_FLAGS_PLUS_MPI "${MPI_CXX_COMPILE_FLAGS} ${BACKEND_CXX_FLAGS} -DUSE_MPI ${GAMBIT_MPI_CXX_INC}")
      string(STRIP "${BACKEND_CXX_FLAGS_PLUS_MPI}" BACKEND_CXX_FLAGS_PLUS_MPI)
      # Libraries
      foreach(lib ${MPI_CXX_LIBRARIES})
        set(GAMBIT_MPI_CXX_LIB "${GAMBIT_MPI_CXX_LIB} ${lib}")
      endforeach()
      if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(GAMBIT_MPI_CXX_LIB "-Wl,--no-as-needed ${GAMBIT_MPI_CXX_LIB}")
      endif()
      string(STRIP "${GAMBIT_MPI_CXX_LIB}" GAMBIT_MPI_CXX_LIB)
      set(CMAKE_CXX_MPI_SO_LINK_FLAGS "${MPI_CXX_LINK_FLAGS} ${GAMBIT_MPI_CXX_LIB}")
      string(STRIP "${CMAKE_CXX_MPI_SO_LINK_FLAGS}" CMAKE_CXX_MPI_SO_LINK_FLAGS)
    endif()
  else()
    message("${BoldRed}   Missing C++ MPI installation.  C++ scanners will not be MPI-enabled.${ColourReset}")
  endif()
else()
  message("${BoldCyan} X MPI is disabled. Executables will not be parallelised with MPI. Please use -DWITH_MPI=ON to enable MPI.${ColourReset}")
endif()

# Check for LAPACK.  Cmake native findLAPACK isn't very thorough, so we need to do a bit more work here.
if(NOT LAPACK_LINKLIBS)
  find_package(LAPACK)
  if(LAPACK_FOUND)
    # Check the libs for MKL
    string(FIND "${LAPACK_LIBRARIES}" "libmkl_" FOUND_MKL)
    if(NOT ${FOUND_MKL} EQUAL -1)
      string(FIND "${LAPACK_LIBRARIES}" "libmkl_rt" FOUND_MKLRT)
      if(NOT ${FOUND_MKLRT} EQUAL -1)
        set(SDL_ADDED TRUE)
      else()
        set(SDL_ADDED FALSE)
      endif()
    endif()
    # Step through the libraries and fix their names up before adding them to the final list
    foreach(lib ${LAPACK_LIBRARIES})
      string(REGEX REPLACE "^(.*)/(.*)\\..*$" "\\1" BLAS_LAPACK_LOCATION ${lib})
      if(NOT ${FOUND_MKL} EQUAL -1)
        # Add the library location to the rpath, in case it wants to dynamically load other libs
        if(EXISTS BLAS_LAPACK_LOCATION)
          set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${BLAS_LAPACK_LOCATION}")
        endif()
        # Add the silver-bullet SDL mkl_rt.so if possible.
        set(SDL "${BLAS_LAPACK_LOCATION}/libmkl_rt.so")
        if(NOT SDL_ADDED AND EXISTS ${SDL})
          set(LAPACK_LINKLIBS "${LAPACK_LINKLIBS} ${SDL}")
          set(SDL_ADDED TRUE)
        endif()
        # Make sure FindLAPACK.cmake doesn't clobber gcc's openmp
        string(FIND "${lib}" "iomp5" IS_IOMP5)
        string(FIND "${lib}" "mkl_intel_thread" IS_MKLINTELTHREAD)
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
          if(NOT ${IS_IOMP5} EQUAL -1)
            set(lib "")
          endif()
          if(NOT ${IS_MKLINTELTHREAD} EQUAL -1)
            string(REGEX REPLACE "intel_thread" "def" DEF ${lib})
            string(REGEX REPLACE "intel_thread" "gnu_thread" lib ${lib})
            if(NOT EXISTS ${lib} OR NOT EXISTS ${DEF})
              message(FATAL_ERROR "${BoldRed}You are using the GNU or LLVM C++ compiler, but cmake's automatic FindLAPACK.cmake"
                                  "script is trying to link to the intel MKL library, using the intel OpenMP implementation."
                                  "I tried to force MKL to use the GNU OpenMP implementation, but I cannot find one or both of "
                                  "libmkl_def.so and libmkl_gnu_thread.so.  Please rerun cmake, manually specifying what LAPACK"
                                  "libraries to use, via e.g."
                                  "  cmake -DLAPACK_LINKLIBS=\"<your libs>\" ..${ColourReset}")
            endif()
            # Add the mkl_def.so library needed by mkl_gnu_thread.  Let mkl_gnu_thread get added below.
            set(LAPACK_LINKLIBS "${LAPACK_LINKLIBS} ${DEF}")
          endif()
        endif()
      endif()
      string(FIND "${lib}" ".framework" IS_FRAMEWORK)
      if(NOT ${IS_FRAMEWORK} EQUAL -1)
        string(REGEX REPLACE "^(.*)/(.*)\\.framework.*$" "-F\\1 -framework \\2" lib ${lib})
      endif()
      set(LAPACK_LINKLIBS "${LAPACK_LINKLIBS} ${lib}")
    endforeach()
    string(STRIP "${LAPACK_LINKLIBS}" LAPACK_LINKLIBS)
    message("   Using the following LAPACK libraries: ${LAPACK_LINKLIBS}")
  endif()
else()
  message("${BoldCyan}   LAPACK linking commands provided by hand; skipping cmake search and assuming no LAPACK-dependent components need to be ditched.${ColourReset}")
endif()
string( REGEX MATCH "\\.l*a[:space:]*$" LAPACK_STATIC "${LAPACK_LINKLIBS}" )
if(LAPACK_STATIC OR (NOT LAPACK_LINKLIBS AND NOT LAPACK_FOUND))
  message(FATAL_ERROR "${BoldRed}LAPACK shared libraries are currently required in order to build GAMBIT.${ColourReset}")
  # In future MN and FS need to be ditched if lapack cannot be found, and the build allowed to continue.
  message("${BoldRed}   LAPACK shared library not found. Excluding FlexibleSUSY and MultiNest from GAMBIT configuration. ${ColourReset}")
endif()

# Helper function to check if ROOT has been compiled with the same standard as we are using here.  If not, downgrade to the standard that ROOT was compiled with.
function(check_root_std_flag std_list prev)
  list(LENGTH std_list std_len)
  if (std_len LESS 1)
    message(FATAL_ERROR "check_root_std_flag called with no standards!  Please report this bug to the GAMBIT developers.")
  endif()
  list(GET std_list 0 std)
  set(CXX_FLAG "-std=c++${std}")
  set(CXX_FLAG_RE "-std=c\\+\\+${std}")
  set(PREV_CXX_FLAG_RE "-std=c\\+\\+${prev}")
  string(REGEX MATCH ${CXX_FLAG_RE} ROOT_USES_STD ${ROOT_CXX_FLAGS})
  if (ROOT_USES_STD)
    message("${BoldYellow}   This ROOT was compiled with ${CXX_FLAG}.${ColourReset}")
    if ((NOT ${prev} STREQUAL "") AND ${COMPILER_SUPPORTS_CXX${prev}})
      unset(${COMPILER_SUPPORTS_CXX${prev}})
      CHECK_CXX_COMPILER_FLAG(${CXX_FLAG} COMPILER_SUPPORTS_CXX${std})
      if(${COMPILER_SUPPORTS_CXX${std}})
        string(REGEX REPLACE ${PREV_CXX_FLAG_RE} ${CXX_FLAG} CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE ${PREV_CXX_FLAG_RE} ${CXX_FLAG} BACKEND_CXX_FLAGS "${BACKEND_CXX_FLAGS}")
      endif()
    endif()
    if (NOT ${COMPILER_SUPPORTS_CXX${std}})
      message(FATAL_ERROR "${BoldRed}This installation of ROOT has been compiled with C++${std} support, "
                          "but your chosen compiler does not support C++${std}.  Please change compiler "
                          "or set -DWITH_ROOT=OFF.${ColourReset}")
    endif()
  elseif (std_len GREATER 1)
    list(REMOVE_AT std_list 0)
    check_root_std_flag("${std_list}" ${std})
  else()
    message(FATAL_ERROR "${BoldRed}Unable to detect what flavour of C++ your installation of ROOT has "
                        "been compiled with; please set -DWITH_ROOT=OFF.${ColourReset}")
  endif()
endfunction()

# Check for ROOT.
option(WITH_ROOT "Compile with ROOT enabled" OFF)
if(WITH_ROOT)
  if (DEFINED ENV{ROOTSYS})
    list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake/)
    find_package(ROOT 6)
    if (ROOT_VERSION VERSION_LESS 6)
      set (ROOT_FOUND FALSE)
    endif()
  else()
    set (ROOT_FOUND FALSE)
  endif()
  if(NOT ROOT_FOUND)
    message("${BoldRed}   ROOT 6 not found.  ROOT support will be disabled.${ColourReset}")
  endif()
else()
  message("${BoldCyan} X ROOT support is deactivated. Set -DWITH_ROOT=ON to activate ROOT support in GAMBIT.${ColourReset}")
endif()
if (WITH_ROOT AND ROOT_FOUND)
  message("${BoldYellow}   Found ROOT version ${ROOT_VERSION}.${ColourReset}")
  if ("${ROOT_INCLUDE_DIRS}" STREQUAL "")
    if ("${ROOT_INCLUDE_DIR}" STREQUAL "")
      message(FATAL_ERROR "${BoldRed}FindROOT.cmake has not provided any include dir."
                          "This is a ROOT bug; please report it to the ROOT developers."
                          "You can set -DWITH_ROOT=OFF to compile GAMBIT without ROOT.${ColourReset}")
    endif()
    set(ROOT_INCLUDE_DIRS "${ROOT_INCLUDE_DIR}")
  endif()
  include_directories(${ROOT_INCLUDE_DIRS})
  add_definitions(${ROOT_DEFINITIONS})
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};$ENV{ROOTSYS}/lib")
  check_root_std_flag("17;1z;14;1y;11;0x" "")
  set (EXCLUDE_ROOT FALSE)
else()
  message("   Disabling GreAT and RestFrames support in GAMBIT configuration.")
  option (WITH_RESTFRAMES OFF)
  set (itch "${itch}" "great")
  set (EXCLUDE_ROOT TRUE)
endif()

# Check for HDF5 libraries
find_package(HDF5 QUIET COMPONENTS C)
if(HDF5_FOUND)
  include_directories(${HDF5_INCLUDE_DIR})  # for older versions of cmake
  include_directories(${HDF5_INCLUDE_DIRS}) # for newer cmake
  message("-- Found HDF5 libraries: ${HDF5_LIBRARIES}")
  if(VERBOSE)
    message(STATUS ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR})
  endif()
else()
  message("${BoldRed}   No HDF5 C libraries found. Excluding hdf5printer and hdf5reader from GAMBIT configuration.${ColourReset}")
  set(itch "${itch}" "hdf5printer" "hdf5reader")
endif()

# Check for SQLite libraries
find_package(SQLite3 QUIET COMPONENTS C)
if(SQLITE3_FOUND)
  include_directories(${SQLITE3_INCLUDE_DIRS})
  message("-- Found SQLite3 libraries: ${SQLITE3_LIBRARIES}")
  if(VERBOSE)
      message(STATUS ${SQLITE3_INCLUDE_DIRS})
  endif()
else()
  message("${BoldRed}   No SQLite C libraries found. Excluding sqliteprinter and sqlitereader from GAMBIT configuration.${ColourReset}")
  set(itch "${itch}" "sqliteprinter" "sqlitereader")
endif()

