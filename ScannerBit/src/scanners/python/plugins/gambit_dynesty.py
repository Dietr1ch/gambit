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
from utils import copydoc, version, get_filename
from mpi4py import MPI

class StaticDynesty(splug.scanner):
    """
    Dynesty nested sampler with static number of live points.

    We add an additional run parameter:

    :param: pkl_name ('static_dynesty.pkl')
    """

    __version__ = version(dynesty)

    @copydoc(dynesty.NestedSampler)
    def __init__(self, filename='dynesty.save', **kwargs):
        super().__init__(use_mpi=True)
        
        if self.mpi_rank == 0:
            self.assign_aux_numbers("Posterior")
            self.printer.new_stream("txt", synchronised=False)
            self.filename = get_filename(filename, "/StaticDynesty/", **kwargs)
        
    @staticmethod
    def gambit_loglike(params):
        lnew = StaticDynesty.loglike_hypercube(params)
        
        return (lnew, np.array([StaticDynesty.mpi_rank, StaticDynesty.point_id]))
    
    @staticmethod
    def gambit_transform(params):
        return params

    def run_internal(self, pkl_name=None, **kwargs):
        if self.mpi_size == 1:
            if self.printer.resume_mode():
                self.sampler = dynesty.NestedSampler.restore(self.filename)
                
                self.sampler.run_nested(checkpoint_file=self.filename, resume=True, **kwargs)
            else:
                self.sampler = dynesty.NestedSampler(
                    self.gambit_loglike,
                    self.gambit_transform,
                    self.dim, 
                    blob=True, 
                    **self.init_args)
                
                self.sampler.run_nested(checkpoint_file=self.filename, **kwargs)
        
        else:
            with MPIPool() as pool:
                if self.printer.resume_mode():
                    self.sampler = dynesty.NestedSampler.restore(self.filename, pool=pool)
                
                    self.sampler.run_nested(checkpoint_file=self.filename, resume=True, **kwargs)
                else:
                    if pool.is_master():
                        self.sampler = dynesty.NestedSampler(
                            self.gambit_loglike,
                            self.gambit_transform,
                            self.dim, 
                            blob=True, 
                            pool=pool, 
                            **self.init_args)
                        
                        self.sampler.run_nested(checkpoint_file=self.filename, **kwargs)
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
            
            if not pkl_name is None:
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

    __version__ = version(dynesty)

    @copydoc(dynesty.DynamicNestedSampler)
    def __init__(self, filename='dynesty.save', **kwargs):
        super().__init__(use_mpi=True)
        
        self.assign_aux_numbers("Posterior")
        if self.mpi_rank == 0:
            self.printer.new_stream("txt", synchronised=False)
            self.filename = get_filename(filename, "/DynamicDynesty/", **kwargs)
        
    #def __reduce__(self):
        #return (self.__class__, ())
    
    #def gambit_loglike(self, params):
        #lnew = self.loglike_hypercube(params)
        
        #return (lnew, np.array([self.mpi_rank, self.point_id]))
        
    @staticmethod
    def gambit_loglike(params):
        lnew = DynamicDynesty.loglike_hypercube(params)
        
        return (lnew, np.array([DynamicDynesty.mpi_rank, DynamicDynesty.point_id]))

    @staticmethod
    def gambit_transform(params):
        return params

    def run_internal(self, pkl_name=None, **kwargs):
        if self.mpi_size == 1:
            if self.printer.resume_mode():
                self.sampler = dynesty.DynamicNestedSampler.restore(self.filename)
                
                self.sampler.run_nested(checkpoint_file=self.filename, resume=True, **kwargs)
            else:
                self.sampler = dynesty.DynamicNestedSampler(
                    self.gambit_loglike,
                    self.gambit_transform,
                    self.dim, 
                    blob=True, 
                    **self.init_args)
                
                self.sampler.run_nested(checkpoint_file=self.filename, **kwargs)
        
        else:
            with MPIPool() as pool:
                if self.printer.resume_mode():
                    self.sampler = dynesty.DynamicNestedSampler.restore(self.filename, pool=pool)
                
                    self.sampler.run_nested(checkpoint_file=self.filename, resume=True, **kwargs)
                else:
                    if pool.is_master():
                        self.sampler = dynesty.DynamicNestedSampler(
                            self.gambit_loglike,
                            self.gambit_transform,
                            self.dim, 
                            blob=True, 
                            pool=pool, 
                            **self.init_args)
                        
                        self.sampler.run_nested(checkpoint_file=self.filename, **kwargs)
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


__plugins__ = {"static_dynesty": StaticDynesty,
               "dynamic_dynesty": DynamicDynesty}
