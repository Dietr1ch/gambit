import scanner_plugin as sp
import numpy as np

vecs = []

# the plugin constructor and deconstructor are optional
def plugin_constructor():
    global dim
    global vecs
    global like
    
    # gets dimension of hyper cube.
    dim = sp.get_dimension()
    
    # gets inifile value corresponding to "like" key
    purpose = sp.get_inifile_value("like", dtype=str)
    
    # gets likelihood corresponding to the purpose "purpose"
    like = sp.get_purpose(purpose)
    
    # get grids pt number from inifile. Return a list of ints
    N = sp.get_inifile_value("grid_pts", dtype=list, etype=int)
    
    if len(N) != dim:
        raise Exception("Grid Scanner: The dimension of gambit ({0}) does not match the dimension of the inputed grid_pts ({1}).".format(dim, len(N)))
    
    # gets "parameters" infile value with a default of []. Returns a list of strings
    user_params = sp.get_inifile_value("parameters", dtype=list, etype=str, default=list());

    if len(user_params) > 0:
        
        # get the parameters names from the prior
        param_names = sp.get_prior().getShownParameters()
        
        if len(param_names) != len(user_params):
            raise Exception("Grid Scanner: The dimension of gambit ({0}) does not match the dimension of the inputed parameters ({1}).".format(len(param_names), len(user_params)))
        
        for param in param_names:
            if param in user_params:
                vecs.append(np.linspace(0.0, 1.0, N[user_params.index(param)]))
            else:
                raise Exception("Grid Scanner: parameter \"{0}\" is not provided.".format(param))
            
    else:
        for n in N:
            vecs.append(np.linspace(0.0, 1.0, n))

def plugin_main():
    
    rank = 0
    
    for pt in np.vstack(np.meshgrid(*vecs)).reshape(dim, -1).T:
        # run likelihood
        like(pt)
        
        # get pt id
        id = like.getPtID()
        
        # prints value
        sp.get_printer().get_stream().print(1.0, "mult", rank, id)
    
    return 0
