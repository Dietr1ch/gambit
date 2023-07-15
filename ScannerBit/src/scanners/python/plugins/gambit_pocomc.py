"""
PocoMC scanner
==============
"""

import pickle
import numpy as np
import pocomc

import scanner_plugin as splug
from utils import copydoc, version


class PocoMC(splug.scanner):

    name = "pocomc"
    __version__ = version(pocomc)

    def initial_state(self):
        """
        :returns: Choice of initial state for chain
        """
        return np.vstack([self.prior_transform(np.random.rand(self.dim))
                         for i in range(self.n_particles)])

    @copydoc(pocomc.Sampler)
    def __init__(self, n_particles=1000, vectorize_likelihood=True, **kwargs):
        super().__init__()
        self.n_particles = n_particles
        self.sampler = pocomc.Sampler(
            self.n_particles,
            self.dim,
            self.loglike,
            self.log_prior_density,
            infer_vectorization=False,
            **kwargs)

    def run_internal(self, pkl_name="pocomc.pkl", initial_state=None, **kwargs):
        if initial_state is None:
            initial_state = self.initial_state()
        self.sampler.run(initial_state, **kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(self.sampler.results, f)

    @copydoc(pocomc.Sampler.run)
    def run():
        self.run_internal(**self.run_args)


__plugins__ = PocoMC.name: PocoMC}
