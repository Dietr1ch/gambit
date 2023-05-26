"""
PocoMC scanner
==============
"""

import pickle
import numpy as np

import pocomc

from .scanner import Scanner
from .copydoc import copydoc
from .version import version


class PocoMC(Scanner):

    name = "pocomc"
    version = version(pocomc)

    def initial_state(self):
        """
        :returns: Choice of initial state for chain
        """
        return np.vstack([self.prior_transform(np.random.rand(self.dim))
                         for i in range(self.n_particles)])

    @copydoc(pocomc.Sampler)
    def __init__(self, n_particles=1000, vectorize_likelihood=True, **kwargs):
        self.n_particles = n_particles
        self.sampler = pocomc.Sampler(
            self.n_particles,
            self.dim,
            self.loglike,
            self.log_prior_density,
            infer_vectorization=False,
            **kwargs)

    @copydoc(pocomc.Sampler.run)
    def run(self, pkl_name="pocomc.pkl", initial_state=None, **kwargs):
        if initial_state is None:
            initial_state = self.initial_state()
        self.sampler.run(initial_state, **kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(self.sampler.results, f)
