"""
Scipy dual annealing scannner
=============================
"""

from utils import copydoc, version

try:
    import scipy.optimize
    scipy_version = version("scipy")
    scipy_optimize_dual_annealing = scipy.optimize.dual_annealing
    scipy_optimize_basinhopping = scipy.optimize.basinhopping
    scipy_optimize_differential_evolution = scipy.optimize.differential_evolution
    scipy_optimize_direct = scipy.optimize.direct
except:
    __error__ = 'scipy.optimize pkg not installed'
    scipy_version = 'n/a'
    scipy_optimize_dual_annealing = None
    scipy_optimize_basinhopping = None
    scipy_optimize_differential_evolution = None
    scipy_optimize_direct = None

import scanner_plugin as splug


class DualAnnealing(splug.scanner):
    """
Dual annealing optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_dual_annealing)
    def run(self):

        # If an initial x0 is given in self.run_args, 
        # extract it and transform it to the unit hypercube
        x0_unit = None
        if 'x0' in self.run_args:
            x0_phys = self.run_args.pop('x0')
            x0_unit = self.inverse_transform(x0_phys)

        bounds = [(0., 1.)] * self.dim

        def neg_loglike_hypercube(x):
            return -self.loglike_hypercube(x)

        res = scipy.optimize.dual_annealing(neg_loglike_hypercube, bounds, x0=x0_unit, **self.run_args)

        print("\nOptimization summary from scipy_dual_annealing, with x-coordinates given in the unit hypercube in the order {0}:\n".format(self.parameter_names))
        print(res)
        x_phys = self.transform_to_vec(res.x)
        print("\nSolution x in physical coordinates: {0}\n".format(dict(zip(self.parameter_names, x_phys))))
        return 0
        

class BasinHopping(splug.scanner):
    """
Basin-hopping optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_basinhopping)
    def run(self):

        x0_unit = None
        if 'x0' in self.run_args:
            x0_phys = self.run_args.pop('x0')
            x0_unit = self.inverse_transform(x0_phys)
        else:
            # scipy.optimize.basinhopping requires an initial guess x0
            raise Exception("Missing 'x0' dictionary in the scipy_basin_hopping run settings.")

        def neg_loglike_hypercube(x):
            return -self.loglike_hypercube(x)

        res = scipy.optimize.basinhopping(neg_loglike_hypercube, x0_unit, **self.run_args)

        print("\nOptimization summary from scipy_basin_hopping, with x-coordinates given in the unit hypercube in the order {0}:\n".format(self.parameter_names))
        print(res)
        x_phys = self.transform_to_vec(res.x)
        print("\nSolution x in physical coordinates: {0}\n".format(dict(zip(self.parameter_names, x_phys))))
        return 0


class DifferentialEvolution(splug.scanner):
    """
Differential evolution optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_differential_evolution)
    def run(self):

        # If an initial x0 is given in self.run_args, 
        # extract it and transform it to the unit hypercube
        x0_unit = None
        if 'x0' in self.run_args:
            x0_phys = self.run_args.pop('x0')
            x0_unit = self.inverse_transform(x0_phys)

        bounds = [(0., 1.)] * self.dim

        def neg_loglike_hypercube(x):
            return -self.loglike_hypercube(x)

        res = scipy.optimize.differential_evolution(neg_loglike_hypercube, bounds, x0=x0_unit, **self.run_args)

        print("\nOptimization summary from scipy_differential_evolution, with x-coordinates given in the unit hypercube in the order {0}:\n".format(self.parameter_names))
        print(res)
        x_phys = self.transform_to_vec(res.x)
        print("\nSolution x in physical coordinates: {0}\n".format(dict(zip(self.parameter_names, x_phys))))
        return 0
        

class Direct(splug.scanner):
    """
The DIRECT optimizer from scipy.
    """

    __version__ = scipy_version
    
    def __init__(self, **kwargs):
        super().__init__(use_mpi=False, use_resume=False)

    @copydoc(scipy_optimize_direct)
    def run(self):

        bounds = [(0., 1.)] * self.dim

        def neg_loglike_hypercube(x):
            return -self.loglike_hypercube(x)

        res = scipy.optimize.direct(neg_loglike_hypercube, bounds, **self.run_args)

        print("\nOptimization summary from scipy_direct, with x-coordinates given in the unit hypercube in the order {0}:\n".format(self.parameter_names))
        print(res)
        x_phys = self.transform_to_vec(res.x)
        print("\nSolution x in physical coordinates: {0}\n".format(dict(zip(self.parameter_names, x_phys))))
        return 0
        

__plugins__ = {"scipy_dual_annealing": DualAnnealing,
               "scipy_basin_hopping": BasinHopping,
               "scipy_differential_evolution": DifferentialEvolution,
               "scipy_direct": Direct}
