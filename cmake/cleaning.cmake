# GAMBIT: Global and Modular BSM Inference Tool  
#************************************************
# \file                                          
#                                                
#  Cmake configuration script to add clean info
#  for GAMBIT.  
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
#  \date 2015 Feb       
#                                        
#************************************************

##### clean ########

# Get a list of all bits, including ditched ones.
retrieve_bits(ALL_GAMBIT_BITS ${PROJECT_SOURCE_DIR} "" "Quiet")

# Ensure that clean removes automatically-generated CMakeLists.txt and source files in modules
foreach(bit ${ALL_GAMBIT_BITS})
  set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/${bit}/CMakeLists.txt")
  if(NOT ScannerBit STREQUAL ${bit})
    set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/${bit}/examples/standalone_functors.cpp")
  endif()
endforeach()
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Backends/CMakeLists.txt")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Printers/CMakeLists.txt")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Models/CMakeLists.txt")

# Make sure clean removes the scratch files indicating that the harvesters have been run.
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/scratch/models_harvested")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/scratch/backends_harvested")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/scratch/modules_harvested")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/scratch/printers_harvested")

# Arrange for removal of all_functor_types.hpp and other generated headers upon "make clean".
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Models/include/gambit/Models/model_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Models/include/gambit/Models/model_types_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Backends/include/gambit/Backends/backend_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Backends/include/gambit/Backends/backend_types_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Core/include/gambit/Core/module_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Elements/include/gambit/Elements/module_types_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Elements/include/gambit/Elements/all_functor_types.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/Printers/include/gambit/Printers/printer_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/cmake/include/gambit/cmake/cmake_variables.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/ScannerBit/include/gambit/ScannerBit/priors_rollcall.hpp")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/ScannerBit/include/gambit/ScannerBit/test_function_rollcall.hpp")

#Arrange for removal of other scanner-related generated files upon "make clean".
set(clean_files ${clean_files} "${PROJECT_BINARY_DIR}/linkedout.cmake")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/scratch/scanbit_reqd_entries.yaml")
set(clean_files ${clean_files} "${PROJECT_SOURCE_DIR}/scratch/scanbit_flags.yaml")

# Add all the clean files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${clean_files}")


##### distclean ########

# Add a true clean target that can have dependencies, to allow us to trigger cleaning of external projects (or run any other custom commands)
add_custom_target(distclean COMMAND ${CMAKE_MAKE_PROGRAM} clean)

# Ensure that distclean cleans the backends (the entry for each backend will be added in backends.cmake)
add_custom_target(clean-backends)
add_dependencies(distclean clean-backends)

# Ensure that disclean cleans the scanners (the entry for each backend will be added in scanners.cmake)
add_custom_target(clean-scanners)
add_custom_target(clean-scanners-lib COMMAND ${CMAKE_COMMAND} -E remove * WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ScannerBit/lib)
add_dependencies(distclean clean-scanners clean-scanners-lib)

# Ensure that distclean sweeps out the scratch directory
add_custom_target(clean-scratch COMMAND ${CMAKE_COMMAND} -E remove_directory scratch WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_dependencies(distclean clean-scratch)

# Ensure that distclean removes .pyc files
add_custom_target(clean-pyc COMMAND ${CMAKE_COMMAND} -E remove *.pyc */*.pyc */*/*.pyc */*/*/*.pyc */*/*/*/*.pyc WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_dependencies(distclean clean-pyc)

# Ensure that distclean removes backup files
add_custom_target(clean-backup COMMAND ${CMAKE_COMMAND} -E remove *~ */*~ */*/*~ */*/*/*~ */*/*/*/*~ WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_dependencies(distclean clean-backup)

# Add clean targets for doxygen
add_custom_target(clean-docs WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                             COMMAND ${CMAKE_COMMAND} -E remove_directory doc/html 
                             COMMAND ${CMAKE_COMMAND} -E remove doc/*.tmp)
add_dependencies(distclean clean-docs)


##### nuke ########

# Do everything in distclean and ensure that nuke sweeps out the backend and scanner download and install directories
add_custom_target(clean-backend-download COMMAND ${CMAKE_COMMAND} -E remove_directory Backends/downloaded WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_target(clean-backend-install COMMAND ${CMAKE_COMMAND} -E remove_directory Backends/installed WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_target(clean-scanner-download COMMAND ${CMAKE_COMMAND} -E remove_directory ScannerBit/downloaded WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_target(clean-scanner-install COMMAND ${CMAKE_COMMAND} -E remove_directory ScannerBit/installed WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_target(nuke-backends DEPENDS clean-backend-download clean-backend-install) 
add_custom_target(nuke-scanners DEPENDS clean-scanner-download clean-scanner-install) 
add_custom_target(nuke-all DEPENDS distclean nuke-backends nuke-scanners) 
