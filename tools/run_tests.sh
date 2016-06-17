#!/bin/bash -u
#
# Simple script for running tests from makefile and making sure they all pass
#
# Usage: run_tests.sh test1 ... testN

run_test() {
    echo $1:
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
