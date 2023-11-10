"""
Scipy dual annealing scannner
=============================
"""

from utils import copydoc, version

try:
    import scipy.optimize
    scipy_version = version("scipy")
    scipy_optimize_dual_annealing = scipy.optimize.dual_annealing
    scipy_optimize_basinhopping = scipy.optimize.basinhopping
    scipy_optimize_differential_evolution = scipy.optimize.differential_evolution
    scipy_optimize_direct = scipy.optimize.direct
except:
    __error__ = 'scipy.optimize pkg not installed'
    scipy_version = 'n/a'
    scipy_optimize_dual_annealing = None
    scipy_optimize_basinhopping = None
    scipy_optimize_differential_evolution = None
    scipy_optimize_direct = None

import scanner_plugin as splug


class DualAnnealing(splug.scanner):
    """
Dual annealing optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_dual_annealing)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.dual_annealing(self.loglike_hypercube, bounds, **self.run_args)
        print(res)
        return 0
        

class BasinHopping(splug.scanner):
    """
Basin-hopping optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_basinhopping)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.basinhopping(
            self.loglike_hypercube, bounds, **self.run_args)
        print(res)
        return 0


class DifferentialEvolution(splug.scanner):
    """
Differential evolution optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_differential_evolution)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.differential_evolution(
            self.loglike_hypercube, bounds, **self.run_args)
        print(res)
        return 0
        

class Direct(splug.scanner):
    """
The DIRECT optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_direct)
    def run(self):
        print("DEBUG: kwargs: ", kwargs)
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.direct(self.loglike_hypercube, bounds, **self.run_args)
        print(res)
        return 0
        

__plugins__ = {"scipy_dual_annealing": DualAnnealing,
               "scipy_basin_hopping": BasinHopping,
               "scipy_differential_evolution": DifferentialEvolution,
               "scipy_direct": Direct}
