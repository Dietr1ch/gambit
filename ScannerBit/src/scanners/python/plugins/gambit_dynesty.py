"""
Dynesty scanners
================

The static and dynamic Dynesty classes are implemented separately.

We don't use the Dynesty checkpointing/saving functionality, as it
attempts to pickle the loglikelihood function etc.
"""


import pickle

import dynesty

from .scanner import Scanner
from .copydoc import copydoc
from .version import version


class StaticDynesty(Scanner):
    """
    Dynesty nested sampler with static number of live points.

    We add an additional run parameter:

    :param: pkl_name ('static_dynesty.pkl')
    """

    name = "static_dynesty"
    version = version(dynesty)

    @copydoc(dynesty.NestedSampler)
    def __init__(self, **kwargs):
        self.sampler = dynesty.NestedSampler(
            self.loglike, self.prior_transform, self.dim, **kwargs)

    @copydoc(dynesty.NestedSampler.run_nested)
    def run(self, pkl_name="static_dynesty.pkl", **kwargs):
        self.sampler.run_nested(**kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(self.sampler.results, f)


class DynamicDynesty(Scanner):
    """
    Dynesty nested sampler with dynamic number of live points.

    We add an additional run parameter:

    :param: pkl_name ('dynamic_dynesty.pkl')
    """

    name = "dynamic_dynesty"
    version = version(dynesty)

    @copydoc(dynesty.DynamicNestedSampler)
    def __init__(self, **kwargs):
        self.sampler = dynesty.DynamicNestedSampler(
            self.loglike, self.prior_transform, self.dim, **kwargs)

    @copydoc(dynesty.DynamicNestedSampler.run_nested)
    def run(self, pkl_name="dynamic_dynesty.pkl", **kwargs):
        self.sampler.run_nested(**kwargs)
        with open(pkl_name, "wb") as f:
            pickle.dump(self.sampler.results, f)
