"""
Emcee scanner
=============
"""

import numpy as np
import emcee

import scanner_plugin as splug
from utils import copydoc, version


class Emcee(splug.scanner):

    name = "emcee"
    __version__ = version(emcee)

    def backend(self, filename, reset):
        """
        :returns: Backend with h5 file to save results
        """
        if not filename:
            return None

        backend = emcee.backends.HDFBackend(filename)
        if reset:
            backend.reset(self.nwalkers, self.dim)

        return backend

    def initial_state(self):
        """
        :returns: Choice of initial state for chain
        """
        return np.vstack([self.prior_transform(np.random.rand(self.dim))
                         for i in range(self.nwalkers)])

    @copydoc(emcee.EnsembleSampler)
    def __init__(self, nwalkers=4, filename="emcee.h5", reset=False, **kwargs):
        """
        There are two additional arguments:

        :param: filename ('emcee.h5')
        :param: reset (False)

        for passing the name of a h5 file to which to save results using the emcee writer,
        and whether to reset that file.
        """
        super().__init__()
        self.nwalkers = nwalkers
        if 'nwalkers' in self.init_args:
            self.nwalkers = self.init_args['nwalkers']
            del self.init_args['nwalkers']
        self.sampler = emcee.EnsembleSampler(
            self.nwalkers,
            self.dim,
            self.log_target_density,
            backend=self.backend(
                filename,
                reset),
            **self.init_args)

    def run_internal(self, nsteps=5000, progress=True, initial_state=None, **kwargs):
        if initial_state is None:
            initial_state = self.initial_state()
        self.sampler.run_mcmc(initial_state, nsteps,
                              progress=progress, **kwargs)
        
    @copydoc(emcee.EnsembleSampler.run_mcmc)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {Emcee.name: Emcee}
