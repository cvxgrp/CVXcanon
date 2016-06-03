#!/bin/bash -eux
#
# Script for running continuous integration tests

base=$(dirname "${BASH_SOURCE[0]}")/..

# build/install cvxcanon
cd $base
tools/build_third_party.sh
python setup.py install

# install cvxpy
cd $base/../cvxpy
python setup.py install

# run tests
nosetests cvxcanon
nosetests cvxpy

# run c++ linter
$base/tools/run_lint.sh
