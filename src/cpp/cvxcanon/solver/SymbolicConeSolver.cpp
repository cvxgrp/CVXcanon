
#include "SymbolicConeSolver.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

#include "cvxcanon/expression/ExpressionShape.hpp"
#include "cvxcanon/expression/ExpressionUtil.hpp"
#include "cvxcanon/expression/LinearExpression.hpp"
#include "cvxcanon/util/MatrixUtil.hpp"
#include "cvxcanon/util/Utils.hpp"
#include "glog/logging.h"

// Incrementally constructs the the coefficients for the objective and
// constraints of the cone problem.
class ConeProblemBuilder {
 public:
  explicit ConeProblemBuilder(VariableOffsetMap* var_offsets)
      : var_offsets_(var_offsets),
        m_(0) {}

  void add_objective(const Expression& expr);
  void add_constraint(const Expression& expr);

  // Specializations of add_constraint for each type of constraint expression
  void add_eq_constraint(const Expression& expr);
  void add_leq_constraint(const Expression& expr);
  void add_soc_constraint(const Expression& expr);
  void add_exp_cone_constraint(const Expression& expr);

  // Helper function that adds the coefficients for a constraint expression to
  // A, b matrices.
  int add_constraint_coefficients(const Expression& expr);

  const ConeProblem& build() {
    const int m = m_;
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

  int m_;
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

void ConeProblemBuilder::add_eq_constraint(const Expression& expr) {
  // x == y becomes y - x in K_0
  const int offset = m_;
  const int size = add_constraint_coefficients(
      add(neg(expr.arg(0)), expr.arg(1)));
  cone_problem_.constraints.push_back({ConeConstraint::ZERO, offset, size});
}

void ConeProblemBuilder::add_leq_constraint(const Expression& expr) {
  // x <= y becomes y - x in K_+
  const int offset = m_;
  const int size = add_constraint_coefficients(
      add(neg(expr.arg(0)), expr.arg(1)));
  cone_problem_.constraints.push_back(
      {ConeConstraint::NON_NEGATIVE, offset, size});
}

void ConeProblemBuilder::add_soc_constraint(const Expression& expr) {
  // ||x||_2 <= y becomes (y,x) in K_soc
  const int offset = m_;
  int size = 0;
  size += add_constraint_coefficients(expr.arg(1));
  size += add_constraint_coefficients(expr.arg(0));
  cone_problem_.constraints.push_back(
      {ConeConstraint::SECOND_ORDER, offset, size});
}

void ConeProblemBuilder::add_exp_cone_constraint(const Expression& expr) {
  // Elementwise exponential cone is encoded as 3-tuples of (x,y,z)
  CHECK_EQ(expr.args().size(), 3);
  std::vector<CoeffMap> coeff_maps;
  for (const Expression& arg : expr.args())
    coeff_maps.push_back(get_coefficients(arg));

  for (int i = 0; i < dim(expr.arg(0)); i++) {
    cone_problem_.constraints.push_back({ConeConstraint::EXPONENTIAL, m_, 3});
    for (const CoeffMap& coeff_map : coeff_maps) {
      for (const auto& iter : coeff_map) {
        const int var_id = iter.first;
        const SparseMatrix& A = iter.second;
        if (var_id == kConstCoefficientId) {
          append_block_triplets(A.row(i), m_, 0, &b_coeffs_);
        } else {
          const int j = var_offsets_->insert(var_id, A.cols());
          append_block_triplets(A.row(i), m_, j, &A_coeffs_);
        }
      }
      m_++;
    }
  }
}

typedef void(ConeProblemBuilder::*ConstraintHandler)(const Expression& expr);
const std::unordered_map<int, ConstraintHandler> kConstraintHandlers = {
  {Expression::EQ,  &ConeProblemBuilder::add_eq_constraint},
  {Expression::LEQ, &ConeProblemBuilder::add_leq_constraint},
  {Expression::SOC, &ConeProblemBuilder::add_soc_constraint},
  {Expression::EXP_CONE, &ConeProblemBuilder::add_exp_cone_constraint},
};

void ConeProblemBuilder::add_constraint(const Expression& expr) {
  auto iter = kConstraintHandlers.find(expr.type());
  CHECK(iter != kConstraintHandlers.end());
  (this->*iter->second)(expr);
}

int ConeProblemBuilder::add_constraint_coefficients(const Expression& expr) {
  CoeffMap coeff_map = get_coefficients(expr);
  const int m = dim(expr);

  for (const auto& iter : coeff_map) {
    const int var_id = iter.first;
    const SparseMatrix& A = iter.second;
    if (var_id == kConstCoefficientId) {
      append_block_triplets(A, m_, 0, &b_coeffs_);
    } else {
      const int j = var_offsets_->insert(var_id, A.cols());
      append_block_triplets(A, m_, j, &A_coeffs_);
    }
  }

  m_ += m;
  return m;
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
