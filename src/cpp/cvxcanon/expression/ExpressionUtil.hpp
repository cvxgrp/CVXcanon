// Utility functions for constructing and manipulating expression trees.

#ifndef EXPRESSION_UTIL_H
#define EXPRESSION_UTIL_H

#include <string>

#include "cvxcanon/expression/Expression.hpp"

Expression abs(Expression x);
Expression add(Expression x, Expression y);
Expression constant(DenseMatrix value);
Expression constant(double value);
Expression hstack(std::vector<Expression> args);
Expression mul(Expression x, Expression y);
Expression neg(Expression x);
Expression p_norm(Expression x, double p);
Expression power(Expression x, double p);
Expression quad_over_lin(Expression x, Expression y);
Expression reshape(Expression x, int m, int n);
Expression sum_entries(Expression x);
Expression var(int m, int n);
Expression vstack(std::vector<Expression> args);

// Constraints
Expression eq(Expression x, Expression y);  // x == y
Expression leq(Expression x, Expression y);  // x <= y
Expression soc(Expression x, Expression y);  // ||x||_2 <= y

// K = {(x,y,z) | y > 0, ye^(x/y) <= z} U {(x,y,z) | x <= 0, y = 0, z >= 0}
Expression exp_cone(Expression x, Expression y, Expression z);

// Utililties
Expression epi_var(const Expression& x, const std::string& name);
Expression scalar_epi_var(const Expression& x, const std::string& name);
int count_nodes(const Expression& x);
int count_nodes(const Problem& prob);
bool is_scalar(const Size& size);

#endif // EXPRESSION_UTIL_H
