#!/bin/bash -eux
#
# For development purposes, recompile core CVXcanon code and run tests. Intended
# to be run from root of repository.

# Use ccache to speed up compilation
# TODO(mwytock): Specific to OSX, Linux version?
export CC="ccache clang"
export CXX="ccache clang"

# Run swig
swig -python -c++ -Isrc/cpp src/python/CVXcanon.i
mv src/python/CVXcanon_wrap.cxx src/python/CVXcanon_wrap.cpp

# Build
python setup.py build
python setup.py -q develop --user

# Test
nosetests cvxcanon
