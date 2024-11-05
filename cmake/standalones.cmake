# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  CMake configuration script for standalone
#  programs that use some GAMBIT libraries.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2016
#
#  \author Tomas Gonzalo
#          (tomas.gonzalo@monash.edu)
#  \date 2020
#
#  \author Anders Kvellestad
#          (anders.kvellestad@fys.uio.no)
#  \date 2023
#
#************************************************

# Add some programs that use the GAMBIT physics libraries but not GAMBIT itself.
option(WITH_COLLIDERBIT_STANDALONE "Compile ColliderBit example binaries" ON)
option(WITH_DARKBIT_STANDALONE     "Compile DarkBit example binaries" ON)
option(WITH_DECAYBIT_STANDALONE    "Compile DecayBit example binaries" ON)
option(WITH_EXAMPLEBIT_STANDALONE  "Compile ExampleBit example binaries" ON)
option(WITH_FLAVBIT_STANDALONE     "Compile FlavBit example binaries" ON)
option(WITH_NEUTRINOBIT_STANDALONE "Compile NeutrinoBit example binaries" ON)


if (WITH_COLLIDERBIT_STANDALONE)
  # ../ColliderBit/examples/solo.cpp
  add_standalone(CBS SOURCES ColliderBit/examples/solo.cpp MODULES ColliderBit DEPENDENCIES hepmc pybind11)
endif()

if (WITH_DARKBIT_STANDALONE)
  # ../DarkBit/examples/DarkBit_standalone_MSSM.cpp
  add_standalone(DarkBit_standalone_MSSM SOURCES DarkBit/examples/DarkBit_standalone_MSSM.cpp MODULES DarkBit)
  # ../DarkBit/examples/DarkBit_standalone_ScalarSingletDM_Z2.cpp
  add_standalone(DarkBit_standalone_ScalarSingletDM_Z2 SOURCES DarkBit/examples/DarkBit_standalone_ScalarSingletDM_Z2.cpp MODULES DarkBit)
  # ../DarkBit/examples/DarkBit_standalone_WIMP.cpp
  add_standalone(DarkBit_standalone_WIMP SOURCES DarkBit/examples/DarkBit_standalone_WIMP.cpp MODULES DarkBit DEPENDENCIES pybind11)
endif()

if (WITH_DECAYBIT_STANDALONE)
  # ../DecayBit/examples/3bithit.cpp
  add_standalone(3bithit SOURCES DecayBit/examples/3bithit.cpp MODULES DecayBit SpecBit PrecisionBit)
endif()

if (WITH_EXAMPLEBIT_STANDALONE)
  # ../ExampleBit_A/examples/ExampleBit_A_standalone_example.cpp
  add_standalone(ExampleBit_A_standalone SOURCES ExampleBit_A/examples/ExampleBit_A_standalone_example.cpp MODULES ExampleBit_A)
endif()

if (WITH_FLAVBIT_STANDALONE)
  # ../FlavBit/examples/FlavBit_standalone_example.cpp
  add_standalone(FlavBit_standalone SOURCES FlavBit/examples/FlavBit_standalone_example.cpp MODULES FlavBit)
endif()

if (WITH_NEUTRINOBIT_STANDALONE)
  # ../NeutrinoBit/examples/standalone.cpp
  add_standalone(NeutrinoBit_standalone SOURCES NeutrinoBit/examples/standalone.cpp MODULES NeutrinoBit)
endif()

# Add a message that is only shown if CBS is built 
# and -O3 level compiler optimisations are not activated.
if(EXISTS CBS AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release" AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  add_custom_command(
    TARGET CBS POST_BUILD
    COMMENT "\n${BoldYellow}-- You have built CBS with CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}. For best performance we recommend building CBS in 'Release' mode. You can do this by rerunning cmake with the option -DCMAKE_BUILD_TYPE=Release and then rebuild CBS. ${ColourReset}\n\n"
  )
endif()
