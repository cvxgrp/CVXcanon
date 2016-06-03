
#include "SymbolicConeSolver.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

#include "cvxcanon/expression/ExpressionShape.hpp"
#include "cvxcanon/expression/ExpressionUtil.hpp"
#include "cvxcanon/expression/LinearExpression.hpp"
#include "cvxcanon/expression/TextFormat.hpp"
#include "cvxcanon/util/MatrixUtil.hpp"
#include "cvxcanon/util/Utils.hpp"
#include "glog/logging.h"

// Incrementally constructs the the coefficients for the objective and
// constraints of the cone problem.
class ConeProblemBuilder {
 public:
  explicit ConeProblemBuilder(VariableOffsetMap* var_offsets)
      : var_offsets_(var_offsets),
        num_constrs_(0), cone_offset_(0) {}

  void add_objective(const Expression& expr);
  void add_constraint(const Expression& expr);

  // Specializations of add_constraint for each type of constraint expression
  void add_eq_constraint(const Expression& expr);
  void add_exp_cone_constraint(const Expression& expr);
  void add_leq_constraint(const Expression& expr);
  void add_sdp_constraint(const Expression& expr);
  void add_sdp_vec_constraint(const Expression& expr);
  void add_soc_constraint(const Expression& expr);

  // Helper functions for building cone constraints, add the coefficients for
  // rows i, ... i+m to A, b matrices
  void add_constraint_coefficients(const CoeffMap& coeff_map, int i, int m);
  void add_constraint_cone(const ConeConstraint::Cone& cone, int n);

  const ConeProblem& build() {
    CHECK_EQ(num_constrs_, cone_offset_);
    const int m = num_constrs_;
    const int n = var_offsets_->n();

    // negative by convention: b - Ax in K
    cone_problem_.A = -sparse_matrix(m, n, A_coeffs_);
    cone_problem_.b = sparse_matrix(m, 1, b_coeffs_);
    cone_problem_.c = sparse_matrix(n, 1, c_coeffs_);

    VLOG(1) << "created cone problem matrices (m=" << m << ", n=" << n << ")\n"
            << "A:\n" << matrix_debug_string(cone_problem_.A)
            << "b: " << vector_debug_string(cone_problem_.b) << "\n"
            << "c: " << vector_debug_string(cone_problem_.c);

    if (VLOG_IS_ON(1)) {
      VLOG(1) << "cones:";
      for (const ConeConstraint& constr : cone_problem_.constraints) {
        VLOG(1) << "cone " << constr.cone
                << ": (" << constr.offset << ", " << constr.size << ")";
      }
    }
    return cone_problem_;
  }

 private:
  // Not owned
  VariableOffsetMap* var_offsets_;

  int num_constrs_;
  int cone_offset_;
  std::vector<Triplet> A_coeffs_, b_coeffs_, c_coeffs_;
  ConeProblem cone_problem_;
};

void ConeProblemBuilder::add_objective(const Expression& expr) {
  CoeffMap coeff_map = get_coefficients(expr);
  for (const auto& iter : coeff_map) {
    const int var_id = iter.first;
    const SparseMatrix& cT = iter.second;

    if (var_id != kConstCoefficientId) {
      const int j = var_offsets_->insert(var_id, cT.cols());
      append_block_triplets(cT.transpose(), j, 0, &c_coeffs_);
    }
  }
}

// eq(x,y) -> y - x in K_0
void ConeProblemBuilder::add_eq_constraint(const Expression& expr) {
  const Expression& x = expr.arg(0);
  const Expression& y = expr.arg(1);

  const int n = dim(x);
  CoeffMap coeff_map = get_coefficients(add(y, neg(x)));
  add_constraint_cone(ConeConstraint::ZERO, n);
  add_constraint_coefficients(coeff_map, 0, n);
}

// leq(x,y) -> y - x in K_+
void ConeProblemBuilder::add_leq_constraint(const Expression& expr) {
  const Expression& x = expr.arg(0);
  const Expression& y = expr.arg(1);

  const int n = dim(x);
  CoeffMap coeff_map = get_coefficients(add(y, neg(x)));
  add_constraint_cone(ConeConstraint::NON_NEGATIVE, n);
  add_constraint_coefficients(coeff_map, 0, n);
}

// soc(X,y) -> (y_i, x_i) in K_soc for each row of X and element of y
void ConeProblemBuilder::add_soc_constraint(const Expression& expr) {
  const Expression& X = expr.arg(0);
  const Expression& y = expr.arg(1);

  const int m = size(X).dims[0];
  const int n = size(X).dims[1];
  CHECK_EQ(m, size(y).dims[0]);
  CHECK_EQ(1, size(y).dims[1]);

  CoeffMap XT_coeff_map = get_coefficients(transpose(X));
  CoeffMap y_coeff_map = get_coefficients(y);
  for (int i = 0; i < m; i++) {
    add_constraint_cone(ConeConstraint::SECOND_ORDER, n + 1);
    add_constraint_coefficients(y_coeff_map, i, 1);
    add_constraint_coefficients(XT_coeff_map, n*i, n);
  }
}

