
#include "LinearConeTransform.hpp"

#include <unordered_map>

#include "Expression.hpp"
#include "ExpressionUtil.hpp"
#include "Logging.hpp"

typedef Expression(*TransformFunction)(
    const Expression& expr,
    const std::vector<Expression>& args,
    std::vector<Expression>* constraints);

Expression transform_linear(
    const Expression& expr,
    const std::vector<Expression>& args,
    std::vector<Expression>* constraints) {
  Expression t = shallow_copy(expr);
  t.args = args;
  return t;
}

Expression transform_abs(
    const Expression& expr,
    const std::vector<Expression>& args,
    std::vector<Expression>* constraints) {
  Expression x = args[0];
  Expression t = epi_var(expr, "abs");
  constraints->push_back(leq(x, t));
  constraints->push_back(leq(neg(x), t));
  return t;
}

Expression transform_p_norm(
    const Expression& expr,
    const std::vector<Expression>& args,
    std::vector<Expression>* constraints) {
  //CHECK_EQ(expr.p, 1);
  return sum(transform_abs(expr, args, constraints));
}

Expression transform_quad_over_lin(
    const Expression& expr,
    const std::vector<Expression>& args,
    std::vector<Expression>* constraints) {
  Expression x = args[0];
  Expression y = args[1];
  Expression t = scalar_epi_var(expr, "qol");

  constraints->push_back(
      soc(hstack(
          {add(y, neg(t)), reshape(mul(constant(2), x), Size({1, dim(x)}))}),
          add(y, t)));
  constraints->push_back(leq(constant(0), y));
  return t;
}

std::unordered_map<int, TransformFunction> kTransforms = {
  {Expression::ADD, &transform_linear},
  {Expression::CONST, &transform_linear},
  {Expression::MUL, &transform_linear},
  {Expression::VAR, &transform_linear},

  {Expression::ABS, &transform_abs},
  {Expression::P_NORM, &transform_p_norm},
  {Expression::QUAD_OVER_LIN, &transform_quad_over_lin},
};

Expression transform_expression(
    const Expression& input,
    std::vector<Expression>* constraints) {
  // First transform children
  std::vector<Expression> output_args;
  for (const Expression& arg : input.args)
    output_args.push_back(transform_expression(arg, constraints));

  auto iter = kTransforms.find(input.type);
  //CHECK(iter != kTransforms.end());
  return iter->second(input, output_args, constraints);
}

Problem LinearConeTransform::transform(const Problem& input) {
  Problem output;
  output.objective = transform_expression(
      input.objective, &output.constraints);
  for (const Expression& constr : input.constraints) {
    output.constraints.push_back(
        transform_expression(constr, &output.constraints));
  }
  return output;
}
