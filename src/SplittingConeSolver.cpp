
#include "SplittingConeSolver.hpp"

#include <unordered_map>

#include "MatrixUtil.hpp"

void SplittingConeSolver::build_scs_constraint(
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
  }
}

void SplittingConeSolver::build_scs_problem(
    const ConeProblem& problem,
    ConeSolution* solution) {
  Eigen::SparseMatrix<double, Eigen::RowMajor> A = problem.A;
  const DenseVector& b = problem.b;

  std::unordered_map<int, std::vector<ConeConstraint>> constr_map;
  for (const ConeConstraint& constr : problem.constraints) {
    constr_map[constr.cone].push_back(constr);
  }

  build_scs_constraint(
      A, b, constr_map[ConeConstraint::ZERO], &cone_.f, nullptr);
  build_scs_constraint(
      A, b, constr_map[ConeConstraint::NON_NEGATIVE], &cone_.l, nullptr);

  cone_.qsize = constr_map[ConeConstraint::SECOND_ORDER].size();
  if (cone_.qsize != 0) {
    q_.reset(new int[cone_.qsize]);
    build_scs_constraint(
        A, b, constr_map[ConeConstraint::SECOND_ORDER], nullptr, q_.get());
  }
  cone_.ssize = 0;
  cone_.ep = 0;
  cone_.ed = 0;
  cone_.psize = 0;

  // Construct A matrix
  A_ = Matrix(problem.A.rows(), problem.A.cols());
  A_.setFromTriplets(A_coeffs_.begin(), A_coeffs_.end());

  A_matrix_.m = A_.rows();
  A_matrix_.n = A_.cols();
  A_matrix_.x = A_.valuePtr();
  A_matrix_.p = A_.outerIndexPtr();
  A_matrix_.i = A_.innerIndexPtr();

  data_.m = A_.rows();
  data_.n = A_.cols();
  data_.A = &A_matrix_;
  data_.b = const_cast<double*>(b_.data());
  data_.c = const_cast<double*>(problem.c.data());

  s_ = DenseVector(A_.rows());
  solution->x = DenseVector(A_.cols());
  solution->y = DenseVector(A_.rows());
  sol_.x = const_cast<double*>(solution->x.data());
  sol_.y = const_cast<double*>(solution->y.data());
  sol_.s = const_cast<double*>(s_.data());
}

ConeSolution SplittingConeSolver::solve(const ConeProblem& problem) {
  ConeSolution solution;
  build_scs_problem(problem, &solution);
  scs::scs(&data_, &cone_, &sol_, &info_);
  return solution;
}
