#ifndef CVXCANON_H
#define CVXCANON_H

#include <vector>
#include "LinOp.hpp"
#include "Utils.hpp"
#include "ProblemData.hpp"

int sum(int a);

// Top Level Entry point
ProblemData build_matrix(std::vector< LinOp* > constraints);

#endif