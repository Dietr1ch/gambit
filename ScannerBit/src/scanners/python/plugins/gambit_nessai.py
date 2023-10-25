"""
Nessai scanner
==============
"""
import nessai
from nessai.flowsampler import FlowSampler
from nessai.model import Model
from nessai.utils import setup_logger
from nessai.utils.multiprocessing import initialise_pool_variables

import scanner_plugin as splug
from utils import copydoc, version, get_filename, MPIPool


class Gambit(Model):
    def __init__(self, cls):
        self.like = cls.loglike_hypercube
        self.names = cls.parameter_names
        self.bounds = {n: [0., 1] for n in self.names}

    def log_prior(self, x):
        return np.log(self.in_bounds(x), dtype="float")

    def log_likelihood(self, x):
        return self.like(x)


class FlowSampler(splug.scanner):
    """
    Nessai nested sampler. This operates on the unit hypercube
    as the nessai implementation requires bounds for each 
    parameter, which may not exist in physical parameters.
    """

    __version__ = version(nessai)

    @copydoc(FlowSampler)
    def __init__(self, logger=True, **kwargs):
        """
        To ensure results are saved, by default we set the argument

        :param: output ('nessai_log_dir')
        """
        #output="nessai_log_dir", 
        #self.output = output
        super().__init__(use_mpi=True)
        #if logger:
        #    setup_logger(output=output)

    def run_internal(self, **kwargs):
        model = Gambit(self)
        
        if self.mpi_size == 1:
            self.sampler = FlowSampler(model, output='./', nlive=1000)
            self.sampler.run()
        else:
            initialise_pool_variables(model)
            with MPIPool() as pool:
                if pool.is_master():
                    self.sampler = FlowSampler(model, pool=pool, output=self.output, **self.init_args)
                    self.sampler.run(**kwargs)
                else:
                    pool.wait()
                    
        results = self.sampler.nested_samples
        print(results)

    @copydoc(FlowSampler.run)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {"nessai_flow_sampler": FlowSampler}
