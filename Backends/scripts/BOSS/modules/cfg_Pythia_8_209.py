###################################
#                                 #
#  Configuration module for BOSS  #
#                                 #
###################################


# ~~~~~ GCCXML options ~~~~~

gccxml_path     = '/usr/bin'  # If the gccxml executable is globally available, set gccxml_path = ''
gccxml_compiler = 'g++'       # g++, gcc, icpc, icc, ... (use name of executable)


# ~~~~~ GAMBIT-specific options ~~~~~

gambit_backend_name    = 'Pythia'
gambit_backend_version = '8.209'
gambit_base_namespace  = ''


# ~~~~~ Information about the external code ~~~~~

# Use either absolute paths or paths relative to the main BOSS directory.
include_paths = ['../../../Backends/installed/Pythia/8.209/include', '../../../contrib/slhaea/include']
source_path   = '../../../Backends/installed/Pythia/8.209/src'

accepted_paths = ['../../../Backends/installed/Pythia/8.209/']


loaded_classes = [
                    'Pythia8::AlphaEM',
                    'Pythia8::AlphaStrong',
                    'Pythia8::BeamParticle',
                    'Pythia8::Couplings',
                    'Pythia8::CoupSM',
                    'Pythia8::CoupSUSY',
                    'Pythia8::DecayChannel',
                    'Pythia8::Event',
                    'Pythia8::Hist',
                    'Pythia8::Info',
                    'Pythia8::LHdecayChannel',
                    'Pythia8::LHdecayTable',
                    'Pythia8::Parm',
                    'Pythia8::ParticleDataEntry',
                    'Pythia8::ParticleData',
                    'Pythia8::ParticleDecays',
                    'Pythia8::Particle',
                    'Pythia8::PartonLevel',
                    'Pythia8::Pythia',
                    'Pythia8::ResonanceDecays',
                    'Pythia8::ResonanceGmZ',
                    'Pythia8::ResonanceWidths',
                    'Pythia8::Rndm',
                    'Pythia8::Settings',
                    'Pythia8::SigmaProcess',
                    'Pythia8::SigmaTotal',
                    'Pythia8::SLHAinterface',
                    'Pythia8::SlowJet',
                    'Pythia8::SusyLesHouches',
                    'Pythia8::UserHooks',
                    'Pythia8::Vec4'
                 ]

loaded_functions = [
                      # 'Pythia8::m2(Pythia8::Wave4, Pythia8::Wave4)',
                      # 'Pythia8::m2(Pythia8::Particle const&, Pythia8::Particle const&)',
                   ]

ditch = [
          'Pythia8::Pythia::initSLHA',
        ]


load_parent_classes    = False
wrap_inherited_members = False


header_extension = '.h'
source_extension = '.cc'

indent = 4

extra_output_dir       = 'BOSS_output'
abstr_header_prefix    = 'abstract_'
wrapper_header_prefix  = 'wrapper_'
factory_file_prefix    = 'factory_'
function_files_prefix  = 'function_'


# ~~~~~ Information about other known types ~~~~~

std_include_paths  = ['/usr/include/']

known_classes = [
                   'SLHAea::Coll',
                ]

known_class_headers = {
    "SLHAea::Coll"           : "SLHAea/slhaea.h",
    "std::array"             : "<array>", 
    "std::vector"            : "<vector>", 
    "std::deque"             : "<deque>", 
    "std::complex"           : "<complex>", 
    "std::list"              : "<list>", 
    "std::forward_list"      : "<forward_list>", 
    "std::set"               : "<set>",  
    "std::multiset"          : "<set>", 
    "std::map"               : "<map>", 
    "std::multimap"          : "<map>", 
    "std::unordered_set"     : "<unordered_set>", 
    "std::unordered_multiset": "<unordered_set>", 
    "std::unordered_map"     : "<unordered_map>", 
    "std::unordered_multimap": "<unordered_map>", 
    "std::stack"             : "<stack>", 
    "std::queue"             : "<queue>",
    "std::priority_queue"    : "<queue>",
    "std::string"            : "<string>",
    "std::istream"           : "<istream>",
    "std::ostream"           : "<ostream>",
    "std::iostream"          : "<iostream>"
}

