"""
Mock for testing
================
"""

import sys

from unittest.mock import MagicMock, PropertyMock
from contextlib import contextmanager

from import_mocker import ImportMocker


@contextmanager
def mock_gambit(dim=1, mpi_rank=0, mpi_size=1, version="1.0.0", name="Mock", log=print, loglike_hypercube=lambda x: -x[0]**2,
                prior_transform=lambda x: x, inverse_prior_transform=lambda x: x, parameter_names=None, log_prior_density=lambda x: 1.):
    """
    Context in which gambit dependencies are mocked to allow for easy testing
    """

    if parameter_names is None:
        parameter_names = [f"p{i}" for i in range(dim)]

    # mock import

    imocker = ImportMocker(['scannerbit', 'scanner_pyplugin'])

    # mock attributes of abc class

    mocked = imocker.import_module('base')
    mocked.Scanner.dim = PropertyMock(return_value=dim)
    mocked.Scanner.mpi_rank = PropertyMock(return_value=mpi_rank)
    mocked.Scanner.mpi_size = PropertyMock(return_value=mpi_size)
    mocked.Scanner.version = PropertyMock(return_value=version)
    mocked.Scanner.name = PropertyMock(return_value=name)
    mocked.Scanner.log = MagicMock(side_effect=log)
    mocked.Scanner.loglike_hypercube = MagicMock(side_effect=loglike_hypercube)
    mocked.Scanner.log_prior_density = MagicMock(side_effect=log_prior_density)
    mocked.Scanner.prior_transform = MagicMock(side_effect=prior_transform)
    mocked.Scanner.inverse_prior_transform = MagicMock(side_effect=inverse_prior_transform)
    mocked.Scanner.parameter_names = PropertyMock(return_value=parameter_names)

    yield mocked

    # reset mocking

    imocker.reset_mocks()
    del sys.modules['base']
