"""
Zeus scanner
============
"""

import numpy as np
from utils import copydoc, version, get_filename, MPIPool

try:
    import zeus
    zeus_version = version(zeus)
    zeus_EnsembleSampler = zeus.EnsembleSampler
    zeus_EnsembleSampler_run_mcmc = zeus.EnsembleSampler.run_mcmc
except:
    __error__='zeus-mcmc pkg not installed'
    zeus_version ='n/a'
    zeus_EnsembleSampler=None
    zeus_EnsembleSampler_run_mcmc=None

import scanner_plugin as splug


class Zeus(splug.scanner):
    """
    An ensamble slice sampler.  Unfortunately, resume does not work on this sampler.  See https://zeus-mcmc.readthedocs.io/en/latest/index.html
    """

    __version__ = zeus_version

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
        return np.vstack([np.random.rand(self.dim)
                         for i in range(self.nwalkers)])
    
    @staticmethod
    def my_like(params):
        
        if ((params < 1.0) & (params > 0.0)).all():
            lnew = Zeus.loglike_hypercube(params)
            
            return (lnew, Zeus.mpi_rank, Zeus.point_id)
        else:
            return  (-np.inf, -1, -1)

    @copydoc(zeus_EnsembleSampler)
    def __init__(self, nwalkers=8, filename="zeus.h5", **kwargs):
        super().__init__()
        self.nwalkers = 8
        if 'nwalkers' in self.init_args:
            self.nwalkers = self.init_args['nwalkers']
            del self.init_args['nwalkers']
            
        self.assign_aux_numbers("mult")
        if self.mpi_rank == 0:
            self.printer.new_stream("txt", synchronised=False)
            self.filename = get_filename(filename, "Emcee", **kwargs)

    def run_internal(self, pkl_name=None, nsteps=100, initial_state=None, **kwargs):
        """
        There is one additional arguments:

        :param: filename ('zeus.h5')

        for passing the name of a h5 file to which to save results using the zeus writer.
        """
        
            
        if self.mpi_size == 1:
            if initial_state is None:
                initial_state = self.initial_state()
            self.sampler = zeus.EnsembleSampler(
                self.nwalkers, self.dim, self.my_like, blobs_dtype=[("rank", int), ("ptid", int)], **self.init_args)
            self.sampler.run_mcmc(initial_state, nsteps, 
                                callbacks=self.save_callback(self.filename), **kwargs)
        else:
            with MPIPool() as pool:
                if pool.is_master():
                    if initial_state is None:
                        initial_state = self.initial_state()
                    self.sampler = zeus.EnsembleSampler(
                        self.nwalkers, self.dim, self.my_like, blobs_dtype=[("rank", int), ("ptid", int)], pool=pool **self.init_args)
                    self.sampler.run_mcmc(initial_state, nsteps,
                                        callbacks=self.save_callback(self.filename), **kwargs)
        
        if self.mpi_rank == 0:
            stream = self.printer.get_stream("txt")
            stream.reset()
            blobs = self.sampler.get_blobs(flat=True)
            blobs = np.array([blobs['rank'], blobs["ptid"]])
            
            for i in range(self.mpi_size):
                us, cs = np.unique(blobs[1, blobs[0, :]==i], return_counts=True)
                print(us, cs)
                for u, c in zip(us, cs):
                    stream.print(c, "mult", i, u)
            stream.flush()
            
            if not pkl_name is None:
                samples = self.sampler.get_samples()
                with open(pkl_name, "wb") as f:
                    pickle.dump(samples, f)

    @copydoc(zeus_EnsembleSampler_run_mcmc)
    def run(self):
        self.run_internal(**self.run_args)
        

__plugins__ = {"zeus": Zeus}
