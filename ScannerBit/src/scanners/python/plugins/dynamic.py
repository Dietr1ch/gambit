"""
Import a module dynamically
===========================
"""

import importlib

from utils import Scanner


class Dynamic(Scanner):

    name = "dynamic"
    version = "dynamic - no fixed version"

    def __init__(self, from_, import_, **kwargs):
        """
        Imports a scanner dynamically as though you had done

            from 'from_' import 'import_'

        Excuse the use of Python keywords in the argument names
        """
        module = importlib.import_module(from_)
        scanner = getattr(module, import_)
        self.instance = scanner(**kwargs)

    def run(self, **kwargs):
        self.instance.run(**kwargs)
