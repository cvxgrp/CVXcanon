#ifndef CVXCANON_H
#define CVXCANON_H

#include <vector>
#include "LinOp.hpp"
#include "Utils.hpp"
#include "ProblemData.hpp"

// Top Level Entry point
ProblemData build_matrix(std::vector< LinOp* > constraints, std::map<int, int> id_to_col);
#endif