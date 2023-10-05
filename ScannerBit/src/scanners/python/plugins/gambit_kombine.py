"""
Kombine MCMC sampler
====================
"""

import pickle
import numpy as np
import kombine

import scanner_plugin as splug
from utils import copydoc, version


class Kombine(splug.scanner):
    """
    MCMC sampler with KDE proposals
    """

    name = "kombine"
    __version__ = version("kombine")

    def initial_state(self):
        """
        :returns: Choice of initial state for chain
        """
        return np.vstack([self.transform_to_vec(np.random.rand(self.dim))
                         for i in range(self.nwalkers)])

    @copydoc(kombine.Sampler)
    def __init__(self, nwalkers=4, **kwargs):
        super().__init__(use_mpi=False)
        self.nwalkers = nwalkers
        self.sampler = kombine.Sampler(self.nwalkers, self.dim, self.log_target_density)

    def run_internal(self, nsteps=5000, pkl_name="kombine.pkl", initial_state=None, **kwargs):
        """
        There are two additional arguments:

        :param: pkl_name ('kombine.pkl')
        :param: initial_state (None)

        for passing the name of a pkl file to which to save results and the initial state.
        """
        if initial_state is None:
            initial_state = self.initial_state()
        self.sampler.burn_in(initial_state)
        res = self.sampler.run_mcmc(nsteps, **kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(res, f)
    
    @copydoc(kombine.Sampler.run_mcmc)
    def run(self):
        self.run_internal(**self.run_args)
        

__plugins__ = {Kombine.name: Kombine}
