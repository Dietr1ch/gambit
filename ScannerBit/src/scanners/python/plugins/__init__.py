"""
Python scanners for use in GAMBIT
=================================
"""

# avoid import errors at load time for optional modules
# errors will still come around at runtime

from .delay import delay_import_error

_optional = [
    "dynesty",
    "emcee",
    "h5py",
    "ultranest",
    "zeus",
    "pocomc"]

for o in _optional:
    delay_import_error(o)

# import implementations of scanner

from .dynamic import Dynamic
from .gambit_pocomc import PocoMC
from .gambit_zeus import Zeus
from .grid import Grid
from .gambit_scipy import BasinHopping, DualAnnealing, DifferentialEvolution, Direct
from .gambit_ultranest import ReactiveUltranest
from .gambit_dynesty import StaticDynesty, DynamicDynesty
from .gambit_emcee import Emcee
from .scanner import Scanner

# place them in dictionary

plugins = [
    Dynamic,
    Emcee,
    StaticDynesty,
    DynamicDynesty,
    ReactiveUltranest,
    BasinHopping,
    DualAnnealing,
    DifferentialEvolution,
    Direct,
    Grid,
    Zeus,
    PocoMC]
plugins = {p.name: p for p in plugins}
