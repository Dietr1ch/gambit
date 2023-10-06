"""
Ultranest scanners
==================
"""

import pickle
import ultranest

import scanner_plugin as splug
from utils import copydoc, version

class ReactiveUltranest(splug.scanner):
    """
    Ultranest reactive sampler.
    """

    name = "reactive_ultranest"
    __version__ = version(ultranest)
    
    def ultra_like(self, cube):
        lnew = self.loglike(cube)
        self.saves[tuple(cube)] = (self.mpi_rank, self.point_id)
        
        return lnew
    
    @copydoc(ultranest.ReactiveNestedSampler)
    def __init__(self, log_dir="ultranest_log_dir", **kwargs):
        """
        To ensure results are saved, by default we set the argument

        :param: log_dir ('reactive_ultranest_run')
        """
        super().__init__(use_mpi=False) # False for now.
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
        result = self.sampler.results
        wts = result["weighted_samples"]["weights"]
        upts = result["weighted_samples"]["upoints"]
        
        for wt, upt in zip(wts, upts):
            if tuple(upt) in self.saves:
                save = self.saves[tuple(upt)]
                self.print(wt, "Posterior", save[0], save[1])
            else:
                print("warning: point has no correponding id.")
                    
        with open(pkl_name, "wb") as f:
            pickle.dump(result, f)
    
    @copydoc(ultranest.ReactiveNestedSampler.run)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {ReactiveUltranest.name: ReactiveUltranest}
