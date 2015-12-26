#!/bin/bash -x
(cd ../include/ecos/ && make)
g++ -o solvertest -I../include/ecos/include -I../include/ecos/external/SuiteSparse_config/ -L../include/ecos/ -lecos Solvers.cpp 