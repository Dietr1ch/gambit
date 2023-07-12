"""
Stochopy optimization
=====================
"""

import numpy as np
import stochopy

import scanner_plugin as splug
from utils import copydoc, version


class StochopyMinimize(splug.scanner):
    """
    Stochastic minimization algorithms - DE, PSO etc.
    """

    name = "stochopy_minimize"
    __version__ = version("stochopy")

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

    @copydoc(stochopy.optimize.sample)
    def run(self):
        bounds = [(0., 1.)] * self.dim
        res = stochopy.optimize.sample(self.loglike, bounds, **self.run_args)
        print(res)


__plugins__ = {StochopyMinimize.name: StochopyMinimize
               StochopySample.name: StochopySample}
