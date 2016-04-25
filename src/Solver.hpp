// Interface for all solvers

#ifndef SOLVER_H
#define SOLVER_H

#include <unordered_map>

#include <Eigen/Core>

#include "Expression.hpp"

class SolverOptions {
};

class Solution {
 public:
  std::unordered_map<int, Eigen::VectorXd> values;
};

class Solver {
 public:
  virtual ~Solver() {}
  virtual Solution solve(const Problem& problem) = 0;
};

#endif  // SOLVER_H
