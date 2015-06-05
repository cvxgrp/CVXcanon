#ifndef LINOPOPERATIONS_H
#define LINOPOPERATIONS_H

#include <vector>
#include <map>
#include "Utils.hpp"
#include "LinOp.hpp"

std::map<int, Matrix> get_variable_coeffs(LinOp &lin);
std::map<int, Matrix> get_const_coeffs(LinOp &lin);
std::vector<Matrix> get_func_coeffs(LinOp& lin);

#endif