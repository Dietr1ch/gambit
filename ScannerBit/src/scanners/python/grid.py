import scanner_plugin as splug
import numpy as np

class scanner_plugin:
    
    def __init__(self):
        
        # gets dimension of hyper cube.
        self.dim = splug.get_dimension()
        
        # gets inifile value corresponding to "like" key
        purpose = splug.get_inifile_value("like", dtype=str)
        
        # gets likelihood corresponding to the purpose "purpose"
        self.like = splug.get_purpose(purpose)
        
        # get grids pt number from inifile. Return a list of ints
        N = splug.get_inifile_value("grid_pts", dtype=list, etype=int)
        
        if len(N) != self.dim:
            raise Exception("Grid Scanner: The dimension of gambit ({0}) does not match the dimension of the inputed grid_pts ({1}).".format(self.dim, len(N)))
        
        # gets "parameters" infile value with a default of []. Returns a list of strings
        user_params = splug.get_inifile_value("parameters", dtype=list, etype=str, default=list());
        
        self.vecs=[]

        if len(user_params) > 0:
            
            # get the parameters names from the prior
            param_names = splug.get_prior().getShownParameters()
            
            if len(param_names) != len(user_params):
                raise Exception("Grid Scanner: The dimension of gambit ({0}) does not match the dimension of the inputed parameters ({1}).".format(len(param_names), len(user_params)))
            
            for param in param_names:
                if param in user_params:
                    self.vecs.append(np.linspace(0.0, 1.0, N[user_params.index(param)]))
                else:
                    raise Exception("Grid Scanner: parameter \"{0}\" is not provided.".format(param))
                
        else:
            for n in N:
                self.vecs.append(np.linspace(0.0, 1.0, n))
            
    def plugin_main(self):
        
        rank = 0

        for pt in np.vstack(np.meshgrid(*self.vecs)).reshape(self.dim, -1).T:
            # run likelihood
            self.like(pt)
            
            # get pt id
            id = self.like.getPtID()
            
            # prints value
            splug.get_printer().get_stream().print(1.0, "mult", rank, id)
        
        return 0
    
    
