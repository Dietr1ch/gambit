"""
Abstract base class for a Python scanner
========================================
"""

from abc import ABC, abstractmethod
from functools import cached_property

from mpi4py import MPI
import numpy as np

import scannerbit
import scanner_pyplugin as splug


class Scanner(ABC):
    """
    GAMBIT scanner implementation in Python
    """
    @abstractmethod
    def run(self):
        """
        Run the Python scanner
        """

    @property
    @abstractmethod
    def name(self):
        """
        :returns: Name of scanner
        """

    @property
    @abstractmethod
    def version(self):
        """
        :returns: Version of scanner
        """

    def _arr2map(self, arr):
        """
        :returns: Array as parameter map
        """
        map_ = scannerbit.std_unordered_map_double()
        for k, v in zip(self.parameter_names, arr):
            map_[k] = v
        return map_

    def _map2arr(self, map_):
        """
        :returns: Parameter map as array
        """
        arr = np.empty(self.dim)
        for i, k in enumerate(self.parameter_names):
            arr[i] = map_[k]
        return arr

    @cached_property
    def parameter_names(self):
        """
        :returns: List of parameter names
        """
        p = splug.get_prior().getShownParameters()
        return [p[i] for i in range(self.dim)]

    def log(self, message):
        """
        Write to GAMBIT logger
        """
        splug.get_printer().get_stream().print(
            0., str(message), self.mpi_rank, self.loglike.getPtID())

    @cached_property
    def dim(self):
        """
        :returns: Dimension of problem, i.e., the number of parameters
        """
        return splug.get_dimension()

    def prior_transform(self, unit):
        """
        :returns: Transform unit hypercube to physical parameters
        """
        return self._map2arr(splug.get_prior().transform(unit))

    def inverse_prior_transform(self, physical):
        """
        :returns: Transform physical parameters to unit hypercube
        """
        return np.array(splug.get_prior().inverse_transform(self._arr2map(physical)))

    def log_prior_density(self, physical):
        """
        :returns: Log of prior density
        """
        unit_hypercube = self.inverse_prior_transform(physical)

        if np.any(unit_hypercube > 1) or np.any(unit_hypercube < 0):
            return -np.inf

        return splug.get_prior().log_prior_density(physical)

    def log_target_density(self, physical):
        """
        :returns: Log of target density
        """
        return self.loglike(physical) + self.log_prior_density(physical)

    @cached_property
    def loglike_hypercube(self):
        """
        :returns: Log of likelihood function, though evaluated on unit hypercube
        """
        purpose = splug.get_inifile_value("like", dtype=str)
        return splug.get_purpose(purpose)

    def loglike(self, physical):
        """
        :returns: Log of likelihood function
        """
        unit_hypercube = self.inverse_prior_transform(physical)
        return self.loglike_hypercube(unit_hypercube)

    @property
    def mpi_rank(self):
        """
        :returns: Rank of this thread for MPI
        """
        if scannerbit.with_mpi and scannerbit.numtasks() > 1:
            return MPI.COMM_WORLD.Get_rank()
        return 0

    @cached_property
    def mpi_size(self):
        """
        :returns: Number of MPI threads
        """
        if scannerbit.with_mpi and scannerbit.numtasks() > 1:
            return MPI.COMM_WORLD.Get_size()
        return 1
