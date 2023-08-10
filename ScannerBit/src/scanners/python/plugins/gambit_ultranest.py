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

    @copydoc(ultranest.ReactiveNestedSampler)
    def __init__(self, log_dir="ultranest_log_dir", **kwargs):
        """
        To ensure results are saved, by default we set the argument

        :param: log_dir ('reactive_ultranest_run')
        """
        self.sampler = ultranest.ReactiveNestedSampler(
            self.parameter_names,
            self.loglike,
            transform=self.transform,
            log_dir=log_dir,
            **kwargs)

    
    def run_internal(self, pkl_name="ultranest.pkl", **kwargs):
        """
        We add the argument

        :param: pkl_name ('ultranest.pkl')

        to store the results from the sampler to a pickle. This helps inspect
        results outside gambit.
        """
        self.sampler.run(**kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(self.sampler.results, f)
    
    @copydoc(ultranest.ReactiveNestedSampler.run)
    def run():
        self.run_internal(**self.run_args)


__plugins__ = {ReactiveUltranest.name: ReactiveUltranest}
