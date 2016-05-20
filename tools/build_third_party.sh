#!/bin/bash -eu
#
# Build third_party dependencies
#
# TODO(mwytock): glog should go away or be better integrated into build process
# TODO(mwytock): third_party solvers should go away to be replaced by dynamic
# loading.

third_party=$PWD/third_party
build=$PWD/build-deps

mkdir -p $build

cmake_flags="-DCMAKE_INSTALL_PREFIX=$build"
if [ "$(uname -s)" == "Linux" ]; then
    cmake_flags+=" -DCMAKE_CXX_FLAGS=-fPIC"
    export CXXFLAGS=-fPIC
fi

# pull submodules
git submodule update --init

# glog
mkdir -p $build/glog
cd $build/glog
cmake $cmake_flags -DWITH_GFLAGS=OFF $third_party/glog
make -j install

# SCS
mkdir -p $build/scs
cd $build/scs
make -C $third_party/scs OUT=$build/scs
cp $build/scs/*.a $build/lib
