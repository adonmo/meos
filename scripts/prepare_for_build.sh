#!/bin/bash

# This script is used to setup GEOS before we build wheels
# Also refer: https://trac.osgeo.org/geos/wiki/BuildingOnUnixWithCMake

# Get latest cmake
pip install cmake

# Clone, configure, build and install GEOS 3.8
git clone https://github.com/libgeos/geos.git --branch 3.8 --depth=1
cmake -B build -S geos -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --config Release
