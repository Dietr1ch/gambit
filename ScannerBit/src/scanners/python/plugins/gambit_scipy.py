"""
Scipy dual annealing scannner
=============================
"""

import scipy.optimize

from .scanner import Scanner
from .copydoc import copydoc
from .version import version


class DualAnnealing(Scanner):
    """
    Dual annealing optimizer from scipy.
    """

    name = "scipy_dual_annealing"
    version = version("scipy")

    def __init__(self, **kwargs):
        """
        No initialisation required.
        """
        pass

    @copydoc(scipy.optimize.dual_annealing)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.dual_annealing(self.loglike, bounds, **kwargs)


class BasinHopping(Scanner):
    """
    Basin-hopping optimizer from scipy.
    """

    name = "scipy_basin_hopping"
    version = version("scipy")

    def __init__(self, **kwargs):
        """
        No initialisation required.
        """
        pass

    @copydoc(scipy.optimize.basinhopping)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.basinhopping(
            self.loglike_hypercube, bounds, **kwargs)


class DifferentialEvolution(Scanner):
    """
    Differential evolution optimizer from scipy.
    """

    name = "scipy_differential_evolution"
    version = version("scipy")

    def __init__(self, **kwargs):
        """
        No initialisation required.
        """
        pass

    @copydoc(scipy.optimize.differential_evolution)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.differential_evolution(
            self.loglike_hypercube, bounds, **kwargs)


class Direct(Scanner):
    """
    The DIRECT optimizer from scipy.
    """

    name = "scipy_direct"
    version = version("scipy")

    def __init__(self, **kwargs):
        """
        No initialisation required.
        """
        pass

    @copydoc(scipy.optimize.direct)
    def run(self, **kwargs):
        print("DEBUG: kwargs: ", kwargs)
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.direct(self.loglike_hypercube, bounds, **kwargs)
