# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  CMake configuration scripts for obtaining,
#  configuring, compiling and installing
#  backends.
#
#  To add an entry for a new backend, copy
#  and modify an existing one.  Don't use
#  CMAKE_C_FLAGS, CMAKE_CXX_FLAGS, etc here,
#  as these contain extra flags for building
#  GAMBIT itself that will break backends. Use
#  BACKEND_C_FLAGS
#  BACKEND_CXX_FLAGS
#  BACKEND_Fortran_FLAGS
#  If you need to avoid the optimisation
#  settings passed in those, instead use
#  BACKEND_C_FLAGS_NO_BUILD_OPTIMISATIONS
#  BACKEND_CXX_FLAGS_NO_BUILD_OPTIMISATIONS
#  BACKEND_Fortran_FLAGS_NO_BUILD_OPTIMISATIONS.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Antje Putze
#          (antje.putze@lapth.cnrs.fr)
#  \date 2014 Sep, Oct, Nov
#  \date 2015 Sep
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2014 Nov, Dec
#  \date 2015 May, Dec
#  \date For the term of my natural life
#
#  \author Chris Rogan
#          (crogan@cern.ch)
#  \date 2015 May
#
#  \author Anders Kvellestad
#          (anderkve@fys.uio.no)
#  \date 2015 May
#
#  \author Christoph Weniger
#          (c.weniger@uva.nl)
#  \date 2015 Sep
#
#  \author Tomas Gonzalo
#          (t.e.gonzalo@fys.uio.no)
#  \date 2016 Apr, Dec
#
#  \author James McKay
#          (j.mckay14@imperial.ac.uk)
#  \date 2016 Aug
#
#  \author Ankit Beniwal
#  	   (ankit.beniwal@adelaide.edu.au)
#  \date 2016 Aug
#  \date 2017 Jun
#  \date 2018 Aug
#
#  \author Aaron Vincent
#          (aaron.vincent@cparc.ca)
#  \date 2017 Sep, Nov
#
#  \author Janina Renk
#          (janina.renk@fysik.su.se)
#  \data 2018 Jun
#
#************************************************


# AlterBBN
set(name "alterbbn")
set(ver "2.1")
set(lib "libbbn")
set(dl "https://alterbbn.hepforge.org/downloads?f=alterbbn_v2.1.tgz")
set(md5 "c8e024ddcb1011281cd18476d49918c7")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    #BUILD_COMMAND sed ${dashi} -e "s#CC = gcc#CC = ${CMAKE_C_COMPILER}#g" Makefile
    BUILD_COMMAND sed ${dashi} -e "s#CC = gcc#CC = ${CMAKE_C_COMPILER}#g" Makefile
          COMMAND sed ${dashi} -e "s#rcsU#rcs#g" Makefile
          COMMAND sed ${dashi} -e "s/CFLAGS= -O3 -pipe -fomit-frame-pointer/CFLAGS= ${BACKEND_C_FLAGS}/g" Makefile
          COMMAND sed ${dashi} -e "s/CFLAGS_MP= -fopenmp/CFLAGS_MP= ${OpenMP_C_FLAGS}/g" Makefile
          COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ar x src/libbbn.a
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_C_COMPILER} ${OpenMP_C_FLAGS} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o ${lib}.so *.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# AlterBBN
set(name "alterbbn")
set(ver "2.0")
set(lib "libbbn")
set(dl "https://alterbbn.hepforge.org/downloads/alterbbn_v2.0.tgz")
set(md5 "cca5fb50440f25dc61fbfb6dbf61b32b")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    #BUILD_COMMAND sed ${dashi} -e "s#CC = gcc#CC = ${CMAKE_C_COMPILER}#g" Makefile
    BUILD_COMMAND sed ${dashi} -e "s#CC = gcc#CC = ${CMAKE_C_COMPILER}#g" Makefile
          COMMAND sed ${dashi} -e "s#rcsU#rcs#g" Makefile
          COMMAND sed ${dashi} -e "s/CFLAGS= -O3 -pipe -fomit-frame-pointer/CFLAGS= ${BACKEND_C_FLAGS}/g" Makefile
          COMMAND sed ${dashi} -e "s/CFLAGS_MP= -fopenmp/CFLAGS_MP= ${OpenMP_C_FLAGS}/g" Makefile
          COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ar x src/libbbn.a
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_C_COMPILER} ${OpenMP_C_FLAGS} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o ${lib}.so *.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()

# CaptnGeneral
set(name "capgen")
set(ver "1.0")
set(lib "gencaplib")
set(dl "null")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    GIT_REPOSITORY https://github.com/aaronvincent/captngen.git
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FOPT=${BACKEND_Fortran_FLAGS} MODULE=${FMODULE}
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# DarkSUSY
set(name "darksusy")
set(ver "5.1.3")
set(dl "staff.fysik.su.se/~edsjo/darksusy/tars/${name}-${ver}.tar.gz")
set(md5 "ca95ffa083941a469469710fab2f3c97")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}
    # FIXME parallel relic density routines don't work yet.
    #COMMAND patch -b -p2 -d src < ${patch}/patchDS_OMP_src.dif
    #COMMAND patch -b -p2 -d include < ${patch}/patchDS_OMP_include.dif
    CONFIGURE_COMMAND ./configure FC=${CMAKE_Fortran_COMPILER} FCFLAGS=${BACKEND_Fortran_FLAGS} FFLAGS=${BACKEND_Fortran_FLAGS} CC=${CMAKE_C_COMPILER} CFLAGS=${BACKEND_C_FLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${BACKEND_CXX_FLAGS}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} dslib_shared
          COMMAND ${CMAKE_MAKE_PROGRAM} install_tables
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()


