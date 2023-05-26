"""
Delay any errors when importing a module
========================================

This is a way of loading modules such that errors are delayed until when methods
from the module are called.

This isn't necessarily by lazy loading - if modules can be imported greadily they will be.
"""

import importlib
import warnings
import sys


def delay_import_error(name):
    try:
        return importlib.import_module(name)
    except Exception as e:
        warnings.warn(str(e), category=ImportWarning)
        module = MockModule(name, e)
        sys.modules[name] = module
        return module


class LazyImportError(ImportError):
    pass


class MockModule:

    __version__ = "mocked"

    def __init__(self, name, exception, parent=None):
        self.name = f"{parent}.{name}" if parent is not None else name
        self.exception = exception
        self.__doc__ = f"mocked {self.name}. exception {repr(self.exception)}"

    def __getattr__(self, name):
        return MockModule(name, self.exception, parent=self.name)

    def __call__(self, *args, **kwargs):
        raise LazyImportError(self.exception)
