#!/bin/bash -eux
#
# Script for running continuous integration tests

cd $(dirname "${BASH_SOURCE[0]}")/..

# build third party dependencies
tools/build_third_party.sh

# build and run C++ tests
make -j test

# install python bindings and cvxpy
python setup.py install
( cd ../cvxpy && python setup.py install )

# handle C++ dynamic library loading
system=$(uname -s)
if [ $system == "Darwin" ]; then
    export DYLD_LIBRARY_PATH=$PWD/build-deps/lib
else
    export LD_LIBRARY_PATH=$PWD/build-deps/lib
fi

# run python tests
nosetests cvxcanon
nosetests cvxpy

# run c++ linter
tools/run_lint.sh
