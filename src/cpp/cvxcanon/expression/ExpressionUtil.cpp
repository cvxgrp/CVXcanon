
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

Expression quad_over_lin(Expression x, Expression y) {
  return {Expression::QUAD_OVER_LIN, {x, y}};
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

Expression hstack(std::vector<Expression> args) {
  return {Expression::HSTACK, args};
}

Expression vstack(std::vector<Expression> args) {
  return {Expression::VSTACK, args};
}

Expression reshape(Expression x, int m, int n) {
  auto attr = std::make_shared<ReshapeAttributes>();
  attr->size = {{m, n}};
  return {Expression::RESHAPE, {x}, attr};
}

Expression sum_entries(Expression x) {
  return {Expression::SUM_ENTRIES, {x}};
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

Expression epi_var(const Expression& x, const std::string& name) {
  Size size_x = size(x);
  int var_id = rand();  // NOLINT(runtime/threadsafe_fn)
  VLOG(2) << "epi_var " << var_id << ", "
          << size_x.dims[0] << " x " << size_x.dims[1];
  return var(size_x.dims[0], size_x.dims[1], var_id);
}

bool is_scalar(const Size& size) {
  return size.dims[0] == 1 && size.dims[1] == 1;
}

std::unordered_set<int> kConstraintTypes = {
  Expression::EQ,
  Expression::LEQ,
  Expression::SOC,
  Expression::EXP_CONE,
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
