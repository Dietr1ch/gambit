import scanner_plugin as sb
import numpy as np

vecs = []
dim = 0

# the plugin constructor and deconstructor are optional
def plugin_constructor():
    global dim
    global vecs
    global like
    
    # gets dimension of hyper cube.
    dim = sb.get_dimension()
    
    # gets inifile value corresponding to "like" key
    purpose = sb.get_inifile_value("like", dtype=str)
    
    # gets likelihood corresponding to the purpose "purpose". 
    like = sb.get_purpose(purpose)
    
    # get grids pt number from inifile
    N = sb.get_inifile_value("grid_pts", dtype=list)
    
    if len(N) != dim:
        raise Exception("Grid Scanner:  The dimension of gambit ({0}) does not match the dimension of the inputed grid_pts ({1})".format(dim, len(N)))
    
    # param_names = sb.get_prior().getShownParameters()
    
    for n in N:
        vecs.append(np.linspace(0.0, 1.0, int(n)))

def plugin_main():
    
    rank = 0
    
    for pt in np.vstack(np.meshgrid(*vecs)).reshape(dim, -1).T:
        # run likelihood
        like(np.array(pt))
        
        # get pt id
        id = like.getPtID()
        
        # prints value
        sb.get_printer().get_stream().print(1.0, "mult", rank, id)
    
    return 0
