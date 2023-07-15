"""
Dynesty scanners
================

The static and dynamic Dynesty classes are implemented separately.

We don't use the Dynesty checkpointing/saving functionality, as it
attempts to pickle the loglikelihood function etc.
"""


import pickle
import dynesty

import scanner_plugin as splug
from utils import copydoc, version


class StaticDynesty(splug.scanner):
    """
    Dynesty nested sampler with static number of live points.

    We add an additional run parameter:

    :param: pkl_name ('static_dynesty.pkl')
    """

    name = "static_dynesty"
    __version__ = version(dynesty)

    @copydoc(dynesty.NestedSampler)
    def __init__(self, **kwargs):
        super().__init__()
        self.sampler = dynesty.NestedSampler(
            self.loglike, self.prior_transform, self.dim, **kwargs)

    def run_internal(self, pkl_name="static_dynesty.pkl", **kwargs):
        self.sampler.run_nested(**kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(self.sampler.results, f)
            
    @copydoc(dynesty.NestedSampler.run_nested)
    def run():
        self.run_internal(**self.run_args)


class DynamicDynesty(splug.scanner):
    """
    Dynesty nested sampler with dynamic number of live points.

    We add an additional run parameter:

    :param: pkl_name ('dynamic_dynesty.pkl')
    """

    name = "dynamic_dynesty"
    __version__ = version(dynesty)

    @copydoc(dynesty.DynamicNestedSampler)
    def __init__(self, **kwargs):
        super().__init__()
        self.sampler = dynesty.DynamicNestedSampler(
            self.loglike, self.prior_transform, self.dim, **kwargs)

    def run_internal(self, pkl_name="dynamic_dynesty.pkl", **kwargs):
        self.sampler.run_nested(**kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(self.sampler.results, f)
     
    @copydoc(dynesty.DynamicNestedSampler.run_nested)
    def run(self):
        self.run_internal(**self.run_args)


__plugins__ = {StaticDynesty.name: StaticDynesty,
               DynamicDynesty.name: DynamicDynesty}
