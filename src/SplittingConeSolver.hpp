// Interface to the splitting conic solver (SCS)
//
// See: https://github.com/cvxgrp/scs

#ifndef SPLITTING_CONIC_SOLVER_H
#define SPLITTING_CONIC_SOLVER_H

#include "ConeSolver.hpp"

#include <scs.h>

class SplittingConeSolver : public ConeSolver {
 public:
  ConeSolution solve(const ConeProblem& problem) override;

 private:
  void build_scs_problem(const ConeProblem& problem);
  ConeSolution build_solution();

  // SCS data structures
  SCS_PROBLEM_DATA data_;
  SCS_CONE cone_;
  SCS_INFO info_;
  SCS_SOL_VARS_ sol_;
  A_DATA_MATRIX A_matrix_;

  // Constraints ordered the way SCS needs them
  Eigen::SparseMatrix<double> A_;
  Eigen::VectorXd b_;
};

#endif  // SPLITTING_CONIC_SOLVER_H
