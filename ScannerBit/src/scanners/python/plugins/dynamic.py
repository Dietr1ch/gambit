"""
Import a module dynamically
===========================
"""

import importlib
import scanner_plugin as splug


class Dynamic(splug.scanner):

    name = "dynamic"
    version = "dynamic - no fixed version"

    def __init__(self, from_, import_, **kwargs):
        """
        Imports a scanner dynamically as though you had done

            from 'from_' import 'import_'

        Excuse the use of Python keywords in the argument names
        """
        super().__init__(use_mpi=False)
        module = importlib.import_module(from_)
        scanner = getattr(module, import_)
        self.instance = scanner(**kwargs)

    def run(self):
        self.instance.run(**self.run_args)

__plugins__ = {Dynamic.name: Dynamic}
