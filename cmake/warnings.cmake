# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  Cmake configuration script to arrange warning
#  options when compiling GAMBIT.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Antje Putze
#          (antje.putze@lapth.cnrs.fr)
#  \date 2014 Sep, Oct, Nov
#
#  \author Andy Buckley
#          (andy.buckley@cern.ch)
#  \date 2016 Feb
#
#************************************************

option(WERROR "WERROR" OFF)

include(CheckCXXCompilerFlag)

macro(set_compiler_warning warning current_flags)
  CHECK_CXX_COMPILER_FLAG("-W${warning}" CXX_SUPPORTS_${warning})
  if (CXX_SUPPORTS_${warning})
    set(${current_flags} "${${current_flags}} -W${warning}")
  else()
    message("${Red}-W${warning} not recognised by compiler${ColourReset}"
  endif()
endmacro()

if(${WERROR})
  set_compiler_warning("error")
else()
  message(STATUS "${Red}Werror is disabled${ColourReset}")
endif()

set_compiler_warning("all" CMAKE_CXX_FLAGS)
set_compiler_warning("extra" CMAKE_CXX_FLAGS)
set_compiler_warning("no-unused-local-typedefs" CMAKE_CXX_FLAGS)
set_compiler_warning("no-unknown-pragmas" CMAKE_CXX_FLAGS)
set_compiler_warning("no-misleading-indentation" CMAKE_CXX_FLAGS)
set_compiler_warning("no-ignored-attributes" CMAKE_CXX_FLAGS)
set_compiler_warning("no-literal-suffix" CMAKE_CXX_FLAGS)

# Suppress additional warnings when using clang and ccache
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  EXEC_PROGRAM(sh
               ARGS ${CMAKE_SOURCE_DIR}/cmake/check_for_ccache.sh ${CMAKE_CXX_COMPILER}
               RETURN_VALUE ret)
  if (${ret})
    message(STATUS "Using ccache with clang - disabling some warnings")
    CHECK_CXX_COMPILER_FLAG("-Qunused-arguments" CXX_SUPPORTS_QUNUSED_ARGUMENTS)
    if (CXX_SUPPORTS_QUNUSED_ARGUMENTS)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
    endif()
    CHECK_CXX_COMPILER_FLAG("-Wno-self-assign" CXX_SUPPORTS_WNO_SELF_ASSIGN)
    if (CXX_SUPPORTS_WNO_SELF_ASSIGN)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} Wno-self-assign")
    endif()
  endif()
endif()

# set intel warnings
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # "remark #981: operands are evaluated in unspecified order"
  # This is a false positive, suppress it.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd981")

  # "remark #1418: external function definition with no prior declaration"
  # This can safely be ignord according to the ICC docs.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd1418")

  # "remark #1419: external declaration in primary source file"
  # This can safely be ignord according to the ICC docs.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd1419")
endif()
