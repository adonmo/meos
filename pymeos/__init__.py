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
from _pymeos import *
from _pymeos import box, io, range, temporal, time

for module in (box, io, range, temporal, time):
    full_name = '{}.{}'.format(__package__, module.__name__.rsplit('.')[-1])
    sys.modules[full_name] = sys.modules[module.__name__]
