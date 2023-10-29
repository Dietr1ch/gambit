"""
Zeus scanner
============
"""

import numpy as np
from utils import copydoc, version, get_filename, with_mpi
if with_mpi:
    from utils import MPIPool, MPI

try:
    import zeus
    zeus_version = version(zeus)
    zeus_EnsembleSampler = zeus.EnsembleSampler
    zeus_EnsembleSampler_run_mcmc = zeus.EnsembleSampler.run_mcmc
    if with_mpi:
        from zeus import ChainManager
except:
    __error__='zeus-mcmc pkg not installed'
    zeus_version ='n/a'
    zeus_EnsembleSampler=None
    zeus_EnsembleSampler_run_mcmc=None

import scanner_plugin as splug


class Zeus(splug.scanner):
    """
    An ensamble slice sampler.  Unfortunately, resume does not work on this sampler.  See https://zeus-mcmc.readthedocs.io/en/latest/index.html
    
    There are additional arguments:
        
        nwalkers (8):  Number of walkers
        filename ('zeus.h5'):  For passing the name of a h5 file to which to save results using the emcee writer
        ncheck (100):  Saves after that many points 
        pkl_name ('zeus.pkl'):  File name where results will be pickled
        use_chain_manager (False):  Use the internal chain manager to manage mpi processes.  This will run mpi_size chains in parrellel
    """

    __version__ = zeus_version

    def save_callback(self, filename=None, ncheck=100):
        """
        :returns: Callback that saves results
        """
        if not filename:
            return None
        return zeus.callbacks.SaveProgressCallback(filename=filename, ncheck=ncheck)

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
    def __init__(self, nwalkers=8, filename="zeus.h5", ncheck=100, pkl_name=None, use_chain_manager=False, **kwargs):
        super().__init__()
        
        if self.printer.resume_mode():
            raise Exception("Zeus does not support resuming.")
        
        self.nwalkers = nwalkers
        if 'nwalkers' in self.init_args:
            self.nwalkers = self.init_args['nwalkers']
            del self.init_args['nwalkers']
        
        self.use_cm = use_chain_manager
        if self.mpi_rank == 0 or self.use_cm:
            self.ncheck=ncheck
            self.log_dir = get_filename("", "Zeus", **kwargs)
            self.filename = self.log_dir + filename
            self.assign_aux_numbers("mult")
            self.pkl_name = pkl_name
            self.printer.new_stream("txt", synchronised=False)


    def run_internal(self, nsteps=1000, nchains=2, start=None, **kwargs):
            
        if self.mpi_size == 1:
            if start is None:
                start = self.initial_state()
            self.sampler = zeus.EnsembleSampler(
                self.nwalkers, self.dim, self.my_like, blobs_dtype=[("rank", int), ("ptid", int)], **self.init_args)
            self.sampler.run_mcmc(start, nsteps, 
                                callbacks=self.save_callback(filename=self.filename, ncheck=self.ncheck), **kwargs)
        else:
            if self.use_cm:
                with ChainManager(self.mpi_size) as cm:
                    if start is None:
                        start = self.initial_state()
                    self.sampler = zeus.EnsembleSampler(
                        self.nwalkers, self.dim, self.my_like, blobs_dtype=[("rank", int), ("ptid", int)], pool=cm.get_pool, **self.init_args)
                    self.sampler.run_mcmc(start, nsteps,
                                        callbacks=self.save_callback(filename=self.filename, ncheck=self.ncheck), **kwargs)
            else:
                with MPIPool() as pool:
                    if pool.is_master():
                        if start is None:
                            start = self.initial_state()
                        self.sampler = zeus.EnsembleSampler(
                            self.nwalkers, self.dim, self.my_like, blobs_dtype=[("rank", int), ("ptid", int)], pool=pool, **self.init_args)
                        self.sampler.run_mcmc(start, nsteps,
                                            callbacks=self.save_callback(filename=self.filename, ncheck=self.ncheck), **kwargs)
                    else:
                        pool.wait()
        
        if self.mpi_rank == 0 or self.use_cm:
            stream = self.printer.get_stream("txt")
            stream.reset()
            blobs = self.sampler.get_blobs(flat=True)
            blobs = np.array([blobs['rank'], blobs["ptid"]])
            
            for i in range(self.mpi_size):
                us, cs = np.unique(blobs[1, blobs[0, :]==i], return_counts=True)
                
                for u, c in zip(us, cs):
                    stream.print(c, "mult", i, u)
            stream.flush()
                
            if (not self.pkl_name is None) and (self.pkl_name != ""):
                samples = self.sampler.get_samples()
                with open(self.log_dir + self.pkl_name + "_" + str(self.mpi_rank), "wb") as f:
                    pickle.dump(samples, f)

    @copydoc(zeus_EnsembleSampler_run_mcmc)
    def run(self):
        self.run_internal(**self.run_args)
        

__plugins__ = {"zeus": Zeus}