// exp_cone(x,y,z) -> (x_i, y_i, z_i) in K_exp for each element of x, y, z
void ConeProblemBuilder::add_exp_cone_constraint(const Expression& expr) {
  const Expression& x = expr.arg(0);
  const Expression& y = expr.arg(1);
  const Expression& z = expr.arg(2);

  const int n = dim(x);
  CHECK_EQ(n, dim(y));
  CHECK_EQ(n, dim(z));

  CoeffMap x_coeff_map = get_coefficients(x);
  CoeffMap y_coeff_map = get_coefficients(y);
  CoeffMap z_coeff_map = get_coefficients(z);
  for (int i = 0; i < n; i++) {
    add_constraint_cone(ConeConstraint::EXPONENTIAL, 3);
    add_constraint_coefficients(x_coeff_map, i, 1);
    add_constraint_coefficients(y_coeff_map, i, 1);
    add_constraint_coefficients(z_coeff_map, i, 1);
  }
}

// Linear transform that extracts lower tri + diagonal elements and scales the
// off-diagonal elements by sqrt(2)
SparseMatrix sdp_scaling_matrix(int n) {
  std::vector<Triplet> coeffs;
  int k = 0;
  for (int j = 0; j < n; j++) {
    for (int i = j; i < n; i++) {
      coeffs.push_back(Triplet(k++, n*j+i, i == j ? 1 : sqrt(2)));
    }
  }
  return sparse_matrix(n*(n+1)/2, n*n, coeffs);
}

// Scales the off diagonal elements in a vector representing a symmetric matrix
SparseMatrix sdp_vec_scaling_matrix(int n) {
  std::vector<Triplet> coeffs;
  int k = 0;
  for (int j = 0; j < n; j++) {
    for (int i = j; i < n; i++) {
      coeffs.push_back(Triplet(k, k, i == j ? 1 : sqrt(2)));
      k++;
    }
  }

  const int y = n*(n+1)/2;
  return sparse_matrix(y, y, coeffs);
}

// sdp(X) -> X in K_sdp
void ConeProblemBuilder::add_sdp_constraint(const Expression& expr) {
  const Expression& X = expr.arg(0);
  const int n = size(X).dims[0];

  SparseMatrix F = sdp_scaling_matrix(n);
  CoeffMap coeff_map = get_coefficients(mul(constant(F), reshape(X, n*n, 1)));
  add_constraint_cone(ConeConstraint::SEMIDEFINITE, n*(n+1)/2);
  add_constraint_coefficients(coeff_map, 0, n*(n+1)/2);
}

// sdp_vec(x) -> full(x) in K_sdp
void ConeProblemBuilder::add_sdp_vec_constraint(const Expression& expr) {
  const Expression& x = expr.arg(0);
  const int y = size(x).dims[0];

  SparseMatrix F = sdp_vec_scaling_matrix(symmetric_single_dim(y));
  CoeffMap coeff_map = get_coefficients(mul(constant(F), x));
  add_constraint_cone(ConeConstraint::SEMIDEFINITE, y);
  add_constraint_coefficients(coeff_map, 0, y);
}

typedef void(ConeProblemBuilder::*ConstraintHandler)(const Expression& expr);
const std::unordered_map<int, ConstraintHandler> kConstraintHandlers = {
  {Expression::EQ,  &ConeProblemBuilder::add_eq_constraint},
  {Expression::EXP_CONE, &ConeProblemBuilder::add_exp_cone_constraint},
  {Expression::LEQ, &ConeProblemBuilder::add_leq_constraint},
  {Expression::SDP, &ConeProblemBuilder::add_sdp_constraint},
  {Expression::SDP_VEC, &ConeProblemBuilder::add_sdp_vec_constraint},
  {Expression::SOC, &ConeProblemBuilder::add_soc_constraint},
};

void ConeProblemBuilder::add_constraint(const Expression& expr) {
  VLOG(2) << "add_constraint " << format_expression(expr);
  auto iter = kConstraintHandlers.find(expr.type());
  CHECK(iter != kConstraintHandlers.end())
      << "no handler for " << format_expression(expr);
  (this->*iter->second)(expr);
}

void ConeProblemBuilder::add_constraint_coefficients(
    const CoeffMap& coeff_map, int i, int m) {

  for (const auto& iter : coeff_map) {
    const int var_id = iter.first;
    const SparseMatrix& A = iter.second;
    if (var_id == kConstCoefficientId) {
      append_block_triplets(A.middleRows(i, m), num_constrs_, 0, &b_coeffs_);
    } else {
      const int j = var_offsets_->insert(var_id, A.cols());
      append_block_triplets(A.middleRows(i, m), num_constrs_, j, &A_coeffs_);
    }
  }

  num_constrs_ += m;
}

void ConeProblemBuilder::add_constraint_cone(
    const ConeConstraint::Cone& cone, int n) {
  cone_problem_.constraints.push_back({cone, cone_offset_, n});
  cone_offset_ += n;
}


// Translate ConeSolution to Solution using VariableOffsetMap.
Solution get_solution(
    const ConeSolution& cone_solution,
    const VariableOffsetMap& var_offsets) {
  Solution solution;
  for (const auto& iter : var_offsets.map()) {
    const int var_id = iter.first;
    const int j = iter.second.first;
    const int n = iter.second.second;
    solution.variable_values[var_id] = cone_solution.x.segment(j, n);
  }
  solution.objective_value = cone_solution.objective_value;
  solution.status = cone_solution.status;
  return solution;
}

SymbolicConeSolver::SymbolicConeSolver(
    std::unique_ptr<ConeSolver> cone_solver)
    : cone_solver_(std::move(cone_solver)) {}

Solution SymbolicConeSolver::solve(const Problem& problem) {
  VariableOffsetMap var_offsets;
  ConeProblemBuilder builder(&var_offsets);
  builder.add_objective(problem.objective);
  for (const Expression& constr : problem.constraints)
    builder.add_constraint(constr);

  return get_solution(cone_solver_->solve(builder.build()), var_offsets);
}
