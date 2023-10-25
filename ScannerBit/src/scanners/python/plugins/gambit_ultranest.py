"""
Ultranest scanners
==================
"""

import pickle
import ultranest
from mpi4py import MPI
import numpy as np

import scanner_plugin as splug
from utils import copydoc, version, parse, get_filename

class ReactiveUltranest(splug.scanner):
    """
    Ultranest reactive sampler.
    """

    __version__ = version(ultranest)
    
    def ultra_like(self, params):
        lnew = self.loglike_hypercube(params)
        self.saves[tuple(params)] = (self.mpi_rank, self.point_id)

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
        
        super().__init__(use_mpi=True)
        if self.mpi_size > 1 and parse(ultranest.__version__) < parse("3.6.3"):
            print("WARNING: UltraNest current version is {0}.  Versions < 3.6.3 are bugged when using MPI.".format(ultranest.__version__))
            #raise Exception("UltraNest current version is {0}.  Versions < 3.6.3 are bugged when using MPI.".format(ultranest.__version__))
        
        self.assign_aux_numbers("Posterior")
        if self.mpi_rank == 0:
            self.printer.new_stream("txt", synchronised=False)

        self.saves = {}
        self.sampler = ultranest.ReactiveNestedSampler(
            ["unit[{0}]".format(i) for i in range(self.dim)],
            self.ultra_like,
            resume='resume-similar' if self.printer.resume_mode() else 'overwrite',
            log_dir=get_filename("", log_dir, **kwargs),
            **self.init_args)
    
    def run_internal(self, pkl_name=None, **kwargs):
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
            stream = self.printer.get_stream("txt")
            stream.reset()
            for wt, pt in zip(wts, pts):
                if tuple(pt) in self.saves:
                    save = self.saves[tuple(pt)]
                    stream.print(wt, "Posterior", save[0], save[1])
                else:
                    print("warning: point ", tuple(pt), " has no correponding id.")
            stream.flush()
            
            if not pkl_name is None:
                with open(pkl_name, "wb") as f:
                    pickle.dump(result, f)
    
    @copydoc(ultranest.ReactiveNestedSampler.run)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {"reactive_ultranest": ReactiveUltranest}
