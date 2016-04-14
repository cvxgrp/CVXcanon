
#include "ExpressionUtil.hpp"

Expression add(Expression x, Expression y) {
  return {Expression::ADD, {x,y}};
}

Expression mul(Expression x, Expression y) {
  return {Expression::MUL, {x,y}};
}

Expression neg(Expression x) {
  return {Expression::NEG, {x}};
}

Expression var(int m, int n) {
  return {Expression::VAR, {}};
}

Expression constant(int m, int n) {
  return {Expression::CONST, {}};
}

Expression constant(double value) {
  return {Expression::CONST, {}};
}

Expression quad_over_lin(Expression x, Expression y) {
  return {Expression::QUAD_OVER_LIN, {x,y}};
}

Expression p_norm(Expression x, double p) {
  return {Expression::P_NORM, {x}};
}

Expression hstack(std::vector<Expression> args) {
  return {Expression::HSTACK, args};
}

Expression reshape(Expression x, int m, int n) {
  return {Expression::RESHAPE, {x}};
}

Expression sum(Expression x) {
  return {Expression::SUM, {x}};
}

// x <= y
Expression leq(Expression x, Expression y) {
  return {Expression::LEQ, {x,y}};
}

// ||x||_2 <= y
Expression soc(Expression x, Expression y) {
  return {Expression::SOC, {x,y}};
}

Expression epi_var(const Expression& x, const std::string& name) {
  return {Expression::VAR, {}};
}

Expression scalar_epi_var(const Expression& x, const std::string& name) {
  return {Expression::VAR, {}};
}

int dim(const Expression& x) {
  return 1;
  // int dim = 1;
  // for (int x : x.size.dims) {
  //   dim *= x;
  // }
  // return dim;
}

int count_nodes(const Expression& x) {
  int retval = 1;
  for (const Expression& arg : x.args()) {
    retval += count_nodes(arg);
  }
  return retval;
}

int count_nodes(const Problem& problem) {
  int retval = count_nodes(problem.objective);
  for (const Expression& constr : problem.constraints) {
    retval += count_nodes(constr);
  }
  return retval;
}
