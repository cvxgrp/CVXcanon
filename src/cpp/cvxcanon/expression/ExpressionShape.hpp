// Compute the output dimension of a function represented by an expression tree.

#ifndef CVXCANON_EXPRESSION_EXPRESSION_SHAPE_H
#define CVXCANON_EXPRESSION_EXPRESSION_SHAPE_H

#include "cvxcanon/expression/Expression.hpp"

// The output dimensions of the expression
Size size(const Expression& x);

// The product of the output dimensions of the expression,
// i.e. dims[0]*dims[1]*...
int dim(const Expression& x);

#endif  // CVXCANON_EXPRESSION_EXPRESSION_SHAPE_H
