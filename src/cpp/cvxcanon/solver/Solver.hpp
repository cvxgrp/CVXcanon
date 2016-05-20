// Interface for all solvers

#ifndef SOLVER_H
#define SOLVER_H

#include <map>

#include <Eigen/Core>

#include "cvxcanon/expression/Expression.hpp"
#include "cvxcanon/solver/SolverStatus.hpp"

class SolverOptions {
};

class Solution {
 public:
  SolverStatus status;
  std::map<int, DenseVector> variable_values;
  double objective_value;

};

class Solver {
 public:
  virtual ~Solver() {}
  virtual Solution solve(const Problem& problem) = 0;
};

Solution solve(const Problem& problem, const SolverOptions& solver_options);

#endif  // SOLVER_H
