
#include "LinearConeTransform.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "cvxcanon/expression/Expression.hpp"
#include "cvxcanon/expression/ExpressionShape.hpp"
#include "cvxcanon/expression/ExpressionUtil.hpp"
#include "cvxcanon/expression/TextFormat.hpp"
#include "cvxcanon/util/MatrixUtil.hpp"
#include "glog/logging.h"

typedef Expression(*TransformFunction)(
    const Expression& expr,
    std::vector<Expression>* constraints);

Expression transform_abs(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  Expression t = epi_var(expr, "abs");
  constraints->push_back(leq(x, t));
  constraints->push_back(leq(neg(x), t));
  return t;
}

Expression transform_p_norm(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  CHECK_EQ(expr.attr<PNormAttributes>().p, 1);
  return sum_entries(transform_abs(abs(expr.arg(0)), constraints));
}

Expression transform_quad_over_lin(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  const Expression& y = expr.arg(1);
  Expression t = epi_var(expr, "qol");

  constraints->push_back(
      soc(vstack({add(y, neg(t)), mul(constant(2), x)}), add(y, t)));
  constraints->push_back(leq(constant(0), y));
  return t;
}

Expression transform_power(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  LOG(FATAL) << "Not implemented "
             << "(p: " << expr.attr<PowerAttributes>().p << ")";
  return expr;
}

Expression transform_huber(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  const Expression& M = expr.arg(1);
  Expression n = epi_var(expr, "huber_n");
  Expression s = epi_var(expr, "huber_s");

  // n**2, 2*M*|s|
  Expression n2 = transform_power(power(n, 2), constraints);
  Expression abs_s = transform_abs(abs(s), constraints);
  Expression M_abs_s = mul(M, abs_s);
  Expression t = add(n2, mul(constant(2), M_abs_s));

  // x == s + n
  constraints->push_back(eq(x, add(n, s)));
  return t;
}

Expression transform_exp(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  Expression t = epi_var(expr, "exp");
  Expression ones = constant(
      ones_matrix(size(expr).dims[0],
                  size(expr).dims[1]));
  constraints->push_back(exp_cone(x, ones, t));
  return t;
}

std::unordered_map<int, TransformFunction> kTransforms = {
  {Expression::ABS, &transform_abs},
  {Expression::EXP, &transform_exp},
  {Expression::HUBER, &transform_huber},
  {Expression::POWER, &transform_power},
  {Expression::P_NORM, &transform_p_norm},
  {Expression::QUAD_OVER_LIN, &transform_quad_over_lin},
};

Expression transform_expression(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  // First transform children
  std::vector<Expression> linear_args;
  for (const Expression& arg : expr.args())
    linear_args.push_back(transform_expression(arg, constraints));

  // New expression, with linear args
  Expression output = Expression(expr.type(), linear_args, expr.attr_ptr());

  // Now transform non-linear functions, if necessary
  VLOG(2) << "transform_func: " << format_expression(expr);
  auto iter = kTransforms.find(expr.type());
  if (iter != kTransforms.end())
    output = iter->second(output, constraints);

  return output;
}

Problem LinearConeTransform::apply(const Problem& problem) {
  std::vector<Expression> constraints;
  Expression linear_objective = transform_expression(
      problem.objective, &constraints);
  for (const Expression& constr : problem.constraints) {
    constraints.push_back(
        transform_expression(constr, &constraints));
  }
  return {problem.sense, linear_objective, constraints};
}

// Return true if we have a transform for this expression itself
bool have_transform_self(const Expression& expr) {
  if (is_leaf(expr) || is_linear(expr) || is_constraint(expr))
    return true;

  auto iter = kTransforms.find(expr.type());
  if (iter == kTransforms.end()) {
    VLOG(1) << "No transform for " << format_expression(expr);
    return false;
  }

  // Special case for P_NORM and POWER, transforms dependent on p
  // TODO(mwytock): Remove this when we implement the full gm_constrs() logic.
  const std::unordered_set<double> easy_p = {1, 2, INFINITY};
  if (expr.type() == Expression::P_NORM)
    return easy_p.find(expr.attr<PNormAttributes>().p) != easy_p.end();
  if (expr.type() == Expression::POWER)
    return easy_p.find(expr.attr<PowerAttributes>().p) != easy_p.end();

  return true;
}

// Return true if we have a transform this expression and its children
bool have_transform(const Expression& expr) {
  if (!have_transform_self(expr))
    return false;

  for (const Expression& arg : expr.args()) {
    if (!have_transform(arg))
      return false;
  }

  return true;
}

bool LinearConeTransform::accepts(const Problem& problem) {
  for (const Expression& constr : problem.constraints)
    if (!have_transform(constr))
      return false;
  return have_transform(problem.objective);
}
