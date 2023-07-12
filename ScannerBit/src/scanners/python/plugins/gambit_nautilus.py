"""
Nautilus scanner
================
"""

import nautilus

import scanner_plugin as splug
from utils import copydoc, version


class Nautilus(splug.scanner):

    name = "nautilus"
    __version__ = version(nautilus)

    @copydoc(nautilus.Sampler)
    def __init__(self, **kwargs):
        self.prior = nautilus.Prior()
        for key in self.parameter_names:
            self.prior.add_parameter(key)

        def loglike(param_dict):
            x = [param_dict[key] for key in self.parameter_names]
            return self.loglike(x)

        sampler = nautilus.Sampler(prior, likelihood, **kwargs)

    def run_internal(self, pkl_name = "nautilus.pkl", **kwargs):
        self.sampler.run(**kwargs)
        res=self.sampler.posterior()

        with open(pkl_name, "wb") as f:
            pickle.dump(res, f)

    @copydoc(nautilus.Sampler.run)
    def run():
        self.run_internal(**self.run_args)


__plugins__={Nautilus.name: Nautilus}