# SuperIso
set(name "superiso")
set(ver "3.6")
set(lib "libsuperiso")
set(dl "http://superiso.in2p3.fr/download/${name}_v${ver}.tgz")
set(md5 "df864ceeccb72467bfbe572a8da9711d")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND sed ${dashi} -e "s#CC = gcc#CC = ${CMAKE_C_COMPILER}#g" Makefile
          COMMAND sed ${dashi} -e "s#rcsU#rcs#g" src/Makefile
          COMMAND sed ${dashi} -e "s/CFLAGS= -O3 -pipe -fomit-frame-pointer/CFLAGS= -fPIC ${BACKEND_C_FLAGS}/g" Makefile
          COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ar x src/libisospin.a
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_C_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o ${lib}.so *.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# DDCalc
set(name "ddcalc")
set(ver "1.0.0")
set(lib "libDDCalc")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "0c0da22b84721fc1d945f8039a4686c9")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(ddcalc_flags "${BACKEND_Fortran_FLAGS} -${FMODULE} ${dir}/build")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FOPT=${ddcalc_flags} DDCALC_DIR=${dir} OUTPUT_PIPE=>/dev/null
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()

set(name "ddcalc")
set(ver "1.1.0")
set(lib "libDDCalc")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "47191564385379dd70aeba4811cd7c3b")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(ddcalc_flags "${BACKEND_Fortran_FLAGS} -${FMODULE} ${dir}/build")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FOPT=${ddcalc_flags} DDCALC_DIR=${dir} OUTPUT_PIPE=>/dev/null
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()

set(name "ddcalc")
set(ver "1.2.0")
set(lib "libDDCalc")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "93b894b80b360159264f0d634cd7387e")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(ddcalc_flags "${BACKEND_Fortran_FLAGS} -${FMODULE} ${dir}/build")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FOPT=${ddcalc_flags} DDCALC_DIR=${dir} OUTPUT_PIPE=>/dev/null
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()

set(name "ddcalc")
set(ver "2.0.0")
set(lib "libDDCalc")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "504cb95a298fa62d11097793dc318549")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}/")
set(ddcalc_flags "${BACKEND_Fortran_FLAGS} -${FMODULE} ${dir}/build")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FOPT=${ddcalc_flags} DDCALC_DIR=${dir} OUTPUT_PIPE=>/dev/null
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()


# Gamlike
set(name "gamlike")
set(ver "1.0.0")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "16b763a2e8b9d6c174d8b7ca2f4cb575")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
if(GSL_FOUND)
  execute_process(
    COMMAND gsl-config --libs
    OUTPUT_VARIABLE GAMLIKE_GSL_LIBS
    RESULT_VARIABLE RET
  )
  if( RET EQUAL 0 )
    string( STRIP "${GAMLIKE_GSL_LIBS}" GAMLIKE_GSL_LIBS )
  endif()
endif()
set(gamlike_CXXFLAGS "${BACKEND_CXX_FLAGS}")
if (NOT GSL_INCLUDE_DIRS STREQUAL "")
  set(gamlike_CXXFLAGS "${gamlike_CXXFLAGS} -I${GSL_INCLUDE_DIRS}")
endif()
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${gamlike_CXXFLAGS} LDFLAGS=${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} LDLIBS=${GAMLIKE_GSL_LIBS} GAMLIKE_DATA_PATH=${dir}/data
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()


