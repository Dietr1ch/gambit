"""
Python scanners for use in GAMBIT
=================================
"""

from .gambit_emcee import Emcee
from .gambit_dynesty import StaticDynesty, DynamicDynesty
from .gambit_ultranest import ReactiveUltranest
from .gambit_scipy import BasinHopping, DualAnnealing, DifferentialEvolution, Direct
from .grid import Grid
from .gambit_zeus import Zeus
from .gambit_pocomc import PocoMC


plugins = [Emcee, StaticDynesty, DynamicDynesty,
           ReactiveUltranest, BasinHopping, DualAnnealing, DifferentialEvolution, Direct, Grid, Zeus, PocoMC]
plugins = {p.name: p for p in plugins}
