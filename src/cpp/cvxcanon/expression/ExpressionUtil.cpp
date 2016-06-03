
#include "cvxcanon/expression/ExpressionUtil.hpp"

#include <stdlib.h>

#include <string>
#include <vector>
#include <unordered_set>

#include "cvxcanon/expression/ExpressionShape.hpp"

Expression add(Expression x, Expression y) {
  return {Expression::ADD, {x, y}};
}

Expression mul(Expression x, Expression y) {
  return {Expression::MUL, {x, y}};
}

Expression neg(Expression x) {
  return {Expression::NEG, {x}};
}

Expression abs(Expression x) {
  return {Expression::ABS, {x}};
}

Expression transpose(Expression x) {
  return {Expression::TRANSPOSE, {x}};
}

Expression log(Expression x) {
  return {Expression::LOG, {x}};
}

Expression exp(Expression x) {
  return {Expression::EXP, {x}};
}

Expression upper_tri(Expression x) {
  return {Expression::UPPER_TRI, {x}};
}

Expression diag_vec(Expression x) {
  return {Expression::DIAG_VEC, {x}};
}

Expression diag_mat(Expression A) {
  return {Expression::DIAG_MAT, {A}};
}

Expression trace(Expression A) {
  return {Expression::TRACE, {A}};
}

Expression quad_over_lin(Expression x, Expression y) {
  return {Expression::QUAD_OVER_LIN, {x, y}};
}

Expression hstack(std::vector<Expression> args) {
  return {Expression::HSTACK, args};
}

Expression vstack(std::vector<Expression> args) {
  return {Expression::VSTACK, args};
}

Expression p_norm(Expression x, double p) {
  auto attr = std::make_shared<PNormAttributes>();
  attr->p = p;
  return {Expression::P_NORM, {x}, attr};
}

Expression power(Expression x, double p) {
  auto attr = std::make_shared<PowerAttributes>();
  attr->p = p;
  return {Expression::POWER, {x}, attr};
}

Expression reshape(Expression x, int m, int n) {
  auto attr = std::make_shared<ReshapeAttributes>();
  attr->size = {{m, n}};
  return {Expression::RESHAPE, {x}, attr};
}

Expression sum_entries(Expression x, int axis) {
  auto attr = std::make_shared<SumEntriesAttributes>();
  attr->axis = axis;
  return {Expression::SUM_ENTRIES, {x}, attr};
}

Expression index(
    Expression x, int start_i, int stop_i, int start_j, int stop_j) {
  auto attr = std::make_shared<IndexAttributes>();
  attr->keys.push_back({start_i, stop_i, 1});
  attr->keys.push_back({start_j, stop_j, 1});
  return {Expression::INDEX, {x}, attr};
}

Expression var(int m, int n, int var_id) {
  auto attr = std::make_shared<VarAttributes>();
  attr->id = var_id;
  attr->size = {{m, n}};
  return {Expression::VAR, {}, attr};
}

Expression constant(double value) {
  return constant(DenseMatrix::Constant(1, 1, value));
}

Expression constant(DenseMatrix value) {
  auto attr = std::make_shared<ConstAttributes>();
  attr->dense_data = value;
  return {Expression::CONST, {}, attr};
}

Expression eq(Expression x, Expression y) {
  return {Expression::EQ, {x, y}};
}

Expression leq(Expression x, Expression y) {
  return {Expression::LEQ, {x, y}};
}

Expression soc(Expression x, Expression y) {
  return {Expression::SOC, {x, y}};
}

Expression exp_cone(Expression x, Expression y, Expression z) {
  return {Expression::EXP_CONE, {x, y, z}};
}

Expression sdp(Expression x) {
  return {Expression::SDP, {x}};
}

Expression epi_var(const Expression& x, const std::string& name) {
  return epi_var_size(x, name, size(x));
}

Expression epi_var_size(
    const Expression& x, const std::string& name, Size size) {
  int var_id = rand();  // NOLINT(runtime/threadsafe_fn)
  VLOG(2) << "epi_var " << var_id << ", "
          << size.dims[0] << " x " << size.dims[1];
  return var(size.dims[0], size.dims[1], var_id);
}

bool is_scalar(const Size& size) {
  return size.dims[0] == 1 && size.dims[1] == 1;
}

std::unordered_set<int> kConstraintTypes = {
  Expression::EQ,
  Expression::EXP_CONE,
  Expression::LEQ,
  Expression::SDP,
  Expression::SOC,
};

std::unordered_set<int> kLinearTypes = {
  Expression::ADD,
  Expression::DIAG_MAT,
  Expression::DIAG_VEC,
  Expression::HSTACK,
  Expression::INDEX,
  Expression::KRON,
  Expression::MUL,
  Expression::NEG,
  Expression::RESHAPE,
  Expression::SUM_ENTRIES,
  Expression::TRACE,
  Expression::TRANSPOSE,
  Expression::UPPER_TRI,
  Expression::VSTACK,
};

std::unordered_set<int> kLeafTypes = {
  Expression::CONST,
  Expression::PARAM,
  Expression::VAR,
};

bool is_type(const std::unordered_set<int>& types, const Expression& expr) {
  return types.find(expr.type()) != types.end();
}

bool is_linear(const Expression& expr) {
  return is_type(kLinearTypes, expr);
}

bool is_constraint(const Expression& expr) {
  return is_type(kConstraintTypes, expr);
}

bool is_leaf(const Expression& expr) {
  return is_type(kLeafTypes, expr);
}

Expression promote_add(Expression x, const Size& prom_size) {
  if (is_scalar(size(x)) && !is_scalar(prom_size)) {
    const int m = prom_size.dims[0];
    const int n = prom_size.dims[1];
    DenseMatrix ones = DenseMatrix::Constant(m, n, 1);
    return mul(x, constant(ones));
  }
  return x;
}

Expression promote_multiply(Expression x, int k) {
  if (is_scalar(size(x)) && k != 1) {
    DenseMatrix ones = DenseMatrix::Constant(k, 1, 1);
    return diag_vec(mul(constant(ones), x));
  }
  return x;
}
