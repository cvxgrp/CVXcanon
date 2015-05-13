#ifndef LINOP_H
#define LINOP_H

#include <vector>
#include <map>
#include "Utils.hpp"


std::map<int,Matrix> get_variable_coeffs(LinOp &lin);
std::map<int,Matrix> get_const_coeffs(LinOp &lin);
std::vector<Matrix> get_func_coeffs(LinOp& lin);

#endif