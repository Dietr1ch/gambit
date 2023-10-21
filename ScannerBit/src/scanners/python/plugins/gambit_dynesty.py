"""
Dynesty scanners
================

The static and dynamic Dynesty classes are implemented separately.

We don't use the Dynesty checkpointing/saving functionality, as it
attempts to pickle the loglikelihood function etc.
"""

import pickle
import dynesty
import numpy as np

from utils import MPIPool
import scanner_plugin as splug
from utils import copydoc, version
from mpi4py import MPI

class StaticDynesty(splug.scanner):
    """
    Dynesty nested sampler with static number of live points.

    We add an additional run parameter:

    :param: pkl_name ('static_dynesty.pkl')
    """

    name = "static_dynesty"
    __version__ = version(dynesty)

    @copydoc(dynesty.NestedSampler)
    def __init__(self, **kwargs):
        super().__init__()
        
        if self.mpi_rank == 0:
            self.assign_aux_numbers("Posterior")
            self.printer.new_stream("txt", synchronised=False)
        
    def gambit_loglike(self, params):
        lnew = self.loglike_physical(params)
        
        return (lnew, np.array([self.mpi_rank, self.point_id]))

    def run_internal(self, pkl_name="static_dynesty.pkl", **kwargs):
        if self.mpi_size == 1:
            self.sampler = dynesty.DynamicNestedSampler(
                self.gambit_loglike, self.transform_to_vec, self.dim, blob=True, **self.init_args)
            
            self.sampler.run_nested(**kwargs)
        
        else:
            with MPIPool() as pool:
                
                if pool.is_master():
                    self.sampler = dynesty.DynamicNestedSampler(
                        self.gambit_loglike, self.transform_to_vec, self.dim, blob=True, pool=pool, **self.init_args)
                    
                    self.sampler.run_nested(**kwargs)
                else:
                    pool.wait()

        if self.mpi_rank == 0:

            wts = self.sampler.results["logwt"]
            blobs = self.sampler.results["blob"]
            
            stream = self.printer.get_stream("txt")
            stream.reset()
            
            for wt, blob in zip(wts, blobs):
                steam.print(np.exp(wt), "Posterior", blob[0], blob[1])
                
            stream.flush()
            
            with open(pkl_name, "wb") as f:
                pickle.dump(self.sampler.results, f)
            
    @copydoc(dynesty.NestedSampler.run_nested)
    def run(self):
        self.run_internal(**self.run_args)

class DynamicDynesty(splug.scanner):
    """
    Dynesty nested sampler with dynamic number of live points.

    We add an additional run parameter:

    :param: pkl_name ('dynamic_dynesty.pkl')
    """

    name = "dynamic_dynesty"
    __version__ = version(dynesty)

    @copydoc(dynesty.DynamicNestedSampler)
    def __init__(self, **kwargs):
        super().__init__()
        
        if self.mpi_rank == 0:
            self.assign_aux_numbers("Posterior")
            self.printer.new_stream("txt", synchronised=False)
        
    def __reduce__(self):
        return (self.__class__, ())
        
    def gambit_loglike(self, params):
        lnew = self.loglike_physical(params)
        
        return (lnew, np.array([self.mpi_rank, self.point_id]))

    def run_internal(self, pkl_name=None, **kwargs):
        if self.mpi_size == 1:
            self.sampler = dynesty.DynamicNestedSampler(
                self.gambit_loglike, self.transform_to_vec, self.dim, blob=True, **self.init_args)
            
            self.sampler.run_nested(**kwargs)
        
        else:
            with MPIPool() as pool:
                
                if pool.is_master():
                    self.sampler = dynesty.DynamicNestedSampler(
                        self.gambit_loglike, self.transform_to_vec, self.dim, blob=True, pool=pool, **self.init_args)
                    
                    self.sampler.run_nested(**kwargs)
                else:
                    pool.wait()

        if self.mpi_rank == 0:

            wts = self.sampler.results["logwt"]
            blobs = self.sampler.results["blob"]
            
            stream = self.printer.get_stream("txt")
            stream.reset()
            
            for wt, blob in zip(wts, blobs):
                stream.print(np.exp(wt), "Posterior", blob[0], blob[1])
                
            stream.flush()
            
            if not pkl_name is None:
                with open(pkl_name, "wb") as f:
                    pickle.dump(self.sampler.results, f)
     
    @copydoc(dynesty.DynamicNestedSampler.run_nested)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {StaticDynesty.name: StaticDynesty,
               DynamicDynesty.name: DynamicDynesty}
