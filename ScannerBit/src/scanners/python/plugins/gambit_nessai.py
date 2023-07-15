"""
Nessai scanner
==============
"""

from nessai.flowsampler import FlowSampler
from nessai.model import Model
from nessai.utils import setup_logger

import scanner_plugin as splug
from utils import copydoc, version


class Gambit(Model):
    def __init__(self, cls):
        self.cls = cls
        self.names = self.cls.parameter_names
        self.bounds = {n: [0., 1] for n in self.names}

    def log_prior(self, x):
        return np.log(self.in_bounds(x), dtype="float")

    def log_likelihood(self, x):
        return self.cls.loglike_hypercube(x)


class FlowSampler(splug.scanner):
    """
    Nessai nested sampler. This operates on the unit hypercube
    as the nessai implementation requires bounds for each 
    parameter, which may not exist in physical parameters.
    """

    name = "nessai_flow_sampler"
    __version__ = version(nessai)

    @copydoc(FlowSampler)
    def __init__(self, output="nessai_log_dir", logger=True, **kwargs):
        """
        To ensure results are saved, by default we set the argument

        :param: output ('nessai_log_dir')
        """
        super().__init__()
        if logger:
            setup_logger(output=output)

        model = Gambit(self)
        self.sampler = FlowSampler(model, output=output, **kwargs)

    def run_internal(self, **kwargs):
        self.sampler.run(**kwargs)

    @copydoc(FlowSampler.run)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {FlowSampler.name: FlowSampler}
