#!/bin/bash -eu
#
# Build solvers in third_party
#
# TODO(mwytock): third_party solvers should go away to be replaced by dynamic
# loading.

# pull submodules
git submodule update --init

# SCS
cd third_party/scs
make
