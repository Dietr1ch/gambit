"""
Zeus scanner
============
"""

import numpy as np

import zeus

from .scanner import Scanner
from .copydoc import copydoc
from .version import version


class Zeus(Scanner):

    name = "zeus"
    version = version(zeus)

    def save_callback(self, filename):
        """
        :returns: Callback that saves results
        """
        if not filename:
            return None
        return zeus.callbacks.SaveProgressCallback(filename)

    def initial_state(self):
        """
        :returns: Choice of initial state for chain
        """
        return np.vstack([self.prior_transform(np.random.rand(self.dim))
                         for i in range(self.nwalkers)])

    @copydoc(zeus.EnsembleSampler)
    def __init__(self, nwalkers=8, **kwargs):
        self.nwalkers = nwalkers
        self.sampler = zeus.EnsembleSampler(
            self.nwalkers, self.dim, self.log_target_density, **kwargs)

    @copydoc(zeus.EnsembleSampler.run_mcmc)
    def run(
            self,
            nsteps=100,
            initial_state=None,
            filename="zeus.h5",
            **kwargs):
        """
        There is one additional arguments:

        :param: filename (zeus.h5')

        for passing the name of a h5 file to which to save results using the zeus writer.
        """
        if initial_state is None:
            initial_state = self.initial_state()
        self.sampler.run_mcmc(initial_state, nsteps,
                              callbacks=self.save_callback(filename), **kwargs)