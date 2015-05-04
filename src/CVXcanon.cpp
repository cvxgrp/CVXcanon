#include "CVXcanon.hpp"
#include <iostream>
#include <map>
#include "utils.hpp"
#include "LinOp.hpp"

Matrix get_coefficient(LinOp &lin){
	return NULL;
}

Matrix build_matrix(CoeffMap &coeff_map){
	return NULL;
}

Matrix get_matrix(std::vector< LinOp* > constraints){
	// TODO

	// Make Map
	CoeffMap coeff_map;

	// Loop over constraints, for each:
		// Call get coefficient
		// add coefficient to map with constr. id

	// Now have map, then call build_matrix
	return NULL;
}

int main()
{
  std::cout << "Hello World!\n";
}