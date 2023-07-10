"""
Adaptive learning
=================
"""


import numpy as np
import adaptive
import scanner_plugin as splug
from utils import copydoc, version


class Adaptive(splug.scanner):
    """
    Dynesty nested sampler with static number of live points.

    We add an additional run parameter:

    :param: pkl_name ('static_dynesty.pkl')
    """

    name = "adaptive_learner_nd"
    __version__ = version(adaptive)

    @copydoc(adaptive.LearnerND)
    def __init__(self, **kwargs):
        bounds = [(0., 1.)] * self.dim
        self.learner = adaptive.Learner1D(self.loglike_hypercube, bounds=bounds, **kwargs)
        
    def run_internal(self, filename="adaptive.npy", **kwargs):
        runner = adaptive.Runner(self.learner, **kwargs)
        runner.live_info()
        runner.live_plot()
        
        if filename is not None:
            data = self.learner.to_numpy()
            np.savetxt(filename, data)
            
    @copydoc(adaptive.Runner)
    def run():
        self.run_internal(**self.run_args)


__plugins__ = {Adaptive.name: Adaptive}
