#!/bin/bash -eu

base=$(dirname "${BASH_SOURCE[0]}")/..

cpplint=$base/third_party/cpplint/cpplint.py
$cpplint --extensions cpp,hpp $(find $base/src -name *.cpp -o -name *.hpp)
