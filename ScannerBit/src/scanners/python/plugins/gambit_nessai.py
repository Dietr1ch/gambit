"""
Nessai scanner
==============
"""

import pickle
import numpy as np
from utils import copydoc, get_filename, version, with_mpi, store_pt_data
if with_mpi:
    from utils import MPIPool

try:
    import nessai
    from nessai.flowsampler import FlowSampler
    from nessai.model import Model
    from nessai.utils import setup_logger
    from nessai.utils.multiprocessing import initialise_pool_variables
    nessai_version = version(nessai)
    FlowSampler_run = FlowSampler.run
except:
    __error__ = 'nessai pkg not installed'
    nessai_version = 'n/a'
    FlowSampler = None
    FlowSampler_run = None
    class Model:
        pass

import scanner_plugin as splug


class Gambit_Model(Model):
    def __init__(self, gambit):
        self.names = ["unit_{0}".format(i) for i in range(gambit.dim)]
        self.bounds = {n: [0.0, 1.0] for n in self.names}
        self.gambit = gambit
        self.pt = np.zeros(len(self.names))

    def log_prior(self, params):
        return np.log(self.in_bounds(params), dtype="float")

    def log_likelihood(self, params):
        for i in range(len(self.names)):
            self.pt[i] = params[self.names[i]]
        
        self.gambit.ids.save((tuple(self.pt)), (self.gambit.mpi_rank, self.gambit.point_id))
        
        return self.gambit.loglike_hypercube(self.pt)


class GambitFlowSampler(splug.scanner):
    """
Nessai nested sampler. This operates on the unit hypercube as the nessai implementation requires bounds for each parameter, which may not exist in physical parameters.  See https://nessai.readthedocs.io/en/latest/index.html

We defined the additional parameters:
    output ('nessai_log_dir'):  output directory name.  Defined in given default path.
    logger (True):  Wether to use the logger
    """

    __version__ = nessai_version

    @copydoc(FlowSampler)
    def __init__(self, logger=True, output="nessai_log_dir", **kwargs):

        super().__init__(use_mpi=True)
        
        self.output_dir = get_filename("", output, **kwargs)
        self.ids = store_pt_data(resume=self.printer.resume_mode(), log_dir=self.output_dir)
        
        if logger:
            setup_logger(output=self.output_dir)
            
        self.assign_aux_numbers("Posterior")
        if self.mpi_rank == 0:
            self.printer.new_stream("txt", synchronised=False)

    def run_internal(self, **kwargs):
        model = Gambit_Model(self)
        
        if self.mpi_size == 1:
            self.sampler = FlowSampler(model,
                                       resume=self.printer.resume_mode(),
                                       output=self.output_dir,
                                       **self.init_args)
            self.sampler.run(**kwargs)
        else:
            initialise_pool_variables(model)
            with MPIPool(use_join=True) as pool:
                if pool.is_master():
                    self.sampler = FlowSampler(model, 
                                               pool=pool,
                                               resume=self.printer.resume_mode(),
                                               output=self.output_dir, 
                                               **self.init_args)
                    self.sampler.run(**kwargs)
                else:
                    pool.wait()
                    
        
        self.ids.load_saves()
                
        if self.mpi_rank == 0:
            results = self.sampler.nested_samples
            stream = self.printer.get_stream("txt")
            stream.reset()
            pt = np.zeros(len(model.names))
            for result in results:
                for i in range(len(model.names)):
                    pt[i] = result[model.names[i]]
                if tuple(pt) in self.ids.saves:
                    save = self.ids.saves[tuple(pt)]
                    stream.print(1.0, "Posterior", save[0], save[1])
                else:
                    print("warning: point ", tuple(pt), " has no correponding id.")
            stream.flush()

    @copydoc(FlowSampler_run)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {"nessai_flow_sampler": GambitFlowSampler}
