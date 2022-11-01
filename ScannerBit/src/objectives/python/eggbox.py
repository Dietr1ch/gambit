import objective_plugin as oplug
import math

# the plugin constructor and deconstructor are optional
def plugin_constructor():
    
    global length
    
    # get_keys() returns std::vector<std::string> of keys
    dim = len(oplug.get_keys())

    if dim != 2:
        throw: "EggBox: Need to have two parameters."

    # gets inifile value for "length" and return a list of floats
    length = oplug.get_inifile_value("length", dtype="list", default=[5.0, 5.0])
    
def plugin_main(map):
    
    # prints paramters
    oplug.print_parameters(map)
    
    params = (map[oplug.get_keys()[0]]*length[0],  map[oplug.get_keys()[1]]*length[1])
    
    return 5.0*math.log(2.0 + math.cos(params[0]*math.pi/2.0)*math.cos(params[1]*math.pi/2.0))
