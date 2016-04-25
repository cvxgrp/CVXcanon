// Interface to the splitting conic solver (SCS)
//
// See: https://github.com/cvxgrp/scs

#ifndef SPLITTING_CONIC_SOLVER_H
#define SPLITTING_CONIC_SOLVER_H

#include "ConeSolver.hpp"

namespace scs {
typedef double scs_float;
typedef int scs_int;
#include <linsys/amatrix.h>
#include <scs.h>
}  // scs

class SplittingConeSolver : public ConeSolver {
 public:
  ConeSolution solve(const ConeProblem& problem) override;

 private:
  void build_scs_problem(const ConeProblem& problem);
  ConeSolution build_cone_solution();

  // SCS data structures
  scs::Data data_;
  scs::Cone cone_;
  scs::Info info_;
  scs::Sol sol_;
  scs::AMatrix A_matrix_;

  // Constraints ordered the way SCS needs them
  Eigen::SparseMatrix<double> A_;
  Eigen::VectorXd b_;
};

#endif  // SPLITTING_CONIC_SOLVER_H
