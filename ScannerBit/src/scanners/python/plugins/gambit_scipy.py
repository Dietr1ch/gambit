"""
Scipy dual annealing scannner
=============================
"""

import scipy.optimize

import scanner_plugin as splug
from utils import copydoc, version


class DualAnnealing(splug.scanner):
    """
    Dual annealing optimizer from scipy.
    """

    name = "scipy_dual_annealing"
    __version__ = version("scipy")
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False)

    @copydoc(scipy.optimize.dual_annealing)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.dual_annealing(self.loglike_hypercube, bounds, **self.run_args)
        print(res)
        

class BasinHopping(splug.scanner):
    """
    Basin-hopping optimizer from scipy.
    """

    name = "scipy_basin_hopping"
    __version__ = version("scipy")
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False)

    @copydoc(scipy.optimize.basinhopping)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.basinhopping(
            self.loglike_hypercube, bounds, **self.run_args)
        print(res)


class DifferentialEvolution(splug.scanner):
    """
    Differential evolution optimizer from scipy.
    """

    name = "scipy_differential_evolution"
    __version__ = version("scipy")
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False)

    @copydoc(scipy.optimize.differential_evolution)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.differential_evolution(
            self.loglike_hypercube, bounds, **self.run_args)
        print(res)
        

class Direct(splug.scanner):
    """
    The DIRECT optimizer from scipy.
    """

    name = "scipy_direct"
    __version__ = version("scipy")
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False)

    @copydoc(scipy.optimize.direct)
    def run(self):
        print("DEBUG: kwargs: ", kwargs)
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.direct(self.loglike_hypercube, bounds, **self.run_args)
        print(res)
        

__plugins__ = {DualAnnealing.name: DualAnnealing,
               BasinHopping.name: BasinHopping,
               DifferentialEvolution.name: DifferentialEvolution,
               Direct.name: Direct}
