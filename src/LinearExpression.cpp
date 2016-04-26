
#include "LinearExpression.hpp"

#include <unordered_map>

#include "ExpressionUtil.hpp"
#include "MatrixUtil.hpp"

bool is_constant(const CoeffMap& coeffs) {
  return (coeffs.find(kConstCoefficientId) != coeffs.end() &&
          coeffs.size() == 1);
}

std::vector<SparseMatrix> get_add_coefficients(const Expression& expr) {
  std::vector<SparseMatrix> coeffs;
  for (int i = 0; i < expr.args().size(); i++)
    coeffs.push_back(scalar_matrix(1));
  return coeffs;
}

std::vector<SparseMatrix> get_neg_coefficients(const Expression& expr) {
  return {scalar_matrix(-1)};
}

std::vector<SparseMatrix> get_sum_coefficients(const Expression& expr) {
  return {ones_matrix(1, dim(expr.arg(0)))};
}

typedef std::vector<SparseMatrix>(*CoefficientFunction)(
    const Expression& expr);

std::unordered_map<int, CoefficientFunction> kCoefficientFunctions = {
  {Expression::ADD, &get_add_coefficients},
  {Expression::NEG, &get_neg_coefficients},
  {Expression::SUM, &get_sum_coefficients},
};


// result += lhs*rhs
void multiply_by_constant(
    const Matrix& lhs,
    const std::map<int, Matrix>& rhs,
    std::map<int, Matrix>* result) {
  for (const auto& iter : rhs) {
    Matrix value = lhs*iter.second;
    auto retval = result->insert(std::make_pair(iter.first, value));
    if (retval.second)
      retval.first->second += value;
  }
}

std::map<int, SparseMatrix> get_coefficients(const Expression& expr) {
  std::map<int, SparseMatrix> coeffs;

  if (expr.type() == Expression::CONST) {
    coeffs[kConstCoefficientId] = to_vector(
        expr.attr<ConstAttributes>().data).sparseView();
  } else if (expr.type() == Expression::VAR) {
    coeffs[expr.attr<VarAttributes>().id] = identity(dim(expr));
  } else if (expr.type() == Expression::MUL) {
    // Special case for binary mul operator which is guaranteed to have one
    // constant argument by DCP rules.
    assert(expr.args().size() == 2);
    CoeffMap lhs_coeffs = get_coefficients(expr.arg(0));
    CoeffMap rhs_coeffs = get_coefficients(expr.arg(1));
    std::vector<SparseMatrix> f_coeffs;
    if (is_constant(lhs_coeffs)) {
      // TODO(mwytock): matrix-matrix multiply not yet supported
      assert(size(expr).dims[1] == 1);
      multiply_by_constant(
          reshape(lhs_coeffs[kConstCoefficientId],
                  size(expr).dims[0],
                  size(expr.arg(1)).dims[0]),
          rhs_coeffs,
          &coeffs);
    } else if (is_constant(rhs_coeffs)) {
      assert(false);
    } else {
      assert(false);
    }
  } else {
    auto iter = kCoefficientFunctions.find(expr.type());
    assert(iter != kCoefficientFunctions.end());
    std::vector<Matrix> f_coeffs = iter->second(expr);
    for (int i = 0; i < expr.args().size(); i++) {
      multiply_by_constant(
          f_coeffs[i], get_coefficients(expr.arg(i)), &coeffs);
    }
  }

  return coeffs;
}