# MicrOmegas base (for all models)
set(name "micromegas")
set(ver "3.6.9.2")
set(dl "http://lapth.cnrs.fr/micromegas/downloadarea/code/${name}_${ver}.tgz")
set(md5 "72807f6d0ef80737554d8702b6b212c1")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    PATCH_COMMAND patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make flags
          COMMAND sed ${dashi} -e "s|FC =.*|FC = ${CMAKE_Fortran_COMPILER}|" CalcHEP_src/FlagsForMake
          COMMAND sed ${dashi} -e "s|CC =.*|CC = ${CMAKE_C_COMPILER}|" CalcHEP_src/FlagsForMake
          COMMAND sed ${dashi} -e "s|CXX =.*|CXX = ${CMAKE_CXX_COMPILER}|" CalcHEP_src/FlagsForMake
          COMMAND sed ${dashi} -e "s|FFLAGS =.*|FFLAGS = ${BACKEND_Fortran_FLAGS}|" CalcHEP_src/FlagsForMake
          COMMAND sed ${dashi} -e "s|CFLAGS =.*|CFLAGS = ${BACKEND_C_FLAGS}|" CalcHEP_src/FlagsForMake
          COMMAND sed ${dashi} -e "s|CXXFLAGS =.*|CXXFLAGS = ${BACKEND_CXX_FLAGS}|" CalcHEP_src/FlagsForMake
          COMMAND sed ${dashi} -e "s|FC=.*|FC=\"${CMAKE_Fortran_COMPILER}\"|" CalcHEP_src/FlagsForSh
          COMMAND sed ${dashi} -e "s|CC=.*|CC=\"${CMAKE_C_COMPILER}\"|" CalcHEP_src/FlagsForSh
          COMMAND sed ${dashi} -e "s|CXX=.*|CXX=\"${CMAKE_CXX_COMPILER}\"|" CalcHEP_src/FlagsForSh
          COMMAND sed ${dashi} -e "s|FFLAGS=.*|FFLAGS=\"${CMAKE_Fortran_FLAGS}\"|" CalcHEP_src/FlagsForSh
          COMMAND sed ${dashi} -e "s|CFLAGS=.*|CFLAGS=\"${CMAKE_C_FLAGS}\"|" CalcHEP_src/FlagsForSh
          COMMAND sed ${dashi} -e "s|CXXFLAGS=.*|CXXFLAGS=\"${BACKEND_CXX_FLAGS}\"|" CalcHEP_src/FlagsForSh
          COMMAND sed ${dashi} -e "s|lFort=.*|lFort=|" CalcHEP_src/FlagsForSh
          COMMAND make
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} "yes | clean")
  set_as_default_version("backend" ${name} ${ver})
endif()

