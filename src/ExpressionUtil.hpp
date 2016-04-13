// Utility functions for constructing and manipulating expression trees.

#ifndef EXPRESSION_UTIL_H
#define EXPRESSION_UTIL_H

#include <string>

#include "Expression.hpp"

// Expressions
Expression add(Expression x, Expression y);
Expression constant(double value);
Expression hstack(std::vector<Expression> x);
Expression mul(Expression x, Expression y);
Expression neg(Expression x);
Expression sum(Expression x);
Expression reshape(Expression x, Size size);

// Constraints
Expression leq(Expression x, Expression y);  // x <= y
Expression soc(Expression x, Expression y);  // ||x||_2 <= y

// Utililties
Expression epi_var(const Expression& x, const std::string& name);
Expression scalar_epi_var(const Expression& x, const std::string& name);
Expression shallow_copy(const Expression& x);
int dim(const Expression& x);

#endif // EXPRESSION_UTIL_H
