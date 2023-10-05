"""
Stochopy optimization
=====================
"""

import stochopy

import scanner_plugin as splug
from utils import copydoc, version


class StochopyMinimize(splug.scanner):
    """
    Stochastic minimization algorithms - DE, PSO etc.
    """

    name = "stochopy_minimize"
    __version__ = version("stochopy")

    def __init__(self, **kwargs):
        super().__init__(use_mpi=False) # False for right now.

    @copydoc(stochopy.optimize.minimize)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = stochopy.optimize.minimize(
            self.loglike_hypercube, bounds, **self.run_args)
        print(res)


class StochopySample(splug.scanner):
    """
    Sampling algorithms from stochopy.
    """

    name = "stochopy_sample"
    __version__ = version("stochopy")

    def __init__(self, **kwargs):
        super().__init__(use_mpi=False) # False for right now.

    @copydoc(stochopy.sample)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = stochopy.sample(self.loglike, bounds, **self.run_args)
        print(res)


__plugins__ = {StochopyMinimize.name: StochopyMinimize,
               StochopySample.name: StochopySample}
