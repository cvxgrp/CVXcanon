// Interface to the splitting conic solver (SCS)
//
// See: https://github.com/cvxgrp/scs
//
// TODO(mwytock): It is expected that this interface will be implemented
// directly in the SCS code base at some point and called via a plugin
// architecture, see comment about pure C interface in README.md.

#ifndef CVXCANON_SOLVER_CONE_SCS_CONE_SOLVER_H
#define CVXCANON_SOLVER_CONE_SCS_CONE_SOLVER_H

#include <memory>
#include <vector>

#include "cvxcanon/solver/cone/ConeSolver.hpp"


class ScsConeSolver : public ConeSolver {
 public:
  ScsConeSolver();
  ~ScsConeSolver();

  ConeSolution solve(const ConeProblem& problem) override;

 private:
  struct ScsData;

  void build_scs_problem(
      const ConeProblem& problem,
      ConeSolution* solution);

  void build_scs_constraint(
    const Eigen::SparseMatrix<double, Eigen::RowMajor>& A,
    const DenseVector& b,
    const std::vector<ConeConstraint>& constraints,
    int* total_size,
    int* sizes);

  SolverStatus get_scs_status();

  // SCS data structures
  std::unique_ptr<ScsData> scs_data_;

  // SCS supporting data structures
  std::unique_ptr<int[]> cone_q_, cone_s_;
  DenseVector s_;

  // Constraints ordered the way SCS needs them
  SparseMatrix A_;
  DenseVector b_;

  // Used for building constraints
  int num_constrs_;
  std::vector<Triplet> A_coeffs_;
};

#endif  // CVXCANON_SOLVER_CONE_SCS_CONE_SOLVER_H
