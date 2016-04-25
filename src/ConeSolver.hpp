// Interface for cone solvers

#ifndef CONE_SOLVER_H
#define CONE_SOLVER_H

#include <Eigen/Core>
#include <Eigen/Sparse>

#include "Utils.hpp"

class ConeConstraint {
 public:
  enum Cone {
    ZERO,
    NON_NEGATIVE,
    SECOND_ORDER,
  };

  Cone cone;
  int offset, size;
};

class ConeProblem {
 public:
  Matrix A;
  VectorFloat b, c;
  std::vector<ConeConstraint> constraints;
};

class ConeSolution {
 public:
  VectorFloat x, y;
};

class ConeSolver {
 public:
  virtual ~ConeSolver() {}
  virtual ConeSolution solve(const ConeProblem& problem) = 0;
};

#endif  // CONE_SOLVER_H
