import objective_pyplugin as oplug
import math

class objective_plugin:
    """
A python example of a simple two dimensional EggBox likelihood of the form:

L ~ (2.0 + std::cos(x*length[0]*pi/2)*std::cos(y*length[1]*pi/2))^5

Inifile option:
    length[10, 10]: A two-vector representing the scale length (default [10, 10]).
"""

    __version__="1.0.0"
    
    def __init__(self, **options):
        
        # get_keys() returns std::vector<std::string> of keys
        dim = len(oplug.get_keys())

        if dim != 2:
            throw: "EggBox: Need to have two parameters."
        
        if "length" in options:
            if type(options["length"]) is list:
                self.length = options["length"]
                if len(self.length) != 2:
                    throw: "EggBox: input length list needs to have 2 values."
            else:
                self.length = list()
                self.length.append(options["length"])
                self.length.append(options["length"])
        else:
            self.length = [10.0, 10.0]
        
    def plugin_main(self, map):
        
        # prints paramters
        oplug.print_parameters(map)

        params = (map[oplug.get_keys()[0]]*self.length[0],  map[oplug.get_keys()[1]]*self.length[1])
        
        return 5.0*math.log(2.0 + math.cos(params[0]*math.pi/2.0)*math.cos(params[1]*math.pi/2.0))
