// Specification of the AST-based solver interface.

#ifndef CVXCANON_SOLVER_SOLVER_H
#define CVXCANON_SOLVER_SOLVER_H

#include <map>

#include "Eigen/Core"
#include "cvxcanon/expression/Expression.hpp"
#include "cvxcanon/solver/SolverStatus.hpp"

class SolverOptions {
  // TODO(mwytock): Add either a generic set of options for solvers and/or a way
  // for solver-specific options to be passed.
};

// A solution
class Solution {
 public:
  SolverStatus status;

  // The values of variables at the solution.
  std::map<int, DenseVector> variable_values;
  double objective_value;

  // TODO(mwytock): Also include dual variables somehow?
};

// The solver interface
class Solver {
 public:
  virtual ~Solver() {}
  virtual Solution solve(const Problem& problem) = 0;
};

// Solve an optimization problem specified in AST form.
Solution solve(const Problem& problem, const SolverOptions& solver_options);

// Checks if we can solve an optimization problem with the given solver options
bool validate(const Problem& problem, const SolverOptions& solver_options);

#endif  // CVXCANON_SOLVER_SOLVER_H
