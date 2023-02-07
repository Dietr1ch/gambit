import scannerbit
import scanner_pyplugin as splug
import numpy as np
from scipy.optimize import dual_annealing

# This scanner is not MPI parallelised. 
if scannerbit.with_mpi:
    if scannerbit.numtasks() > 1:
        raise Exception(f"This scanner ({__file__}) is not MPI parallelised. Either run GAMBIT "
                        f"with a single MPI process or use a different scanner.")        

class scanner_plugin:
    
    def __init__(self):
        
        # Get dimension of the hyper cube.
        self.dim = splug.get_dimension()

        # Get the inifile value corresponding to the "like" key.
        # This is the purpose name (typically "LogLike") that GAMBIT uses 
        # to identify the functions that should contribute to the total loglike function.
        purpose = splug.get_inifile_value("like", dtype=str)
        
        # Get the likelihood function corresponding to the purpose "purpose".
        self.like = splug.get_purpose(purpose)
        
        # Gets inifile options that will be passed on to scipy's dual_annealing.
        self.maxiter = splug.get_inifile_value("maxiter", dtype=int, default=1000)
        self.initial_temp = splug.get_inifile_value("initial_temp", dtype=float, default=5230.0)
        self.restart_temp_ratio = splug.get_inifile_value("restart_temp_ratio", dtype=float, default=2e-05)
        self.visit = splug.get_inifile_value("visit", dtype=float, default=2.62)
        self.accept = splug.get_inifile_value("accept", dtype=float, default=-5.0)
        self.maxfun = splug.get_inifile_value("maxfun", dtype=float, default=10000000.0)
        self.no_local_search = splug.get_inifile_value("no_local_search", dtype=bool, default=False)

    def plugin_main(self):
        
        rank = 0

        # The scanner always works with the unit hyper cube.
        bounds = [(0, 1)] * self.dim

        # The objective function that the scipy optimizer will call
        def objective_function(x):

            # Run likelihood
            loglike = self.like(x)

            # # DEBUG: send some info to the GAMBIT printer 
            # # get point id
            # id = self.like.getPtID()
            # # print something
            # splug.get_printer().get_stream().print(loglike, "loglike_in_scanner_plugin", rank, id)

            # Return result
            return loglike

        # Run scipy.optimize.dual_annealing
        dual_annealing(objective_function, bounds, args=(), maxiter=self.maxiter,
                       minimizer_kwargs=None, initial_temp=self.initial_temp, 
                       restart_temp_ratio=self.restart_temp_ratio, visit=self.visit, 
                       accept=self.accept, maxfun=self.maxfun, 
                       no_local_search=self.no_local_search, x0=None)

        return 0
    