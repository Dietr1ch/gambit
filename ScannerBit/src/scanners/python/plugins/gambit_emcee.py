"""
Emcee scanner
=============
"""

import numpy as np
import emcee

import scanner_plugin as splug
from utils import copydoc, version
from utils import MPIPool

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
        #return np.vstack([self.transform_to_vec(np.random.rand(self.dim))
         #                for i in range(self.nwalkers)])
        return np.vstack([np.random.rand(self.dim)
                         for i in range(self.nwalkers)])

    def my_like(self, params):
        if (params < self.hi).all() and (params > self.low).all():
            lnew = self.loglike_hypercube(params)
            
            return (lnew, self.mpi_rank, self.point_id)
        else:
            return  (-np.inf, -1, -1)

    @staticmethod
    def make_emcee(kwargs):
        return Emcee(**kwargs)
    
    def __reduce__(self):
        return (Emcee.make_emcee, (self.args,))

    @copydoc(emcee.EnsembleSampler)
    def __init__(self, nwalkers=1, filename="emcee.h5", reset=False, **kwargs):
        """
        There are two additional arguments:
        
        :param: filename ('emcee.h5')
        :param: reset (False)

        for passing the name of a h5 file to which to save results using the emcee writer,
        and whether to reset that file.
        """
        super().__init__() # False for right now.
        self.hi = np.array([1.0]*self.dim)
        self.low = np.array([0.0]*self.dim)
        self.nwalkers = nwalkers
        if 'nwalkers' in self.init_args:
            self.nwalkers = self.init_args['nwalkers']
            del self.init_args['nwalkers']

    def run_internal(self, nsteps=5000, progress=True, initial_state=None, **kwargs):
        if self.mpi_size == 1:
            if initial_state is None:
                initial_state = self.initial_state()
                
            self.sampler = emcee.EnsembleSampler(
            self.nwalkers,
            self.dim,
            self.my_like,
            **self.init_args)
            
            self.sampler.run_mcmc(initial_state, nsteps,
                                progress=progress, **kwargs)
        else:
            with MPIPool() as pool:
                if pool.is_master():
                    if initial_state is None:
                        initial_state = self.initial_state()
                        
                    self.sampler = emcee.EnsembleSampler(
                    self.nwalkers,
                    self.dim,
                    self.my_like,
                    pool=pool,
                    **self.init_args)
                    
                    self.sampler.run_mcmc(initial_state, nsteps,
                                        progress=progress, **kwargs)
                else:
                    pool.wait()
        
        if self.mpi_rank == 0:
            blobs = np.array(self.sampler.get_blobs(flat=True))
            for i in range(self.mpi_size):
                us, cs = np.unique(blobs[blobs[:, 0]==i, 1], return_counts=True)
                print(i, us, cs)
                for u, c in zip(us, cs):
                    self.print(c, "mult", i, u)

        
    @copydoc(emcee.EnsembleSampler.run_mcmc)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {Emcee.name: Emcee}
