#!/usr/bin/env python
#
#  GAMBIT: Global and Modular BSM Inference Tool
#  *********************************************
#  \file
#
#  Backend and type harvesting script.
#  Generates:
#    backend_rollcall.hpp
#    backend_types_rollcall.hpp
#
#  This script identifies all the frontend
#  and type headers, and includes them in the
#  relevant auto-generated header files.
#  It also excludes specific backends from this
#  process if specifically asked to.
#
#*********************************************
#
#  Authors (add name and date if you modify):
#
#  \author Pat Scott
#          (patscott@physics.mcgill.ca)
#  \date 2014 Nov
#
#  \author Ben Farmer
#          (b.farmer@imperial.ac.uk)
#  \date 2018 Oct
#
#*********************************************
import os

toolsfile="./Utils/scripts/harvesting_tools.py"
exec(compile(open(toolsfile, "rb").read(), toolsfile, 'exec')) # Python 2/3 compatible version of 'execfile'


def extract_yaml_for_diagnostic(headers):
    """Parses the list of header files and reorganizes them into a dictionary of backend:versions.

    The output is to be used with the diagnostic system. Should be called twice, once for included, once for excluded headers.
    This implementation works for the backend harvester.

    Args:
        headers (list): List of collected header files from the backend harvester.

    Returns:
        dict: backend name key with a list of versions as value
    """
    # Remove the file extension
    headers_without_file_extension = [header.split(".")[0] for header in headers]
    # Get a list of backend names without version numbers
    backend_names = {"_".join(x for x in header.split("_") if not x[0].isdigit()) for header in headers_without_file_extension}

    backend_yaml = {}
    for backend in sorted(backend_names):
        # Because of some overlapping names, we need to also check that the extracted substring actually starts with the version number
        backend_yaml[backend] = sorted(["_".join(x.split(backend)[1:])[1:].replace("_", ".") for x in headers_without_file_extension if x.startswith(backend) and x.split(backend)[1][1].isdigit()])
    return backend_yaml



def main(argv):

    frontend_headers=set([])
    frontend_headers_excluded=set([])
    backend_type_headers = set([])
    bossed_backend_type_headers = set([])
    exclude_backends=set([])

    # Handle command line options
    verbose = False
    try:
        opts, args = getopt.getopt(argv,"vx:",["verbose","exclude-backends="])
    except getopt.GetoptError:
        print('Usage: backend_harvestor.py [flags]')
        print(' flags:')
        print('        -v                       : More verbose output')
        print('        -x backend1,backend2,... : Exclude backend1, backend2, etc.')
        sys.exit(2)
    for opt, arg in opts:
      if opt in ('-v','--verbose'):
        verbose = True
        print('backend_harvester.py: verbose=True')
      elif opt in ('-x','--exclude-backends'):
        exclude_backends.update(neatsplit(",",arg))

    # Get list of frontend header files to include in backend_rollcall.hpp
    frontend_headers.update(retrieve_generic_headers(verbose,"./Backends/include/gambit/Backends/frontends","frontend",exclude_backends))
    frontend_headers_excluded.update(retrieve_generic_headers(verbose,"./Backends/include/gambit/Backends/frontends","frontend",exclude_backends, retrieve_excluded=True))
    # Get list of backend type header files
    backend_type_headers.update(retrieve_generic_headers(verbose,"./Backends/include/gambit/Backends/backend_types","backend type",set([])))
    bossed_backend_type_headers.update(retrieve_generic_headers(verbose,"./Backends/include/gambit/Backends/backend_types","BOSSed type",set([])))
    # Remove bossed backends from list of excluded backends
    exclude_backends = set([be for be in exclude_backends if not any([excluded(bossed_be, [be]) for bossed_be in bossed_backend_type_headers])])
    # Get list of frontend header files to include in backend_rollcall.hpp
    frontend_headers.update(retrieve_generic_headers(verbose,"./Backends/include/gambit/Backends/frontends","frontend",exclude_backends))

    if verbose:
        print("Frontend headers identified:")
        for h in frontend_headers:
            print('  gambit/Backends/frontends/'+h)
        print("Backend type headers identified:")
        for h in backend_type_headers:
            print('  gambit/Backends/backend_types/'+h)
        for h in bossed_backend_type_headers:
            print('  gambit/Backends/backend_types/'+h)

    # Generate a c++ header containing all the frontend headers we have just harvested.
    towrite = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file                                       \n\
