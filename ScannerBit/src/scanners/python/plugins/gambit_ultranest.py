"""
Ultranest scanners
==================
"""

import pickle
import ultranest
from mpi4py import MPI
import numpy as np

import scanner_plugin as splug
from utils import copydoc, version

class ReactiveUltranest(splug.scanner):
    """
    Ultranest reactive sampler.
    """

    name = "reactive_ultranest"
    __version__ = version(ultranest)
    
    def ultra_like(self, params):
        #print("params = ", params)
        lnew = self.loglike_physical(params)
        self.saves[tuple(params)] = (self.mpi_rank, self.point_id)
        #print(tuple(cube), ":", self.point_id)
        return lnew
    
    def transfer(self):
        comm = MPI.COMM_WORLD
        
        if self.mpi_rank == 0:
            for i in range(1, self.mpi_size):
                data = comm.recv(source=i, tag=1)
                self.saves.update(data)
        else:
            comm.send(self.saves, dest=0, tag=1)
    
    @copydoc(ultranest.ReactiveNestedSampler)
    def __init__(self, log_dir="ultranest_log_dir", **kwargs):
        """
        To ensure results are saved, by default we set the argument

        :param: log_dir ('reactive_ultranest_run')
        """
        print("inits = ", self.init_args)
        super().__init__()
        self.saves = {}
        self.sampler = ultranest.ReactiveNestedSampler(
            self.parameter_names,
            self.ultra_like,
            transform=self.transform_to_vec,
            log_dir=log_dir,
            **self.init_args)

    
    def run_internal(self, pkl_name="ultranest.pkl", **kwargs):
        """
        We add the argument

        :param: pkl_name ('ultranest.pkl')

        to store the results from the sampler to a pickle. This helps inspect
        results outside gambit.
        """
        
        self.sampler.run(**kwargs)
        self.transfer()
        
        if self.mpi_rank == 0:
            result = self.sampler.results
            wts = result["weighted_samples"]["weights"]
            pts = result["weighted_samples"]["points"]
            
            for wt, pt in zip(wts, pts):
                if tuple(pt) in self.saves:
                    save = self.saves[tuple(pt)]
                    self.print(wt, "Posterior", save[0], save[1])
                else:
                    print("warning: point ", tuple(pt), " has no correponding id.")
                        
            with open(pkl_name, "wb") as f:
                pickle.dump(result, f)
    
    @copydoc(ultranest.ReactiveNestedSampler.run)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {ReactiveUltranest.name: ReactiveUltranest}
