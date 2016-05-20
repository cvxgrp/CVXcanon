#!/bin/bash -eux
#
# Script for running continuous integration tests

# build/install cvxcanon
cd $HOME/CVXcanon
tools/build_third_party.sh
python setup.py install

# install cvxpy
cd $HOME/cvxpy
python setup.py install

# run tests
nosetests cvxcanon
nosetests cvxpy
