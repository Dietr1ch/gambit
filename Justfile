# From ./README.md
#
# WARNING: This is still broken.

# Build options
# -------------
# System
some_cpus := "8"
# Paths
SOURCE_DIR := "."
BUILD_DIR := "build/"
# Build target (See ./BUILD_OPTIONS.md)
CMAKE_BULD_TYPE := "-DCMAKE_BUILD_TYPE=Release"
# Features  (See ./BUILD_OPTIONS.md)
CMAKE_MPI := "-DWITH_MPI=ON"
CMAKE_ROOT := "-DWITH_ROOT=On"
CMAKE_GRAPHVIZ := "-DHAVE_GRAPHVIZ=On"
CMAKE_DITCH := "-Ditch='ColliderBit;Mathematica'"
CMAKE_AXEL := "-DWITH_AXEL=ON"
# All options (Can't I just intersperse with " "?)
CMAKE_OPTIONS := CMAKE_BULD_TYPE + " " + CMAKE_DITCH + " " + CMAKE_MPI + " " + CMAKE_ROOT + " " + CMAKE_GRAPHVIZ + " " + CMAKE_AXEL

CMAKE_BASE  := "cmake -B " + BUILD_DIR + " -S " + SOURCE_DIR
CMAKE       := CMAKE_BASE + " " + CMAKE_OPTIONS

# Make
MAKE_BASE  := "make --directory " +BUILD_DIR
MAKE       := MAKE_BASE + " -j" + num_cpus()
MAKE_SLOW  := MAKE_BASE + " -j" + some_cpus


default_target: build_gambit
    @echo 'Done!'


build_cmake:
    @echo 'Generating build instructions...'
    mkdir -p {{BUILD_DIR}}
    {{CMAKE}}


build_standalones: build_cmake
    @echo '  Building standalones...'
    {{MAKE}} standalones


build_neutrinobit_standalone:
    @echo 'Building NeutrinoBit standalone...'

    @echo 'Generating build instructions...'
    mkdir -p {{BUILD_DIR}}
    {{CMAKE}} -WITH_COLLIDERBIT_STANDALONE=OFF -WITH_DARKBIT_STANDALONE=OFF -WITH_DECAYBIT_STANDALONE=OFF -WITH_EXAMPLEBIT_STANDALONE=OFF -WITH_FLAVBIT_STANDALONE=OFF

    {{MAKE}} NeutrinoBit_standalone


build_gambit: build_cmake
    @echo '  Building standalones...'
    {{MAKE}} gambit


build_backends: build_cmake
    @echo 'Building all backends'
    {{MAKE}} backends


print_commands:
    @echo "cmake:  {{CMAKE}}"
    @echo "make:   {{MAKE}}"


format:
    @echo "Formatting all C++ files..."
    clang-format -i **/*.cpp **/*.c **/*.hpp **/*.h


clean_build: clean_thoroughly build_gambit
    @echo 'Done :)'


clean:
    @echo 'Removing build directory'
    rm -rf {{BUILD_DIR}}
    mkdir -p {{BUILD_DIR}}


clean_thoroughly: clean
    @echo 'Will remove untracked files:'
    git clean -d --force -X --dry-run
    @echo ''
    @echo 'Removing untracked files...'
    git clean -d --force -X
    direnv reload
