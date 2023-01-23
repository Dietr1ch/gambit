import objective_pyplugin as oplug
import math

class objective_plugin:
    
    def __init__(self):
        
        # get_keys() returns std::vector<std::string> of keys
        dim = len(oplug.get_keys())

        if dim != 2:
            throw: "EggBox: Need to have two parameters."

        # gets inifile value for "length" and return a list of floats
        self.length = oplug.get_inifile_value("length", dtype="list", default=[5.0, 5.0])
        
    def plugin_main(self, map):
        
        # prints paramters
        oplug.print_parameters(map)
        
        params = (map[oplug.get_keys()[0]]*self.length[0],  map[oplug.get_keys()[1]]*self.length[1])
        
        return 5.0*math.log(2.0 + math.cos(params[0]*math.pi/2.0)*math.cos(params[1]*math.pi/2.0))
