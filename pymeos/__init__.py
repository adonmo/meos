import importlib
import inspect
import os
import platform
import sys


# For Windows (Python 3.8+) we need some additional configuration to setup DLL path
# Refer: https://docs.python.org/3/whatsnew/3.8.html#bpo-36085-whatsnew
DEFAULT_GEOS_PATH = os.path.join('C:' + os.sep, 'Program Files (x86)', 'GEOS', 'bin')
ENV_KEY_GEOS_PATH = 'GEOS_BIN_PATH'

# This needs to happen before we can import anything from _pymeos
if platform.system() == "Windows":
    if hasattr(os, 'add_dll_directory'):
        path = os.environ.get(ENV_KEY_GEOS_PATH, DEFAULT_GEOS_PATH)
        os.add_dll_directory(path)


# Include all bindings created through pybind11
def _get_pymeos_path(package_name):
    """Takes a path starting with _pymeos.something and converts it into pymeos.something"""
    if package_name.startswith("_pymeos"):
        return package_name[1:]
    raise ValueError("Unsupported package name passed: " + package_name)


def _import_pymeos_submodules(package, recursive=True):
    """Import all submodules of '_pymeos', recursively, under 'pymeos'

    :param package: package (name or actual module)
    :type package: str | module
    :rtype: dict[str, types.ModuleType]
    """
    if isinstance(package, str):
        package = importlib.import_module(package)

    results = {}

    _m = importlib.import_module(package.__name__)
    pymeos_module_name = _get_pymeos_path(package.__name__)
    _m.__module__ = pymeos_module_name
    _m.__name__ = pymeos_module_name

    # Import all classes with __module__ modified
    # This is especially needed for generating Sphinx docs properly
    for _, _c in inspect.getmembers(package, inspect.isclass):
        _c.__module__ = pymeos_module_name

    sys.modules[pymeos_module_name] = _m

    # Recurse over all submodules
    if recursive:
        for _, _sm in inspect.getmembers(package, inspect.ismodule):
            _import_pymeos_submodules(_sm, recursive)


_import_pymeos_submodules('_pymeos')
