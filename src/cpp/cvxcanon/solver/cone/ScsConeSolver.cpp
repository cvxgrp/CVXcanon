
#include "cvxcanon/solver/cone/ScsConeSolver.hpp"

#include <unordered_map>
#include <vector>

#include "cvxcanon/util/MatrixUtil.hpp"

namespace scs {
#include "scs/include/scs.h"
#include "scs/include/util.h"
#include "scs/linsys/amatrix.h"
typedef double scs_float;
typedef int scs_int;
}  // namespace scs

struct ScsConeSolver::ScsData {
  // SCS data structures
  scs::Data data_;
  scs::Cone cone_;
  scs::Info info_;
  scs::Sol sol_;
  scs::Settings settings_;

  // SCS supporting data structures
  scs::AMatrix A_matrix_;
};

ScsConeSolver::ScsConeSolver() : scs_data_(new ScsData) {}

ScsConeSolver::~ScsConeSolver() {}

void ScsConeSolver::build_scs_constraint(
    const Eigen::SparseMatrix<double, Eigen::RowMajor>& A,
    const DenseVector& b,
    const std::vector<ConeConstraint>& constraints,
    int* total_size,
    int* sizes) {
  int j = 0;
  for (const ConeConstraint& constr : constraints) {
    append_block_triplets(
        A.middleRows(constr.offset, constr.size), num_constrs_, 0, &A_coeffs_);
    b_.segment(num_constrs_, constr.size) =
        b.segment(constr.offset, constr.size);
    if (total_size != nullptr) *total_size += constr.size;
    if (sizes != nullptr) sizes[j++] = constr.size;
    num_constrs_ += constr.size;
  }
}

void ScsConeSolver::build_scs_problem(
    const ConeProblem& problem,
    ConeSolution* solution) {
  const int m = problem.A.rows();
  const int n = problem.A.cols();

  // SCS expects the contraints to be specified in a certain order based on cone
  // type, whereas the ConeProblem interface is more flexible. In this block, we
  // shuffle the constraints so that they are in the order required by SCS and
  // at the same time build the SCS cone data structure which contains the sizes
  // of each constraint.
  //
  // Inputs: problem.A, problem.b, wwith  original constraints
  // Outputs: A_, b_, with shuffled rows
  {
    Eigen::SparseMatrix<double, Eigen::RowMajor> A = problem.A;
    const DenseVector& b = problem.b;

    A_coeffs_.clear();
    b_ = DenseVector(m);
    num_constrs_ = 0;

    std::unordered_map<int, std::vector<ConeConstraint>> constr_map;
    for (const ConeConstraint& constr : problem.constraints) {
      constr_map[constr.cone].push_back(constr);
    }

    build_scs_constraint(
        A, b, constr_map[ConeConstraint::ZERO], &scs_data_->cone_.f, nullptr);
    build_scs_constraint(
        A, b, constr_map[ConeConstraint::NON_NEGATIVE], &scs_data_->cone_.l,
        nullptr);
    scs_data_->cone_.qsize = constr_map[ConeConstraint::SECOND_ORDER].size();
    if (scs_data_->cone_.qsize != 0) {
      cone_q_.reset(new int[scs_data_->cone_.qsize]);
      build_scs_constraint(
          A, b, constr_map[ConeConstraint::SECOND_ORDER], nullptr,
          cone_q_.get());
      scs_data_->cone_.q = cone_q_.get();
    }
    scs_data_->cone_.ssize = constr_map[ConeConstraint::SEMIDEFINITE].size();
    if (scs_data_->cone_.ssize != 0) {
      cone_s_.reset(new int[scs_data_->cone_.ssize]);
      build_scs_constraint(
          A, b, constr_map[ConeConstraint::SEMIDEFINITE], nullptr,
          cone_s_.get());
      scs_data_->cone_.s = cone_s_.get();

      // SCS expects the matrix dimension for each SDP constraint, so we invert
      // n*(n+1)/2 to get the matrix dimension from the number of constraints.
      for (int i = 0; i < scs_data_->cone_.ssize; i++)
        cone_s_[i] = symmetric_single_dim(cone_s_[i]);
    }
    build_scs_constraint(
        A, b, constr_map[ConeConstraint::EXPONENTIAL], &scs_data_->cone_.ep, nullptr);
    CHECK_EQ(scs_data_->cone_.ep % 3, 0);
    scs_data_->cone_.ep /= 3;  // SCS expects the total number of 3-tuples

    scs_data_->cone_.ed = 0;
    scs_data_->cone_.psize = 0;

    A_ = sparse_matrix(m, n, A_coeffs_);

    VLOG(1) << "SCS constraints:\n"
            << "A:\n" << matrix_debug_string(A_)
            << "b: " << vector_debug_string(b_);
  }

  // Now we fill in the rest of the structs that make up the SCS interface.
  scs_data_->A_matrix_.m = m;
  scs_data_->A_matrix_.n = n;
  scs_data_->A_matrix_.p = A_.outerIndexPtr();
  scs_data_->A_matrix_.i = A_.innerIndexPtr();
  scs_data_->A_matrix_.x = A_.valuePtr();

  scs_data_->data_.m = m;
  scs_data_->data_.n = n;
  scs_data_->data_.A = &scs_data_->A_matrix_;
  scs_data_->data_.b = const_cast<double*>(b_.data());
  scs_data_->data_.c = const_cast<double*>(problem.c.data());
  scs_data_->data_.stgs = &scs_data_->settings_;
  scs::setDefaultSettings(&scs_data_->data_);
  scs_data_->settings_.verbose = 0;

  s_ = DenseVector(A_.rows());
  solution->x = DenseVector(A_.cols());
  solution->y = DenseVector(A_.rows());
  scs_data_->sol_.x = const_cast<double*>(solution->x.data());
  scs_data_->sol_.y = const_cast<double*>(solution->y.data());
  scs_data_->sol_.s = const_cast<double*>(s_.data());
}

SolverStatus ScsConeSolver::get_scs_status() {
  if (strcmp(scs_data_->info_.status, "Solved") == 0) {
    return OPTIMAL;
  } else if (strcmp(scs_data_->info_.status, "Infeasible") == 0) {
    return INFEASIBLE;
  } else if (strcmp(scs_data_->info_.status, "Unbounded") == 0) {
    return UNBOUNDED;
  } else {
    return ERROR;
  }
}

ConeSolution ScsConeSolver::solve(const ConeProblem& problem) {
  ConeSolution solution;
  build_scs_problem(problem, &solution);
  scs::scs(
      &scs_data_->data_,
      &scs_data_->cone_,
      &scs_data_->sol_,
      &scs_data_->info_);
  solution.objective_value = scs_data_->info_.pobj;
  solution.status = get_scs_status();
  return solution;
}
