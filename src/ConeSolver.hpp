// Interface for cone solvers

#ifndef CONE_SOLVER_H
#define CONE_SOLVER_H

#include <Eigen/Core>
#include <Eigen/Sparse>

enum Cone {
  ZERO,
  NON_NEGATIVE,
  SECOND_ORDER,
};

class ConeIndices {
 public:
  Cone cone;
  int offset, size;
};

class ConeProblem {
 public:
  Eigen::SparseMatrix<double> A;
  Eigen::VectorXd b, c;
  std::vector<ConeIndices> cones;
};

class ConeSolution {
 public:
  Eigen::VectorXd x, y;
};

class ConeSolver {
 public:
  virtual ~ConeSolver() {}
  virtual ConeSolution solve(const ConeProblem& problem) = 0;
};

#endif  // CONE_SOLVER_H