# MicrOmegas MSSM model
set(model "MSSM")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}_${model}")
check_ditch_status(${name}_${model} ${ver})
if(NOT ditched_${name}_${model}_${ver})
  ExternalProject_Add(${name}_${model}_${ver}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${dir}
    PATCH_COMMAND patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${model} ${CMAKE_MAKE_PROGRAM} sharedlib main=main.c
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend model" ${name} ${ver} ${dir}/${model} ${model} "yes | clean")
  set_as_default_version("backend model" ${name}_${model} ${ver})
endif()

# MicrOmegas ScalarSingletDM_Z2 model
set(model "ScalarSingletDM_Z2")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}_${model}")
check_ditch_status(${name}_${model} ${ver})
if(NOT ditched_${name}_${model}_${ver})
  ExternalProject_Add(${name}_${model}_${ver}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${dir}
    PATCH_COMMAND ./newProject ${model} && patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${model} ${CMAKE_MAKE_PROGRAM} sharedlib main=main.c
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend model" ${name} ${ver} ${dir}/${model} ${model} "yes | clean")
  set_as_default_version("backend model" ${name}_${model} ${ver})
endif()

# MicrOmegas ScalarSingletDM_Z3 model
set(model "ScalarSingletDM_Z3")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}_${model}")
check_ditch_status(${name}_${model} ${ver})
if(NOT ditched_${name}_${model}_${ver})
  ExternalProject_Add(${name}_${model}_${ver}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${dir}
    PATCH_COMMAND ./newProject ${model} && patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${model} ${CMAKE_MAKE_PROGRAM} sharedlib main=main.c
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend model" ${name} ${ver} ${dir}/${model} ${model} "yes | clean")
  set_as_default_version("backend model" ${name}_${model} ${ver})
endif()

# MicrOmegas VectorSingletDM_Z2 model
set(model "VectorSingletDM_Z2")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}_${model}")
check_ditch_status(${name}_${model} ${ver})
if(NOT ditched_${name}_${model}_${ver})
  ExternalProject_Add(${name}_${model}_${ver}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${dir}
    PATCH_COMMAND ./newProject ${model} && patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${model} ${CMAKE_MAKE_PROGRAM} sharedlib main=main.c
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend model" ${name} ${ver} ${dir}/${model} ${model} "yes | clean")
  set_as_default_version("backend model" ${name}_${model} ${ver})
endif()

# MicrOmegas MajoranaSingletDM_Z2 model
set(model "MajoranaSingletDM_Z2")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}_${model}")
check_ditch_status(${name}_${model} ${ver})
if(NOT ditched_${name}_${model}_${ver})
  ExternalProject_Add(${name}_${model}_${ver}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${dir}
    PATCH_COMMAND ./newProject ${model} && patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${model} ${CMAKE_MAKE_PROGRAM} sharedlib main=main.c
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend model" ${name} ${ver} ${dir}/${model} ${model} "yes | clean")
  set_as_default_version("backend model" ${name}_${model} ${ver})
endif()

# MicrOmegas DiracSingletDM_Z2 model
set(model "DiracSingletDM_Z2")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}_${model}")
check_ditch_status(${name}_${model} ${ver})
if(NOT ditched_${name}_${model}_${ver})
  ExternalProject_Add(${name}_${model}_${ver}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${dir}
    PATCH_COMMAND ./newProject ${model} && patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${model} ${CMAKE_MAKE_PROGRAM} sharedlib main=main.c
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend model" ${name} ${ver} ${dir}/${model} ${model} "yes | clean")
  set_as_default_version("backend model" ${name}_${model} ${ver})
endif()

# Pythia
set(name "pythia")
set(ver "8.212")
set(lib "libpythia8")
set(dl "http://home.thep.lu.se/~torbjorn/pythia8/pythia8212.tgz")
set(md5 "0886d1b2827d8f0cd2ae69b925045f40")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")

# - Add additional compiler-specific optimisation flags and suppress some warnings from -Wextra.
set(pythia_CXXFLAGS "${BACKEND_CXX_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  set(pythia_CXXFLAGS "${pythia_CXXFLAGS} -fast") # -fast sometimes makes xsecs come out as NaN, but we catch that and invalidate those points.
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(pythia_CXXFLAGS "${pythia_CXXFLAGS} -fno-math-errno -funsafe-math-optimizations -fno-rounding-math -fno-signaling-nans -fcx-limited-range") # Including all flags from -ffast-math except -ffinite-math-only which has proved to cause incorrect results.
  set_compiler_warning("no-extra" pythia_CXXFLAGS)
  set_compiler_warning("no-deprecated-declarations" pythia_CXXFLAGS)
endif()

# - Add "-undefined dynamic_lookup flat_namespace" to linker flags when OSX linker is used
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(pythia_CXX_SHARED_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup -flat_namespace")
else()
  set(pythia_CXX_SHARED_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS}")
endif()

# - Add option to turn off intel IPO if insufficient memory exists to use it.
option(PYTHIA_OPT "For Pythia: Switch Intel's multi-file interprocedural optimization on/off" ON)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND NOT "${PYTHIA_OPT}")
  set(pythia_CXXFLAGS "${pythia_CXXFLAGS} -no-ipo -ip")
endif()

# - Set include directories
set(pythia_CXXFLAGS "${pythia_CXXFLAGS} -I${Boost_INCLUDE_DIR} -I${PROJECT_SOURCE_DIR}/contrib/slhaea/include")

# - Actual configure and compile commands
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}
    CONFIGURE_COMMAND ./configure --enable-shared --cxx="${CMAKE_CXX_COMPILER}" --cxx-common="${pythia_CXXFLAGS}" --cxx-shared="${pythia_CXX_SHARED_FLAGS}" --lib-suffix=".so"
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CXX="${CMAKE_CXX_COMPILER}" lib/${lib}.so
    INSTALL_COMMAND ""
  )
  BOSS_backend(${name} ${ver})
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} distclean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# Pythia external model (EM)
set(name "pythia")
set(model "EM")
set(ver "8.212.${model}")
set(lib "libpythia8")
set(dl "http://home.thep.lu.se/~torbjorn/pythia8/pythia8212.tgz")
set(md5 "0886d1b2827d8f0cd2ae69b925045f40")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")
set(ext_model_dir "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/ExternalModel")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}
    CONFIGURE_COMMAND ./configure --enable-shared --cxx="${CMAKE_CXX_COMPILER}" --cxx-common="${pythia_CXXFLAGS}" --cxx-shared="${pythia_CXX_SHARED_FLAGS}" --lib-suffix=".so"
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CXX="${CMAKE_CXX_COMPILER}" lib/${lib}.so
    INSTALL_COMMAND ""
  )
  ExternalProject_Add_Step(${name}_${ver} add_external_Pythia_model
    COMMAND ${CMAKE_COMMAND} -E copy ${ext_model_dir}/ProcessContainer.cc ${dir}/src/
    COMMAND ${CMAKE_COMMAND} -E copy ${ext_model_dir}/Index.xml  ${dir}/share/Pythia8/xmldoc/
    COMMAND ${CMAKE_COMMAND} -E copy ${ext_model_dir}/UserModel.xml ${dir}/share/Pythia8/xmldoc/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ext_model_dir}/src ${dir}/src/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ext_model_dir}/include ${dir}/include/
    DEPENDEES download
    DEPENDERS patch
  )
  BOSS_backend(${name} ${ver})
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} distclean)
endif()


# Nulike
set(name "nulike")
set(ver "1.0.4")
set(lib "libnulike")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "47649992d19984ee53df6a1655c48227")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FF=${CMAKE_Fortran_COMPILER} FOPT=${BACKEND_Fortran_FLAGS} MODULE=${FMODULE}
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} distclean)
endif()

# Nulike
set(name "nulike")
set(ver "1.0.5")
set(lib "libnulike")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "20cee73a38fb3560298b6a3acdd4d83a")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FF=${CMAKE_Fortran_COMPILER} FOPT=${BACKEND_Fortran_FLAGS} MODULE=${FMODULE}
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} distclean)
endif()

# Nulike
set(name "nulike")
set(ver "1.0.6")
set(lib "libnulike")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "fc4c35dc867bb1213d80acd12e5c1169")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FF=${CMAKE_Fortran_COMPILER} FOPT=${BACKEND_Fortran_FLAGS} MODULE=${FMODULE}
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} distclean)
endif()

# Nulike
set(name "nulike")
set(ver "1.0.7")
set(lib "libnulike")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "5c8e74d125b619abe01e196af7baf790")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FF=${CMAKE_Fortran_COMPILER} FOPT=${BACKEND_Fortran_FLAGS} MODULE=${FMODULE}
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} distclean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# SUSY-HIT
set(name "susyhit")
set(ver "1.5")
set(lib "libsusyhit")
set(dl "https://www.itp.kit.edu/~maggie/SUSY-HIT/version${ver}_${name}.tar.gz")
set(md5 "493c7ba3a07e192918d3412875fb386a")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")

