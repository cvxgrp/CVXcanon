// Specifical of cone problems and interface for cone solvers.

#ifndef CVXCANON_SOLVER_CONE_CONE_SOLVER_H
#define CVXCANON_SOLVER_CONE_CONE_SOLVER_H

#include <vector>

#include "Eigen/Core"
#include "Eigen/Sparse"
#include "cvxcanon/solver/SolverStatus.hpp"
#include "cvxcanon/util/Utils.hpp"

// A cone constraint
class ConeConstraint {
 public:
  enum Cone {
    ZERO,
    NON_NEGATIVE,
    SECOND_ORDER,
    EXPONENTIAL,
  };
  Cone cone;
  int offset, size;
};

// A cone problem of the form
// minimize    c'x
// subject to  b - Ax in K
class ConeProblem {
 public:
  SparseMatrix A;
  DenseVector b, c;
  std::vector<ConeConstraint> constraints;
};

// The solution to a cone problem
class ConeSolution {
 public:
  SolverStatus status;

  // Primal and dual variables
  DenseVector x, y;

  // Primal objective value
  double objective_value;
};

// The cone solver interface.
class ConeSolver {
 public:
  virtual ~ConeSolver() {}
  virtual ConeSolution solve(const ConeProblem& problem) = 0;
};

#endif  // CVXCANON_SOLVER_CONE_CONE_SOLVER_H
