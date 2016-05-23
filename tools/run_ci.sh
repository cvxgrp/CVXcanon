#!/bin/bash -eux
#
# Script for running continuous integration tests

build=/build

# build/install cvxcanon
cd $build/CVXcanon
tools/build_third_party.sh
python setup.py install

# install cvxpy
cd $build/cvxpy
python setup.py install

# run tests
nosetests cvxcanon
nosetests cvxpy
