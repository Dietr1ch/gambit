# From ./README.md
#
# WARNING: This is still broken.

main: build_gambit
    @echo 'Done :)'

build_gambit:
    @echo 'Building gambit'
    @echo '  Generating build instructions...'
    mkdir -p build/
    cmake -B build/ -S .

    @echo '  building scanners...'
    make --debug -j --directory build/ scanners

    @echo '  building gambit...'
    cmake -B build/ -S . -Ditch="great"
    make --debug -j --directory build/ gambit

build_backends: build_gambit
    @echo 'Building all backends'
    make --debug -j --directory build/ backends

clean:
    @echo 'Removing build/'
    rm -rf ./build/
    mkdir -p build/

clean_thoroughly: clean
    @echo 'Will remove untracked files:'
    git clean -d --force -X --dry-run
    @echo ''
    @echo 'Removing untracked files...'
    git clean -d --force -X


# From https://gambitbsm.org/tutorials/BSM2/Pre_Tutorial_Instructions.txt

build_gambit_tutorial:
    @echo 'Building gambit'
    @echo 'Building gambit'
    @echo '  Generating build instructions for running the Tutorial...'
    cmake -B build/ -S . -DWITH_MPI="on" -DWITH_ROOT="on" -Ditch="Cosmo;Collider;Precision;Mathematica;pybind"  # (yes, do it again, needed to register the scanner plugins with the build system)

    @echo '  building scanners...'
    make --debug -j --directory build/ scanners
    cmake -B build/ -S . -DWITH_MPI="on" -DWITH_ROOT="on" -Ditch="Cosmo;Collider;Precision;Mathematica;pybind"  # (yes, do it again, needed to register the scanner plugins with the build system)

    make --debug -j --directory build/ superiso
    make --debug -j --directory build/ heplike
    make --debug -j --directory build/ gambit
