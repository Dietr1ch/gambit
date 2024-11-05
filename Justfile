# From ./README.md
#
# WARNING: This is still broken.

some_cpus := "16"


SOURCE_DIR := "."
BUILD_DIR := "build/"

CMAKE  := "cmake -B " + BUILD_DIR + " -S " + SOURCE_DIR
MAKE_BASE  := "make --directory " +BUILD_DIR
MAKE       := MAKE_BASE + " -j" + num_cpus()
MAKE_SLOW  := MAKE_BASE + " -j" + some_cpus


build_gambit:
    @echo 'Building gambit'
    @echo '  Generating build instructions...'
    mkdir -p {{BUILD_DIR}}
    {{CMAKE}}

    @echo '  building scanners...'
    {{MAKE}} scanners

    @echo '  building gambit...'
    {{CMAKE}} -Ditch="great"
    {{MAKE_SLOW}} gambit  # This may have a race condition. Re-running a dirty build seems to work.

build_backends: build_gambit
    @echo 'Building all backends'
    {{MAKE}} backends


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
