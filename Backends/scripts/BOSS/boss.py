#!/usr/bin/env python

########################################
#                                      #
#   BOSS - Backend-On-a-Stick Script   #
#                                      #
########################################

#
# This is a Python package for making the classes of a C++ library
# available for dynamic use through the 'dlopen' system.
# BOSS makes use of CastXML to parse the C++ source code.
#
# Default usage:
# ./boss configs/some_config_file.py
#

from __future__ import print_function
import xml.etree.ElementTree as ET
import os
import sys
import shutil
import glob
import subprocess
import pickle
from collections import OrderedDict
from optparse import OptionParser

# ====== main ========

def main():

    # print()
    # print()
    # print('  ========================================')
    # print('  ||                                    ||')
    # print('  ||  BOSS - Backend-On-a-Stick-Script  ||')
    # print('  ||                                    ||')
    # print('  ========================================')
    # print()
    # print()


    # Parse command line arguments and options
    usage_string = "Usage: %prog [options] <config module>"
    usage_string += "\n\n"
    usage_string += "<config module> refers to a config file configs/<config module>.py."
    usage_string += "\n\n"
    usage_string += "Example: '%prog example_1_234' will use the config file configs/example_1_234.py."


    parser = OptionParser(usage=usage_string,
                          version="%prog 0.1")
    parser.add_option("--castxml-cc-id",
                      dest="castxml_cc_id",
                      default="",
                      help="Set the reference compiler for CastXML to ID, e.g. 'gnu'.",
                      metavar="ID")
    parser.add_option("--castxml-cc",
                      dest="castxml_cc",
                      default="",
                      help="Tell CastXML to use the specific compiler COMPILER, e.g. 'g++'.",
                      metavar="COMPILER")
    parser.add_option("--castxml-cc-opt",
                      dest="castxml_cc_opt",
                      default="",
                      help="Provide the additional flags OPTIONS to the compiler.",
                      metavar="OPTIONS")
    parser.add_option("-l", "--list",
                      action="store_true",
                      dest="list_flag",
                      default=False,
                      help="Output a list of the available classes and functions.")
    parser.add_option("-g", "--generate-only",
                      action="store_true",
                      dest="generate_only_flag",
                      default=False,
                      help="Stop BOSS after code generation step.")
    parser.add_option("-t", "--types-header",
                      action="store_true",
                      dest="types_header_flag",
                      default=False,
                      help="Generate loaded_types.hpp. (BOSS continues from a previous saved state, no other input required.)")
    parser.add_option("-r", "--reset-source",
                      dest="reset_info_file_name",
                      default="",
                      help="Reset source code that has been mangled by BOSS. Requires a RESET_INFO_FILE generated by BOSS.",
                      metavar="RESET_INFO_FILE")
    parser.add_option("-I", "--include",
                      action="append",
                      dest="cmdline_include_paths",
                      help="Add PATH to the list of paths used by CastXML to search for included header files.",
                      metavar="PATH")
    parser.add_option("--no-instructions",
                      action="store_true",
                      dest="no_instructions_flag",
                      default=False,
                      help="When BOSS finishes, don't print the instructions for manually connecting the BOSS-generated output to GAMBIT.")
    # parser.add_option("-G", "--to-gambit",
    #                   dest="main_gambit_path",
    #                   default="",
    #                   help="Copy all the code needed by GAMBIT to the correct locations within the main GAMBIT path GAMBIT_PATH.",
    #                   metavar="GAMBIT_PATH")


    (options, args) = parser.parse_args()

    # Print banner
    msg = '''\033[1m

  ========================================
  ||                                    ||
  ||  BOSS - Backend-On-a-Stick-Script  ||
  ||                                    ||
  ========================================
    \033[0m'''
    # msg = utils.modifyText(msg,'bold')
    print( msg)


    # Check for conflicting options
    if options.generate_only_flag and options.types_header_flag:
        print()
        print('Conflicting flags: --generate-only and --types-header')
        print()

        sys.exit()


    # Check that arguments list is not empty
    if (len(args) < 1) and not (options.types_header_flag or options.reset_info_file_name):

        print()
        print('Missing input arguments. For instructions, run: boss.py --help')
        print()

        sys.exit()


    # Check platform
    if not (sys.platform.startswith('linux') or sys.platform == 'darwin'):

        print()
        print('Platform "%s" is not supported.' % (sys.platform))
        print()

        sys.exit()


    # Check that CastXML is found and get the correct path
    # (system-wide executable or prebuilt binary in castxml/bin/castxml)
    boss_abs_dir = os.path.dirname(os.path.abspath(__file__))
    local_castxml_path = os.path.join(boss_abs_dir,"castxml/bin/castxml")
    has_castxml_system = True
    has_castxml_local = True
    try:
        subprocess.check_output(["which","castxml"])
    except subprocess.CalledProcessError:
        print()
        print("Cannot find a system-wide 'castxml' executable.")
        print("CastXML can be installed with 'apt install castxml' (Linux) or 'brew install castxml' (OS X).")
        has_castxml_system = False
        print("Will now look for a prebuilt CastXML binary in %s" % (local_castxml_path))

    try:
        subprocess.check_output(["which",local_castxml_path])
    except subprocess.CalledProcessError:
        print()
        print("Cannot find 'castxml' binary in %s." % (local_castxml_path))
        print("To get the prebuilt castxml binary for your system, download a tarball from ")
        print()
        print("  https://midas3.kitware.com/midas/download/item/318227/castxml-linux.tar.gz (for Linux)")
        print()
        print("or ")
        print()
        print("  https://midas3.kitware.com/midas/download/item/318762/castxml-macosx.tar.gz (for OS X)")
        print()
        print("and extract it in the main BOSS directory: %s/" % (boss_abs_dir))
        print()
        has_castxml_local = False

    # Quit if no version of CastXML is found
    if (not has_castxml_system) and (not has_castxml_local): sys.exit()


    # Get the config file name from command line. Import the correct config module.
    # If reset option is used, then skip this part and simply import configs.example_1_234.

    import modules.active_cfg as active_cfg
    if options.reset_info_file_name == '':

        # Get the config file name from command line input, unless reset option is used
        input_cfg_path = args[0]

        # Import the given config file as a module named 'cfg'.
        input_cfg_dir, input_cfg_filename = os.path.split(input_cfg_path)
        input_cfg_modulename = input_cfg_filename.rstrip('.py')

        active_cfg.module_name = input_cfg_modulename

    try:
        exec("import configs." + active_cfg.module_name + " as cfg", globals())
    except ImportError as e:
        print("Failed to import config module '%s'. Are you sure the file %s exists?" % (input_cfg_modulename, os.path.join('configs', input_cfg_modulename + '.py')))
        print()
        sys.exit()

    import modules.gb as gb
    import modules.classutils as classutils
    import modules.classparse as classparse
    import modules.funcparse as funcparse
    import modules.funcutils as funcutils
    import modules.utils as utils
    import modules.filehandling as filehandling
    import modules.infomsg as infomsg

    # Write the result of the castxml checks to gb
    gb.has_castxml_system = has_castxml_system
    gb.has_castxml_local = has_castxml_local

    # Construct file name for the BOSS reset file to be created
    reset_info_file_name = 'reset_info.' + gb.gambit_backend_name_full + '.boss'


    # Convert all input and final output paths in the config file to absolute paths
    cfg.input_files = [(gb.boss_dir + '/' + x if not x.startswith('/') else x) for x in cfg.input_files]
    cfg.include_paths = [(gb.boss_dir + '/' + x if not x.startswith('/') else x) for x in cfg.include_paths]
    cfg.base_paths = [(gb.boss_dir + '/' + x if not x.startswith('/') else x) for x in cfg.base_paths]
    if not cfg.header_files_to.startswith('/'): cfg.header_files_to = gb.boss_dir + '/' + cfg.header_files_to
    if not cfg.src_files_to.startswith('/'): cfg.src_files_to = gb.boss_dir + '/' + cfg.src_files_to

    # If castxml compiler settings are also given as command line input, append or override the values set in cfg.
    # - Override castxml_cc_id option:
    if options.castxml_cc_id != '':
        cfg.castxml_cc_id = " " + options.castxml_cc_id
    print('Running with setting castxml_cc_id="' + cfg.castxml_cc_id + '".')
    # - Override castxml_cc option:
    if options.castxml_cc != '':
        cfg.castxml_cc = " " + options.castxml_cc
    print('Running with setting castxml_cc="' + cfg.castxml_cc + '".')
    # - Append to castxml_cc_opt option, putting the command line input at the end (takes priority if conflicts/duplicates)
    if options.castxml_cc_opt != '':
        cfg.castxml_cc_opt += " " + options.castxml_cc_opt
    print('Running with setting castxml_cc_opt="' + cfg.castxml_cc_opt + '".')


    # If additional include paths are given at the command line,
    # add them to the list given in cfg.
    if options.cmdline_include_paths:
        cfg.include_paths = cfg.include_paths + options.cmdline_include_paths


    # Remove any empty-string elements in the lists of input files and paths
    cfg.input_files = list(filter(None, cfg.input_files))
    cfg.include_paths = list(filter(None, cfg.include_paths))
    cfg.base_paths = list(filter(None, cfg.base_paths))


    #
    # If types_header_flag is True: Load saved variables, parse factory function files and generate loaded_types.hpp
    #
    if options.types_header_flag:
        with open('savefile.boss', 'rb') as f:
            gb.classes_done, gb.factory_info = pickle.load(f)

        print('(Continuing from saved state.)')

        print()
        print()
        print( utils.modifyText('Generating file loaded_types.hpp:','underline'))
        print()

        filehandling.createLoadedTypesHeader()

        print()
        print( utils.modifyText('Done!','bold'))
        print()

        sys.exit()

    #
    # If reset option is used: Run the reset function and then quit
    #
    if options.reset_info_file_name != '':
        print()
        print()
        print( utils.modifyText('Reset source code:','underline'))
        print()
        print('  Input file: ' + options.reset_info_file_name)
        print()

        filehandling.resetSourceCode(options.reset_info_file_name)

        sys.exit()


    #
    # Check if backend source tree has already been BOSSed. (Look for the backend_undefs.hpp header file.)
    #
    check_file = os.path.join(cfg.header_files_to, gb.gambit_backend_incl_dir, 'backend_undefs.hpp')
    if os.path.isfile(check_file):
        print()
        print( utils.modifyText('The backend source tree seems to already have been BOSSed.','yellow'))
        print()
        print('  If you want to reBOSS the source tree you must first revert it to the original state with the command:')
        print('  python boss.py -r ' + reset_info_file_name)
        print()

        sys.exit()



    # If the output directory is to be used, delete the current one if it exists.
    if (not options.list_flag) and (not options.types_header_flag):
        try:
            shutil.rmtree(gb.boss_output_dir)
        except OSError as e:
            if e.errno == 2:
                pass
            else:
                raise e



    #
    # Identify standard include paths
    #
    if cfg.auto_detect_stdlib_paths:
        print()
        print( utils.modifyText('Identifying standard include paths:','underline'))
        print()

        utils.identifyStdIncludePaths()



    #
    # Run castxml for all input header/source files
    #

    print()
    print( utils.modifyText('Parsing the input files:','underline'))
    print()

    # Create the temp output dir if it does not exist
    filehandling.createOutputDirectories(selected_dirs=['temp'])

    # Sort list of input files
    input_files = cfg.input_files
    input_files.sort()

    xml_files = []
    for i,input_file_path in enumerate(input_files):

        # Get path and filename for the input file
        input_file_dir, input_file_short_name = os.path.split(input_file_path)

        # Construct file name for xml file produced by castxml
        xml_output_path = os.path.join(gb.boss_temp_dir, 'tempfile_' + str(i) + '_' + input_file_short_name.replace('.','_') + '.xml' )

        # List all include paths
        # include_paths_list = [cfg.include_path] + cfg.additional_include_paths

        # Run castxml
        try:
            utils.castxmlRunner(input_file_path, cfg.include_paths, xml_output_path)
        except Exception as e:
            print()
            print("  The initial CastXML command failed. Some problems can be solved by simply specifying")
            print("  a different C++ compiler in the 'castxml_cc' option in %s. It is currently set to '%s'." % (input_cfg_path, cfg.castxml_cc))
            print()
            raise e
            sys.exit(1)

        # Append xml file to list of xml files
        xml_files.append(xml_output_path)

    #
    # END: Run castxml on input files
    #
    print()


    #
    # If -l option is given, printa list of all classes and functions, then exit.
    #

    if options.list_flag:

        all_class_names    = []
        all_function_names = []

        for xml_file in xml_files:

            tree = ET.parse(xml_file)
            root = tree.getroot()

            # Set the global xml id dict. (Needed by the functions called from utils.)
            gb.id_dict = OrderedDict([ (el.get('id'), el) for el in list(root) ])

            # Find all available classes
            for el in (root.findall('Class') + root.findall('Struct')):

                try:
                    class_name = classutils.getClassNameDict(el)
                except KeyError:
                    continue

                # Only list native classes
                if utils.isNative(el):
                    all_class_names.append(class_name['long_templ'])


            # Find all available functions
            for el in root.findall('Function'):

                try:
                    func_name = funcutils.getFunctionNameDict(el)
                except KeyError:
                    continue

                # Only list native functions
                if utils.isNative(el):
                    all_function_names.append(func_name['long_templ_args'])


        # END: Loop over xml files

        # Remove duplicates
        all_class_names    = list(OrderedDict.fromkeys(all_class_names))
        all_function_names = list(OrderedDict.fromkeys(all_function_names))

        # Output lists
        print( utils.modifyText('Classes:','underline'))
        print()
        for demangled_class_name in all_class_names:
            print('  - ' + demangled_class_name)
        print()
        print( utils.modifyText('Functions:','underline'))
        print()
        for demangled_func_name in all_function_names:
            print('  - ' + demangled_func_name)
        print()

        # Exit
        sys.exit()



    #
    # Analyse types and functions
    #

    print( utils.modifyText('Analysing types and functions:','underline'))
    print()
    print('  (This may take a little while.)')
    print()
    #
    # Read all xml elements of all files and store in two dict of dicts:
    #
    # 1. all_id_dict:    file name --> xml id --> xml element
    # 2. all_name_dict:  file name --> name   --> xml element
    #
    utils.xmlFilesToDicts(xml_files)

    # Fill the global dictionary gb.std_typedef_names_dict
    utils.initGlobalTypedefDicts(xml_files)


    #
    # Look up potential parent classes and add to cfg.load_classes
    #

    if cfg.load_parent_classes:
        utils.addParentClasses()


    #
    # Remove from cfg.load_classes all classes that are not loadable (not found, incomplete, abstract, ...)
    #

    # Remove duplicates from cfg.load_classes
    cfg.load_classes = list(OrderedDict.fromkeys(cfg.load_classes))

    is_loadable = OrderedDict.fromkeys(cfg.load_classes, False)

    # Determine which requested classes are actually loadable
    for xml_file in xml_files:

        # Initialise global dicts
        gb.xml_file_name = xml_file
        utils.initGlobalXMLdicts(xml_file, id_and_name_only=True)

        # Loop over all named elements in the xml file
        for full_name, el in gb.name_dict.items():

            if el.tag in ['Class', 'Struct']:

                # If a requested class is loadable, set the entry in is_loadable to True
                if full_name in cfg.load_classes:

                    if utils.isLoadable(el, print_warning=False):
                        is_loadable[full_name] = True


    # Remove from cfg.load_classes those that are not loadable
    for full_name in is_loadable.keys():

        if not is_loadable[full_name]:

            cfg.load_classes.remove(full_name)


    # Output info on why classes are not loadable
    for xml_file in xml_files:

        # Initialise global dicts
        gb.xml_file_name = xml_file
        utils.initGlobalXMLdicts(xml_file, id_and_name_only=True)

        for full_name in is_loadable.keys():

            if not is_loadable[full_name]:

                # If the class exists, print reason why it is not loadable.
                # If the class is not found, say so.
                try:
                    el = gb.name_dict[full_name]
                except KeyError:
                    el = None

                if el is not None:
                    utils.isLoadable(el, print_warning=True)
                else:
                    reason = "Class not found."
                    infomsg.ClassNotLoadable(full_name, reason).printMessage()


    #
    # Fill the gb.parents_of_loaded_classes list
    #
    utils.fillParentsOfLoadedClassesList()


    #
    # Fill the gb.accepted_types list
    #
    utils.fillAcceptedTypesList()


    #
    # Remove from cfg.load_functions all functions that are not loadable
    #

    # Remove whitespace in entries in cfg.load_functions
    cfg.load_functions = [func_name.replace(' ', '') for func_name in cfg.load_functions]

    # Remove duplicates from cfg.load_functions
    cfg.load_functions = list(OrderedDict.fromkeys(cfg.load_functions))

    for xml_file in xml_files:

        # Initialise global dicts
        gb.xml_file_name = xml_file
        utils.initGlobalXMLdicts(xml_file, id_and_name_only=True)

        # Loop over all named elements in the xml file
        for full_name, el in gb.name_dict.items():

            if el.tag == 'Function':

                # Get function name
                func_name_long_templ_args = ''
                try:
                    func_name = funcutils.getFunctionNameDict(el)
                    func_name_long_templ_args = func_name['long_templ_args']
                except KeyError:
                    func_name_long_templ_args = 'UNKNOWN_NAME'
                except:
                    print('  ERROR: Unexpected error!')
                    raise

                compare_func_name = func_name_long_templ_args.replace(' ','')
                if compare_func_name in cfg.load_functions:

                    is_loadable = not funcutils.ignoreFunction(el, limit_pointerness=True, print_warning=True)

                    if not is_loadable:

                        cfg.load_functions.remove(compare_func_name)


    #
    # Main loop over all xml files
    #

    # Check that we have something to do...
    if (len(cfg.load_classes) == 0) and (len(cfg.load_functions) == 0):
        print()
        print()
        print('  - No classes or functions to load!')
        print()
        print( utils.modifyText('Done!','bold'))

        sys.exit()

    print()
    print()
    print( utils.modifyText('Generating code:','underline'))

    for xml_file in xml_files:

        # Output xml file name
        print()
        print()
        print('  ' + utils.modifyText('Current XML file:', 'underline') + ' ' + xml_file)
        print()

        #
        # Initialise global dicts with the current XML file
        #

        gb.xml_file_name = xml_file
        utils.initGlobalXMLdicts(xml_file)


        #
        # Parse classes
        #

        classparse.run()


        #
        # Parse functions
        #

        funcparse.run()


        #
        # Create header with forward declarations of all abstract classes
        #

        abs_frwd_decls_header_path = os.path.join(gb.boss_output_dir, gb.frwd_decls_abs_fname + cfg.header_extension)
        utils.constrAbsForwardDeclHeader(abs_frwd_decls_header_path)


        #
        # Create header with forward declarations of all wrapper classes
        #

        wrp_frwd_decls_header_path = os.path.join(gb.boss_output_dir, gb.frwd_decls_wrp_fname + cfg.header_extension)
        utils.constrWrpForwardDeclHeader(wrp_frwd_decls_header_path)


        # #
        # # Create header with declarations of all enum types
        # #

        # enum_decls_header_path = os.path.join(gb.boss_output_dir, gb.enum_decls_wrp_fname + cfg.header_extension)
        # utils.constrEnumDeclHeader(root.findall('Enumeration'), enum_decls_header_path)


    #
    # END: loop over xml files
    #

    # Check that we have done something...
    if (len(gb.classes_done) == 0) and (len(gb.functions_done) == 0):
        print()
        print()
        print('  - No classes or functions loaded!')
        print()
        print( utils.modifyText('Done!','bold'))
        print()

        sys.exit()

    #
    # Clear global dicts
    #

    utils.clearGlobalXMLdicts()



    #
    # Write new files
    #

    # Create all output directories that do not exist.
    filehandling.createOutputDirectories()

    # File writing loop
    for src_file_name, code_dict in gb.new_code.items():

        add_include_guard = code_dict['add_include_guard']
        code_tuples = code_dict['code_tuples']

        code_tuples.sort( key=lambda x : x[0], reverse=True )

        new_src_file_name  = os.path.join(gb.boss_output_dir, os.path.basename(src_file_name))

        if code_tuples == []:
            continue

        boss_backup_exists = False
        if os.path.isfile(src_file_name):
            try:
                f = open(src_file_name + '.backup.boss', 'r')
                boss_backup_exists = True
            except IOError as e:
                if e.errno != 2:
                    raise e
                f = open(src_file_name, 'r')

            f.seek(0)
            file_content = f.read()
            f.close()
            new_file_content = file_content
        else:
            new_file_content = ''

        if not boss_backup_exists and new_file_content:
            f = open(src_file_name + '.backup.boss', 'w')
            f.write(new_file_content)
            f.close()

        for pos,code in code_tuples:

            if pos == -1:
                new_file_content = new_file_content + code
            else:
                new_file_content = new_file_content[:pos] + code + new_file_content[pos:]

        # Add include guard where requested
        if add_include_guard:

            short_new_src_file_name = os.path.basename(new_src_file_name)

            try:
                prefix = code_dict['include_guard_prefix']
            except KeyError:
                prefix = ''

            new_file_content = utils.addIncludeGuard(new_file_content, short_new_src_file_name, prefix=prefix ,suffix=gb.gambit_backend_name_full)

        # Do the writing!
        f = open(new_src_file_name, 'w')
        f.write(new_file_content)
        f.close()



    #
    # Copy files from common_headers/ and common_source_files/ and replace any code template tags
    #

    filehandling.createCommonHeaders()
    filehandling.createCommonSourceFiles()


    #
    # Move files to correct directories
    #

    filehandling.moveFilesAround()


    #
    # Run through all the generated files and use the code tags __START_GAMBIT_NAMESPACE__ and __END_GAMBIT_NAMESPACE__ to construct
    # the correct namespace.
    #

    construct_namespace_in_files = glob.glob(os.path.join(gb.backend_types_dir_complete,'*')) + glob.glob(os.path.join(gb.for_gambit_backend_types_dir_complete,'*'))

    filehandling.replaceNamespaceTags(construct_namespace_in_files, gb.gambit_backend_namespace, '__START_GAMBIT_NAMESPACE__', '__END_GAMBIT_NAMESPACE__')


    #
    # Run through all the generated files and remove tags that are no longer needed
    #

    all_generated_files = glob.glob(os.path.join(gb.boss_output_dir,'*')) + glob.glob(os.path.join(gb.backend_types_dir_complete, '*')) + glob.glob(os.path.join(gb.for_gambit_backend_types_dir_complete,'*'))
    remove_tags_list = [ '__START_GAMBIT_NAMESPACE__',
                         '__END_GAMBIT_NAMESPACE__',
                         '__INSERT_CODE_HERE__' ]

    filehandling.removeCodeTagsFromFiles(all_generated_files, remove_tags_list)



    #
    # Copy files to the correct locations within the source tree of the original code
    #

    print()
    print()
    print( utils.modifyText('Copying generated files to original source tree:','underline'))
    print()

    manipulated_files, new_files, new_dirs = filehandling.copyFilesToSourceTree(verbose=True)

    # # Create the reset dir if it does not exist
    # filehandling.createOutputDirectories(selected_dirs=['reset'])

    # Save source_target_tuples to be able to undo the changes at a later time
    with open(reset_info_file_name, 'wb') as f:
        pickle.dump([manipulated_files, new_files, new_dirs], f)


    #
    # If generate_only_flag is True, save state and quit
    #

    if options.generate_only_flag:
        with open('savefile.boss', 'wb') as f:
            pickle.dump([gb.classes_done, gb.factory_info], f)
        print()
        print()
        print('Done with code generation. Program state saved.')
        print('To generate loaded_types.hpp, run: boss.py --types-header')
        print()
        sys.exit()



    #
    # Generate header file 'loaded_types.hpp'
    #

    print()
    print()
    print( utils.modifyText('Generating file loaded_types.hpp:','underline'))
    print()

    filehandling.createLoadedTypesHeader()


    #
    # Parse any source files for global functions using castxml
    #

    print()
    print()
    print( utils.modifyText('Parsing the generated function source files:','underline'))
    print()

    function_xml_files = filehandling.parseFunctionSourceFiles()


    #
    # Generate GAMBIT frontend header file ''
    #

    print()
    print()
    print( utils.modifyText('Generating GAMBIT frontend header file:','underline'))
    print()

    filehandling.createFrontendHeader(function_xml_files)


    #
    # Done!
    #

    print()
    print( utils.modifyText('BOSS done!','bold'))
    print()
    print()

    if not options.no_instructions_flag:
        print("  To prepare this backend for use with GAMBIT, do the following:")
        print()
        print("    1. BOSS has added new source files to '%s' and new header files to '%s'." % (cfg.src_files_to, cfg.header_files_to))
        print("       Make sure that these are included when building '%s'." % (cfg.gambit_backend_name))
        print("    2. Build a shared library (.so) from the '%s' source code that BOSS has edited." % (cfg.gambit_backend_name))
        print("    3. Set the correct path to this library in the 'backends_locations.yaml' file in GAMBIT.")
        print("    4. Copy the '%s' directory from '%s' to the 'backend_types' directory within GAMBIT." % (gb.gambit_backend_name_full, gb.for_gambit_backend_types_dir_complete))
        print("    5. Copy the file '%s' from '%s' to the GAMBIT 'frontends' directory." % (gb.frontend_fname, gb.frontend_path))
        print()
        print()

# ====== END: main ========

if  __name__ =='__main__':main()

