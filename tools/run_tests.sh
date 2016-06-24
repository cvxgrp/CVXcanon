#!/bin/bash -u
#
# Simple script for running tests from makefile and making sure they all pass
#
# Usage: run_tests.sh test1 ... testN

system=$(uname -s)
if [ $system == "Darwin" ]; then
    ld_library_path=DYLD_LIBRARY_PATH
else
    ld_library_path=LD_LIBRARY_PATH
fi
export ld_library_path

run_test() {
    echo $1:
    export $ld_library_path=$PWD/build-deps/lib
    $1
}
export -f run_test

export SHELL=/bin/bash
parallel run_test ::: $*

if [ $? -eq 0 ]; then
    echo "ALL TESTS PASSED"
else
    echo "TESTS FAILED"
    exit 1
fi
