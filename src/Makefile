

CFLAGS = -I../include/ecos/include -I../include/ecos/external/SuiteSparse_config/ -L../include/ecos/ -lecos
testmake:
	g++ $(CFLAGS) -o solvertest SolversTest.cpp BuildMatrix.cpp LinOpOperations.cpp EcosProblem.cpp CVXcanon.cpp 