import fnmatch
import os
import platform
import subprocess
import sys
import logging
from distutils.version import LooseVersion
from setuptools import setup, Extension

log = logging.getLogger(__name__)
ch = logging.StreamHandler()
log.addHandler(ch)

MIN_GEOS_VERSION = "3.5"

if "all" in sys.warnoptions:
    # show GEOS messages in console with: python -W all
    log.setLevel(logging.DEBUG)


def get_geos_config(option):
    """Get configuration option from the `geos-config` development utility

    The PATH environment variable should include the path where geos-config is located.
    """
    try:
        stdout, stderr = subprocess.Popen(
            ["geos-config", option], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        ).communicate()
    except OSError:
        return
    if stderr and not stdout:
        log.warning("geos-config %s returned '%s'", option, stderr.decode().strip())
        return
    result = stdout.decode().strip()
    log.debug("geos-config %s returned '%s'", option, result)
    return result


def get_geos_paths():
    """Obtain the paths for compiling and linking with the GEOS C-API

    First the presence of the GEOS_INCLUDE_PATH and GEOS_LIBRARY_PATH environment
    variables is checked. If they are both present, these are taken.

    If one of the two paths was not present, geos-config is called (it should be on the
    PATH variable). geos-config provides all the paths.

    If geos-config was not found, no additional paths are provided to the extension. It is
    still possible to compile in this case using custom arguments to setup.py.
    """
    include_dir = os.environ.get("GEOS_INCLUDE_PATH")
    library_dir = os.environ.get("GEOS_LIBRARY_PATH")
    if include_dir and library_dir:
        return {
            "include_dirs": [include_dir],
            "library_dirs": [library_dir],
            "libraries": ["geos_c"],
        }
    geos_version = get_geos_config("--version")
    if not geos_version:
        log.warning(
            "Could not find geos-config executable. Either append the path to geos-config"
            " to PATH or manually provide the include_dirs, library_dirs, libraries and "
            "other link args for compiling against a GEOS version >=%s.",
            MIN_GEOS_VERSION,
        )
        return {}
    if LooseVersion(geos_version) < LooseVersion(MIN_GEOS_VERSION):
        raise ImportError(
            "GEOS version should be >={}, found {}".format(
                MIN_GEOS_VERSION, geos_version
            )
        )
    libraries = []
    library_dirs = []
    include_dirs = []
    extra_link_args = []
    for item in get_geos_config("--cflags").split():
        if item.startswith("-I"):
            include_dirs.extend(item[2:].split(":"))
    for item in get_geos_config("--clibs").split():
        if item.startswith("-L"):
            library_dirs.extend(item[2:].split(":"))
        elif item.startswith("-l"):
            libraries.append(item[2:])
        else:
            extra_link_args.append(item)
    return {
        "include_dirs": include_dirs,
        "library_dirs": library_dirs,
        "libraries": libraries,
        "extra_link_args": extra_link_args,
    }


class PyBind11Include:
    def __str__(self):
        import pybind11

        return pybind11.get_include()


geos_paths = get_geos_paths()

sources = ['pybind/source/pybind.cpp']
for root, dirnames, filenames in os.walk('./source'):
    for filename in fnmatch.filter(filenames, '*.cpp'):
        sources.append(os.path.join(root, filename))

include_dirs = ['include', 'pybind/include', PyBind11Include()] + geos_paths.get("include_dirs", [])
library_dirs = geos_paths.get("library_dirs", [])
libraries = geos_paths.get("libraries", [])

extra_compile_args = []
if platform.system() == "Windows":
    extra_compile_args.append('/std:c++14')
else:
    extra_compile_args.append('-std=c++14')
    extra_compile_args.append('-g0')

extra_link_args = geos_paths.get("extra_link_args", [])

setup(
    ext_modules=[
        Extension(
            'pymeos',
            sources,
            include_dirs=include_dirs,
            library_dirs=library_dirs,
            libraries=libraries,
            extra_compile_args=extra_compile_args,
            extra_link_args=extra_link_args,
            language='c++',
        ),
    ],
)