///                                               \n\
///  Compile-time registration of available back- \n\
///  ends.                                        \n\
///                                               \n\
///  This file was automatically generated by     \n\
///  backend_harvester.py. Do not modify.         \n\
///                                               \n\
///  Do not add to this if you want to add a new  \n\
///  backend -- just add your frontend header to  \n\
///  Backends/include/gambit/Backends/frontends   \n\
///  and rest assured that backend_harvester.py   \n\
///  will make sure it ends up here.              \n\
///                                               \n\
///  *********************************************\n\
///                                               \n\
///  Authors (add name and date if you modify):   \n\
///                                               \n\
///  \\author The GAMBIT Collaboration            \n\
///  \date "+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+"\n\
///                                               \n\
///  *********************************************\n\
                                                  \n\
#ifndef __backend_rollcall_hpp__                  \n\
#define __backend_rollcall_hpp__                  \n\
                                                  \n\
// Include the backend macro definitions          \n\
#include \"gambit/Backends/backend_macros.hpp\"   \n\
                                                  \n\
// Automatically-generated list of frontends.     \n"

    for h in frontend_headers:
        towrite+='#include \"gambit/Backends/frontends/{0}\"\n'.format(h)
    towrite+="\n#endif // defined __backend_rollcall_hpp__\n"

    with open("./Backends/include/gambit/Backends/backend_rollcall.hpp","w") as f:
        f.write(towrite)

    # Generate a c++ header containing all the frontend headers we have just harvested.
    towrite = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file                                       \n\
///                                               \n\
///  Compile-time registration of backend types.  \n\
///                                               \n\
///  This file was automatically generated by     \n\
///  backend_harvester.py. Do not modify.         \n\
///                                               \n\
///  Do not add to this if you want to add new    \n\
///  types associated with a new (or old) backend.\n\
///  Just add your backend type headers to the    \n\
///  directory tree in the right place, and rest  \n\
///  assured that backend_harvester.py will make  \n\
///  sure they end up here.                       \n\
///                                               \n\
///  Where is \'the right place\'?                \n\
///  -Regular backend types: in a new header file \n\
///    Backends/include/gambit/Backends/backend_types/X_types.hpp \n\
///   where X is the name of the backend.         \n\
///  -When the types you want to add are a result \n\
///   of running BOSS: add the directory created  \n\
///   by BOSS as                                  \n\
///    Backends/include/gambit/Backends/backend_types/X/ \n\
///   where X is the name of the backend.  BOSS   \n\
///   will actually do this itself automatically  \n\
///   in some circumstances.                      \n\
///                                               \n\
///  *********************************************\n\
///                                               \n\
///  Authors (add name and date if you modify):   \n\
///                                               \n\
///  \\author The GAMBIT Collaboration            \n\
///  \date "+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+"\n\
///                                               \n\
///  *********************************************\n\
                                                  \n\
#ifndef __backend_types_rollcall_hpp__            \n\
#define __backend_types_rollcall_hpp__            \n\
                                                  \n\
// Macro definitions                              \n\
#include \"gambit/Utils/type_macros.hpp\"         \n\
                                                  \n\
// Regular backend type definitions.              \n"
    for h in backend_type_headers:
        towrite+='#include \"gambit/Backends/backend_types/{0}\"\n'.format(h)

    towrite += "\n// BOSSed backend type definitions.\n"
    for h in bossed_backend_type_headers:
        towrite+='#include \"gambit/Backends/backend_types/{0}\"\n'.format(h)

    towrite+="\n#endif // defined __backend_types_rollcall_hpp__\n"

    with open("./Backends/include/gambit/Backends/backend_types_rollcall.hpp","w") as f:
        f.write(towrite)

    if verbose:
        print("Generated backend_rollcall.hpp.")
        print("Generated backend_types_rollcall.hpp.\n")

    import yaml
    with open("./config/gambit_backends.yaml", "w+") as f:
        yaml.dump({
            "enabled": extract_yaml_for_diagnostic(frontend_headers),
            "disabled": extract_yaml_for_diagnostic(frontend_headers_excluded)
        }, f)

# Handle command line arguments (verbosity)
if __name__ == "__main__":
   main(sys.argv[1:])
