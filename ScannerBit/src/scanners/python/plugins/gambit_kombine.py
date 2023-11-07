"""
Kombine MCMC sampler
====================
"""

import pickle
import numpy as np
from utils import copydoc, version, get_directory, with_mpi
if with_mpi:
    from utils import MPIPool

try:
    import kombine
    kombine_version = version(kombine)
    kombine_Sampler = kombine.Sampler
    kombine_Sampler_run_mcmc = kombine.Sampler.run_mcmc
except:
    __error__="kombine pkg not installed"
    kombine_version = "n/a"
    kombine_Sampler = None
    kombine_Sampler_run_mcmc = None

import scanner_plugin as splug

class Kombine(splug.scanner):
    """
    MCMC sampler with KDE proposals
    """

    __version__ = kombine_version

    @copydoc(kombine_Sampler)
    def __init__(self, nwalkers=4, pkl_name="kombine.pkl", **kwargs):
        super().__init__(use_mpi=False)
        
        self.assign_aux_numbers("Posterior")
        if self.mpi_rank == 0:
            self.printer.new_stream("txt", synchronised=False)
            self.nwalkers = nwalkers
            if 'nwalkers' in self.init_args:
                self.nwalkers = self.init_args['nwalkers']
                del self.init_args['nwalkers']
            self.log_dir = get_directory("Kombine", **kwargs)
            self.pkl_name = pkl_name


    @classmethod
    def my_like(cls, params):
        lnew = cls.loglike_hypercube(params)
        
        return (lnew, (cls.mpi_rank, cls.pointid))

    def run_internal(self, nsteps=5000, p0=None, **kwargs):

        if self.mpi_size == 1:
            self.sampler = kombine.Sampler(self.nwalkers, 
                                           self.dim, 
                                           self.loglike_hypercube,
                                           #processes=1
                                           )
            if p0 is None:
                p0 = np.random.rand(self.nwalkers, self.dim)
            self.sampler.burnin(p0, **kwargs)
            res = self.sampler.run_mcmc(nsteps, **kwargs)
        else:
            with MPIPool() as pool:
                if pool.is_master():
                    self.sampler = kombine.Sampler(self.nwalkers, 
                                                   self.dim, 
                                                   self.loglike_hypercube, 
                                                   #processes=1,
                                                   pool=pool
                                                   )
                    if p0 is None:
                        p0 = np.random.rand(self.nwalkers, self.dim)
                    self.sampler.burnin(p0, **kwargs)
                    res = self.sampler.run_mcmc(nsteps, **kwargs)
                else:
                    pool.wait()
                
        if self.mpi_rank == 0:
            
            stream = self.printer.get_stream("txt")
            stream.reset()
            
            #for i in range(self.mpi_size):
                #us, cs = np.unique(blobs[blobs[:, 0]==i, 1], return_counts=True)
                
                #for u, c in zip(us, cs):
                    #stream.print(c, "Posterior", i, u)
            stream.flush()
            
            if self.pkl_name:
                with open(self.log_dir + self.pkl_name, "wb") as f:
                    pickle.dump(res, f)
    
    @copydoc(kombine_Sampler_run_mcmc)
    def run(self):
        self.run_internal(**self.run_args)
        

__plugins__ = {"kombine": Kombine}
