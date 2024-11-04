# From ./README.md
#
# WARNING: This is still broken.

build_gambit:
  @echo 'Building gambit'
  @echo '  Generating build instructions...'
  cmake -B build/ -S .

  @echo '  building scanners...'
  make -j --directory build/ scanners

  @echo '  building gambit...'
  cmake -B build/ -S .
  make -j --directory build/ gambit

build_backends: build_gambit
  @echo 'Building all backends'
  make -j --directory build/ backends

clean:
  @echo 'Removing build/'
  rm -rf ./build/
