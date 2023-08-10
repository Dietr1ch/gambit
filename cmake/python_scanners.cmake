
macro(check_python_modules name modules)
  set(_modules ${modules} ${ARGN})
  string (REPLACE "," ";" _modules "${_modules}")
  string (REPLACE " " "" _modules "${_modules}")
  foreach(module ${_modules})
    if (NOT DEFINED PY_${module}_FOUND)
      gambit_find_python_module(${module})
      if (NOT PY_${module}_FOUND)
        set(PY_${module}_FOUND FALSE)
      endif()
    endif()
    if (NOT PY_${module}_FOUND)
      set(modules_missing_${name} "${modules_missing_${name}},${module}" )
    endif()
  endforeach()
endmacro()

macro(inform_of_missing_modules name missing_with_commas)
  string (REPLACE "," " " missing "${missing_with_commas}")
  set(package ${name})
  set(rmstring "${CMAKE_BINARY_DIR}/${package}-prefix/src/${package}-stamp/${package}-configure")
  set(errmsg1 "Cannot make ${package} because you are missing Python module(s):${missing}")
  set(errmsg2 "Please install the missing package(s), e.g. with ")
  set(errmsg3 "  pip install --user${missing}")
  set(errmsg4 "and then rerun ")
  set(errmsg5 "  make ${package}")
  ExternalProject_Add(${package}
    DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E make_directory ${package}-prefix/src/${package}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo
              COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${errmsg1}
              COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${errmsg2}
              COMMAND ${CMAKE_COMMAND} -E echo
              COMMAND ${CMAKE_COMMAND} -E cmake_echo_color       --bold ${errmsg3}
              COMMAND ${CMAKE_COMMAND} -E echo
              COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${errmsg4}
              COMMAND ${CMAKE_COMMAND} -E echo
              COMMAND ${CMAKE_COMMAND} -E cmake_echo_color       --bold ${errmsg5}
              COMMAND ${CMAKE_COMMAND} -E echo
    BUILD_COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/cmake/backends.cmake
    INSTALL_COMMAND ${CMAKE_COMMAND} -E remove ${rmstring}
  )
endmacro()

macro(check_python_scanner  name required_modules)
    message("   Checking for Python modules required by the scanner ${name}")
    check_python_modules(${name}  ${required_modules})
    if(modules_missing_${name})
        inform_of_missing_modules(${name}  ${modules_missing_${name}})
    else()
    ExternalProject_Add(${name}
      DOWNLOAD_COMMAND ""
      #SOURCE_DIR ${dir}
      BUILD_IN_SOURCE 1
      PATCH_COMMAND ""
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      #${CMAKE_COMMAND} -E copy_directory ${examples_dir} ${dir}
      INSTALL_COMMAND ""
    )
    endif(modules_missing_${name})
    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL 1)
endmacro()

check_python_scanner(emcee "emcee,numpy,h5py")
check_python_scanner(adaptive_learner_nd "adaptive,numpy")
check_python_scanner(static_dynesty "dynesty,pickle")
check_python_scanner(dynamic_dynesty "dynesty,pickle")
check_python_scanner(pocomc "pocomc,pickle,numpy")
check_python_scanner(scipy_dual_annealing "scipy")
check_python_scanner(scipy_basin_hopping "scipy")
check_python_scanner(scipy_differential_evolution "scipy")
check_python_scanner(scipy_direct "scipy")
check_python_scanner(reactive_ultranest "ultranest,pickle")
check_python_scanner(zeus "zeus,numpy")
check_python_scanner(kombine "kombine,numpy,pickle")
check_python_scanner(nautilus "nautilus")
check_python_scanner(nessai_flow_sampler "nessai")
check_python_scanner(stochopy_minimize "stochopy,numpy")
check_python_scanner(stochopy_sample "stochopy,numpy")

