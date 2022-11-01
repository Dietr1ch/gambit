import objective_plugin as oscan
import math

length = 10.0
    
def plugin_constructor():
    global length
    dim = len(oscan.get_keys())

    if dim != 2:
        throw: "EggBox: Need to have two parameters."

    length = oscan.get_inifile_value("length", dtype="list", default=[5.0, 5.0])
    
def plugin_main(map):
    oscan.print_parameters(map)
    params = (map[oscan.get_keys()[0]]*length[0],  map[oscan.get_keys()[1]]*length[1])
    
    return 5.0*math.log(2.0 + math.cos(params[0]*math.pi/2.0)*math.cos(params[1]*math.pi/2.0))