# - Due to a bug/instability in SUSYHIT, switch off optimization for Intel compilers
set(susyhit_Fortran_FLAGS "${BACKEND_Fortran_FLAGS}")
if("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel")
  set(susyhit_Fortran_FLAGS "${susyhit_Fortran_FLAGS} -O0")
endif()

check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${susyhit_Fortran_FLAGS}
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()


# FeynHiggs
set(name "feynhiggs")
set(ver "2.12.0")
set(lib "libFH")
set(dl "http://wwwth.mpp.mpg.de/members/heinemey/feynhiggs/newversion/FeynHiggs-${ver}.tar.gz")
set(md5 "da2d0787311525213cd4721da9946b86")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(FH_Fortran_FLAGS "${BACKEND_Fortran_FLAGS_NO_BUILD_OPTIMISATIONS}") #For skipping -O2, which seems to cause issues
set(FH_C_FLAGS "${BACKEND_C_FLAGS_NO_BUILD_OPTIMISATIONS}")             #For skipping -O2, which seems to cause issues
set(FH_CXX_FLAGS "${BACKEND_CXX_FLAGS_NO_BUILD_OPTIMISATIONS}")         #For skipping -O2, which seems to cause issues
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    # Fix bug preventing the use of array bounds checking.
    CONFIGURE_COMMAND sed ${dashi} -e "s#ComplexType spi_(2, 6:7, nvec, 1)#ComplexType spi_(2, 6:7, nvec, LEGS)#g" src/Decays/VecSet.F
              COMMAND ./configure FC=${CMAKE_Fortran_COMPILER} FFLAGS=${FH_Fortran_FLAGS} CC=${CMAKE_C_COMPILER} CFLAGS=${FH_C_FLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${FH_CXX_FLAGS}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ${CMAKE_COMMAND} -E make_directory lib
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o lib/${lib}.so build/*.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()

# FeynHiggs
set(name "feynhiggs")
set(ver "2.11.3")
set(lib "libFH")
set(dl "http://wwwth.mpp.mpg.de/members/heinemey/feynhiggs/newversion/FeynHiggs-${ver}.tar.gz")
set(md5 "49f5ea1838cb233baffd85bbc1b0d87d")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(FH_Fortran_FLAGS "${BACKEND_Fortran_FLAGS_NO_BUILD_OPTIMISATIONS}") #For skipping -O2, which seems to cause issues
set(FH_C_FLAGS "${BACKEND_C_FLAGS_NO_BUILD_OPTIMISATIONS}")             #For skipping -O2, which seems to cause issues
set(FH_CXX_FLAGS "${BACKEND_CXX_FLAGS_NO_BUILD_OPTIMISATIONS}")         #For skipping -O2, which seems to cause issues
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    # Fix bug preventing the use of array bounds checking.
    CONFIGURE_COMMAND sed ${dashi} -e "s#ComplexType spi_(2, 6:7, nvec, 1)#ComplexType spi_(2, 6:7, nvec, LEGS)#g" src/Decays/VecSet.F
              COMMAND ./configure FC=${CMAKE_Fortran_COMPILER} FFLAGS=${FH_Fortran_FLAGS} CC=${CMAKE_C_COMPILER} CFLAGS=${FH_C_FLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${FH_CXX_FLAGS}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ${CMAKE_COMMAND} -E make_directory lib
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o lib/${lib}.so build/*.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# FeynHiggs
set(name "feynhiggs")
set(ver "2.11.2")
set(lib "libFH")
set(dl "http://wwwth.mpp.mpg.de/members/heinemey/feynhiggs/newversion/FeynHiggs-${ver}.tar.gz")
set(md5 "edb73eafa6dab291bd8827242c16ac0a")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(FH_Fortran_FLAGS "${BACKEND_Fortran_FLAGS_NO_BUILD_OPTIMISATIONS}") #For skipping -O2, which seems to cause issues
set(FH_C_FLAGS "${BACKEND_C_FLAGS_NO_BUILD_OPTIMISATIONS}")             #For skipping -O2, which seems to cause issues
set(FH_CXX_FLAGS "${BACKEND_CXX_FLAGS_NO_BUILD_OPTIMISATIONS}")         #For skipping -O2, which seems to cause issues
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    # Fix bug preventing the use of array bounds checking.
    CONFIGURE_COMMAND sed ${dashi} -e "s#ComplexType spi_(2, 6:7, nvec, 1)#ComplexType spi_(2, 6:7, nvec, LEGS)#g" src/Decays/VecSet.F
              COMMAND ./configure FC=${CMAKE_Fortran_COMPILER} FFLAGS=${FH_Fortran_FLAGS} CC=${CMAKE_C_COMPILER} CFLAGS=${FH_C_FLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${FH_CXX_FLAGS}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ${CMAKE_COMMAND} -E make_directory lib
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o lib/${lib}.so build/*.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()


# HiggsBounds tables
set(name "higgsbounds_tables")
set(ver "0.0")
set(dl "https://higgsbounds.hepforge.org/downloads/csboutput_trans_binary.tar.gz")
set(md5 "004decca30335ddad95654a04dd034a6")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver} "retain container folder"
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()


# HiggsBounds
set(name "higgsbounds")
set(ver "4.3.1")
set(lib "libhiggsbounds")
set(dl "https://${name}.hepforge.org/downloads/HiggsBounds-${ver}.tar.gz")
set(md5 "c1667613f814a9f0297d1f11a8b3ef34")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")
set(hb_tab_name "higgsbounds_tables")
set(hb_tab_ver "0.0")
set(hb_tab_dir "${PROJECT_SOURCE_DIR}/Backends/installed/${hb_tab_name}/${hb_tab_ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DEPENDS ${hb_tab_name}_${hb_tab_ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    PATCH_COMMAND patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy configure-with-chisq my_configure
              COMMAND sed ${dashi} -e "s|clsbtablesdir=.*|clsbtablesdir=\"${hb_tab_dir}/\"|" my_configure
              COMMAND sed ${dashi} -e "s|F90C =.*|F90C = ${CMAKE_Fortran_COMPILER}|" my_configure
              COMMAND sed ${dashi} -e "s|F77C =.*|F77C = ${CMAKE_Fortran_COMPILER}|" my_configure
              COMMAND sed ${dashi} -e "s|F90FLAGS =.*|F90FLAGS = ${BACKEND_Fortran_FLAGS}|" my_configure
              COMMAND sed ${dashi} -e "s|\\.SUFFIXES|.NOTPARALLEL:${nl}${nl}.SUFFIXES|" makefile.in
              COMMAND ${CMAKE_COMMAND} -E copy makefile.in makefile.in.tmp
              COMMAND awk "{gsub(/${nl}/,${true_nl})}{print}" makefile.in.tmp > makefile.in
              COMMAND ${CMAKE_COMMAND} -E remove makefile.in.tmp
              COMMAND ./my_configure
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ${CMAKE_COMMAND} -E make_directory lib
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o lib/${lib}.so *.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# HiggsBounds
set(name "higgsbounds")
set(ver "4.2.1")
set(lib "libhiggsbounds")
set(dl "https://${name}.hepforge.org/downloads/HiggsBounds-${ver}.tar.gz")
set(md5 "47b93330d4e0fddcc23b381548db355b")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(hb_tab_name "higgsbounds_tables")
set(hb_tab_ver "0.0")
set(hb_tab_dir "${PROJECT_SOURCE_DIR}/Backends/installed/${hb_tab_name}/${hb_tab_ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DEPENDS ${hb_tab_name}_${hb_tab_ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy configure-with-chisq my_configure
              COMMAND sed ${dashi} -e "s|clsbtablesdir=.*|clsbtablesdir=\"${hb_tab_dir}/\"|" my_configure
              COMMAND sed ${dashi} -e "s|F90C =.*|F90C = ${CMAKE_Fortran_COMPILER}|" my_configure
              COMMAND sed ${dashi} -e "s|F77C =.*|F77C = ${CMAKE_Fortran_COMPILER}|" my_configure
              COMMAND sed ${dashi} -e "s|F90FLAGS =.*|F90FLAGS = ${BACKEND_Fortran_FLAGS}|" my_configure
              COMMAND sed ${dashi} -e "s|\\.SUFFIXES|.NOTPARALLEL:${nl}${nl}.SUFFIXES|" makefile.in
              COMMAND ${CMAKE_COMMAND} -E copy makefile.in makefile.in.tmp
              COMMAND awk "{gsub(/${nl}/,${true_nl})}{print}" makefile.in.tmp > makefile.in
              COMMAND ${CMAKE_COMMAND} -E remove makefile.in.tmp
              COMMAND ./my_configure
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ${CMAKE_COMMAND} -E make_directory lib
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o lib/${lib}.so *.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()


# HiggsSignals
set(name "higgssignals")
set(ver "1.4.0")
set(lib "libhiggssignals")
set(dl "https://higgsbounds.hepforge.org/downloads/HiggsSignals-${ver}.tar.gz")
set(md5 "537d3885b1cbddbe1163dbc843ec2beb")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")
set(hb_name "higgsbounds")
set(hb_ver "4.3.1")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DEPENDS higgsbounds_${hb_ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    PATCH_COMMAND patch -p1 < ${patch}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy configure my_configure
              COMMAND sed ${dashi} -e "s|HBLIBS =.*|HBLIBS =-L../../${hb_name}/${hb_ver}|" my_configure
              COMMAND sed ${dashi} -e "s|HBINCLUDE =.*|HBINCLUDE =-I../../${hb_name}/${hb_ver}|" my_configure
              COMMAND sed ${dashi} -e "s|F90C =.*|F90C = ${CMAKE_Fortran_COMPILER}|" my_configure
              COMMAND sed ${dashi} -e "s|F77C =.*|F77C = ${CMAKE_Fortran_COMPILER}|" my_configure
              COMMAND sed ${dashi} -e "s|F90FLAGS =.*|F90FLAGS = ${BACKEND_Fortran_FLAGS}|" my_configure
              COMMAND sed ${dashi} -e "s|\\.SUFFIXES|.NOTPARALLEL:${nl}${nl}.SUFFIXES|" makefile.in
              COMMAND ${CMAKE_COMMAND} -E copy makefile.in makefile.in.tmp
              COMMAND awk "{gsub(/${nl}/,${true_nl})}{print}" makefile.in.tmp > makefile.in
              COMMAND ${CMAKE_COMMAND} -E remove makefile.in.tmp
              COMMAND ./my_configure
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
          COMMAND ${CMAKE_COMMAND} -E make_directory lib
          COMMAND ${CMAKE_COMMAND} -E remove HiggsSignals.o
          COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -o lib/${lib}.so ./*.o ../../${hb_name}/${hb_ver}/*.o" > make_so.sh
          COMMAND chmod u+x make_so.sh
          COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()


# SPheno
set(name "spheno")
set(ver "3.3.8")
set(lib "lib/libSPheno.so")
set(dl "https://${name}.hepforge.org/downloads/SPheno-${ver}.tar.gz")
set(md5 "4307cb4b736cebca5e57ca6c5e0b5836")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}_${ver}.dif")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} $F90=${CMAKE_Fortran_COMPILER} FFLAGS=${BACKEND_Fortran_FLAGS} ${lib}
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()


# gm2calc
set(name "gm2calc")
set(ver "1.3.0")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "1bddab5a411a895edd382a1f8a991c15")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}")
# - Silence the deprecated-declarations warnings coming from Eigen3
set(GM2CALC_CXX_FLAGS "${BACKEND_CXX_FLAGS}")
set_compiler_warning("no-deprecated-declarations" GM2CALC_CXX_FLAGS)
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}_error.dif
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${GM2CALC_CXX_FLAGS} EIGENFLAGS=-I${EIGEN3_INCLUDE_DIR} BOOSTFLAGS=-I${Boost_INCLUDE_DIR} alllib
    INSTALL_COMMAND ""
  )
  BOSS_backend(${name} ${ver})
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# gm2calc
set(name "gm2calc")
set(ver "1.2.0")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "07d55bbbd648b8ef9b2d69ad1dfd8326")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}/patch_${name}")
# - Silence the deprecated-declarations warnings coming from Eigen3
set(GM2CALC_CXX_FLAGS "${BACKEND_CXX_FLAGS}")
set_compiler_warning("no-deprecated-declarations" GM2CALC_CXX_FLAGS)
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}_makefile.dif
          COMMAND patch -p1 < ${patch}_module.dif
          COMMAND patch -p1 < ${patch}_error.dif
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${GM2CALC_CXX_FLAGS} EIGENFLAGS=-I${EIGEN3_INCLUDE_DIR} BOOSTFLAGS=-I${Boost_INCLUDE_DIR} sharedlib
    INSTALL_COMMAND ""
  )
  BOSS_backend(${name} ${ver})
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()

# SUSYHD
set(name "susyhd")
set(ver "1.0.2")
set(dl "http://users.ictp.it/~${name}/v${ver}/SUSYHD.tgz")
set(md5 "e831c3fa977552ff944e0db44db38e87")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(ditch_if_absent "Mathematica")
check_ditch_status(${name} ${ver} ${ditch_if_absent})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# cfitsio
set(name "cfitsio")
set(ver "3.390")
set(lib "libcfitsio")
set(dl "http://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio3390.tar.gz")
set(md5 "e92dd2a4282a1c50d46167041a29fc67")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(CFITSIO_SO ".so")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure --includedir=${dir}/include --libdir=${dir}/lib FC=${CMAKE_Fortran_COMPILER} FCFLAGS=${BACKEND_Fortran_FLAGS} FFLAGS=${BACKEND_Fortran_FLAGS} CC=${CMAKE_C_COMPILER} CFLAGS=${BACKEND_C_FLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${BACKEND_CXX_FLAGS} SHLIB_SUFFIX=${CFITSIO_SO}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} shared SHLIB_SUFFIX=${CFITSIO_SO}
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install SHLIB_SUFFIX=${CFITSIO_SO}
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# plc
set(name "plc")
set(ver "2.0")
set(lib "libclik")
set(dl "https://pla.esac.esa.int/pla-sl/data-action?COSMOLOGY.COSMOLOGY_OID=1904")
set(md5 "1d732465a5cc8833cec72a414676c655")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(cfitsio_name "cfitsio")
set(cfitsio_ver "3.390")
set(cfitsio_dir "${PROJECT_SOURCE_DIR}/Backends/installed/${cfitsio_name}/${cfitsio_ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DEPENDS ${cfitsio_name}_${cfitsio_ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND python ${dir}/waf configure --cfitsio_include=${cfitsio_dir}/include --cfitsio_lib=${cfitsio_dir}/lib
    BUILD_COMMAND ""
    INSTALL_COMMAND python ${dir}/waf install
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# class
set(name "class")
set(ver "2.6.3")
set(lib "libclass")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}")
set(dl "https://github.com/lesgourg/class_public/archive/v${ver}.tar.gz")
set(md5 "e6eb0fd721bb1098e642f5d1970501ce")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}/${name}_${ver}.diff
    ## PATCH_COMMAND patch -p0 < ${patch}/primordial_header.dif
    ## COMMAND patch -p0 < ${patch}/primordial_source.dif
    ## COMMAND patch -p0 < ${patch}/input_source.dif
    ## COMMAND patch -p0 < ${patch}/output_source.dif
    ## COMMAND patch -p0 < ${patch}/spectra_source.dif
    ## COMMAND patch -p0 < ${patch}/transfer_source.dif
    ## COMMAND patch -p0 < ${patch}/perturbations_source.dif
    ## COMMAND patch -p0 < ${patch}/thermo_source.dif
    ## COMMAND patch -p0 < ${patch}/backgrnd_source.dif
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CC=${CMAKE_C_COMPILER} OMPFLAG= OPTFLAG= CCFLAG=${BACKEND_C_FLAGS} LDFLAG=${BACKEND_C_FLAGS} class
    COMMAND ${CMAKE_COMMAND} -E make_directory lib
    COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_C_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} ${BACKEND_C_FLAGS} -o lib/${lib}.so build/*.o" > make_so.sh
    COMMAND chmod u+x make_so.sh
    COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# exoclass
set(name "exoclass")
set(ver "2.7.0")
set(lib "libclass")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}")
set(dl "https://github.com/lesgourg/class_public/archive/ExoCLASS.tar.gz")
set(md5 "54e5700ebef5ac8ef6c3a073edaa87ea")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}/${name}_${ver}.diff
    COMMAND patch -p1 < ${patch}/${name}_${ver}_decay_fix.diff
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} CC=${CMAKE_C_COMPILER} OMPFLAG= OPTFLAG= CCFLAG=${BACKEND_C_FLAGS} LDFLAG=${BACKEND_C_FLAGS} class
    COMMAND ${CMAKE_COMMAND} -E make_directory lib
    COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_C_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} ${BACKEND_C_FLAGS} -o lib/${lib}.so build/*.o" > make_so.sh
    COMMAND chmod u+x make_so.sh
    COMMAND ./make_so.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

#MontePython
set(name "MontePython")
set(ver "2.2.2")
set(dl "https://github.com/baudren/montepython_public/archive/2.2.2.zip")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(md5 "93a34a7d989c4754516f3745f872abeb")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

#DarkAges
set(name "darkages")
set(ver "1.0.0")
set(dl "null")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    GIT_REPOSITORY https://github.com/pstoecker/DarkAges.git
    GIT_TAG v1.0.0
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
endif()

set(name "darkages")
set(ver "1.1.0")
set(dl "null")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
check_ditch_status(${name} ${ver})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    GIT_REPOSITORY https://github.com/pstoecker/DarkAges.git
    GIT_TAG v1.1.0
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

#MultiModeCode
set(name "MultiModeCode")
set(ver "2.0.0")
set(lib "libmodecode")
set(dl "http://modecode.org/wp-content/uploads/2014/09/MultiModeCode.2.0.0.tar.gz")
set(md5 "03f99f02c572ea34383a0888fb0658d6")
set(dir "${PROJECT_SOURCE_DIR}/Backends/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/Backends/patches/${name}/${ver}")
set(driver "${PROJECT_SOURCE_DIR}/Backends/include/gambit/Backends/backend_types/MultiModeCode_2_0_0")
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_BACKEND} ${dl} ${md5} ${dir}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}/modpk.dif
          COMMAND patch -p1 < ${patch}/modpk_modules.dif
          COMMAND patch -p1 < ${patch}/modpk_backgrnd_2.dif
          COMMAND patch -p1 < ${patch}/modpk_potential.dif
          COMMAND patch -p1 < ${patch}/modpk_utils.dif
          COMMAND patch -p1 < ${patch}/multimodecode_driver.dif
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} FC=${CMAKE_Fortran_COMPILER} F90C=${CMAKE_Fortran_COMPILER} FFLAG=${BACKEND_Fortran_FLAGS}
    COMMAND ${CMAKE_COMMAND} -E copy ${driver}/multimodecode_gambit.f90 ${dir}
    COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${BACKEND_Fortran_FLAGS} -c ${dir}/multimodecode_gambit.f90" > make_so1.sh
    COMMAND chmod u+x make_so1.sh
    COMMAND ./make_so1.sh
    COMMAND ${CMAKE_COMMAND} -E make_directory lib
    COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} ${BACKEND_Fortran_FLAGS} -o lib/${lib}.so *.o" > make_so2.sh
    COMMAND chmod u+x make_so2.sh
    COMMAND ./make_so2.sh
    INSTALL_COMMAND ""
  )
  add_extra_targets("backend" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("backend" ${name} ${ver})
endif()

# Alternative download command for getting unreleased things from the gambit_internal repository.
# If you don't know what that is, you don't need to tinker with these.
#    DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow --bold ${private_code_warning1}
#             COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${private_code_warning2}
#             COMMAND ${CMAKE_COMMAND} -E copy_directory ${loc} ${dir}
