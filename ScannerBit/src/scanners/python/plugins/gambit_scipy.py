"""
Scipy dual annealing scannner
=============================
"""

import scipy.optimize

from utils import Scanner
from utils import copydoc
from utils import version


class DualAnnealing(Scanner):

    name = "scipy_dual_annealing"
    __version__ = version("scipy")

    @copydoc(scipy.optimize.dual_annealing)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.dual_annealing(self.loglike, bounds, **kwargs)
        self.log(res)


class BasinHopping(Scanner):

    name = "scipy_basin_hopping"
    __version__ = version("scipy")

    @copydoc(scipy.optimize.basinhopping)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.basinhopping(
            self.loglike_hypercube, bounds, **kwargs)
        self.log(res)


class DifferentialEvolution(Scanner):

    name = "scipy_differential_evolution"
    __version__ = version("scipy")

    @copydoc(scipy.optimize.differential_evolution)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.differential_evolution(
            self.loglike_hypercube, bounds, **kwargs)
        self.log(res)


class Direct(Scanner):

    name = "scipy_direct"
    __version__ = version("scipy")

    @copydoc(scipy.optimize.direct)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = scipy.optimize.direct(self.loglike_hypercube, bounds, **kwargs)
        self.log(res)


__plugins__={DualAnnealing.name: DualAnnealing,
             BasinHopping.name: BasinHopping,
             DifferentialEvolution.name: DifferentialEvolution,
             Direct.name: Direct}
