#!/bin/bash -eu
#
# Build third_party dependencies
#
# TODO(mwytock): glog should go away or be better integrated into build process
# TODO(mwytock): third_party solvers should go away to be replaced by dynamic
# loading.

third_party=$PWD/third_party
build=$PWD/build-deps
system=$(uname -s)

mkdir -p $build

cmake_flags="-DCMAKE_INSTALL_PREFIX=$build"
if [ $system == "Linux" ]; then
    cmake_flags+=" -DCMAKE_CXX_FLAGS=-fPIC"
    export CXXFLAGS=-fPIC
fi

if [ $system == "Darwin" ]; then
    shared=dylib
else
    shared=so
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
scs_flags="USE_LAPACK=1"
scs_targets=$build/scs/libscsdir.${shared}
make -C $third_party/scs OUT=$build/scs "$scs_flags" "$scs_targets"
cp $build/scs/*.${shared} $build/lib

# ECOS
make -C $third_party/ecos libecos.${shared}
cp $third_party/ecos/libecos.${shared} $build/lib
