"""
Stochopy optimization
=====================
"""

import numpy as np

import stochopy

from utils import Scanner, copydoc, version


class StochopyMinimize(Scanner):
    """
    Stochastic minimization algorithms - DE, PSO etc.
    """

    name = "stochopy_minimize"
    __version__ = version("stochopy")

    @copydoc(stochopy.optimize.minimize)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = stochopy.optimize.minimize(
            self.loglike_hypercube, bounds, **kwargs)
        print(res)


class StochopySample(Scanner):
    """
    Sampling algorithms from stochopy.
    """

    name = "stochopy_sample"
    __version__ = version("stochopy")

    @copydoc(stochopy.optimize.sample)
    def run(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        res = stochopy.optimize.sample(self.loglike, bounds, **kwargs)
        print(res)


__plugins__ = {StochopyMinimize.name: StochopyMinimize
               StochopySample.name: StochopySample}
