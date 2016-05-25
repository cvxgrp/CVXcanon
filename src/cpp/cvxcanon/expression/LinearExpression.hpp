// Methods for converting linear expressions to sparse matrices
//
// TODO(mwytock): This is eventually intended to replace the fuinctionality in
// ../linop, i.e. we should merge all functionality from LinOpOperations and
// replace LinOp with Expression everywhere.

#ifndef CVXCANON_EXPRESSION_LINEAR_EXPRESSION_H
#define CVXCANON_EXPRESSION_LINEAR_EXPRESSION_H

#include <map>

#include "cvxcanon/util/Utils.hpp"
#include "cvxcanon/expression/Expression.hpp"

// ID for the const term of the affine fnuction
const int kConstCoefficientId = -1;

// Given a expression representing a linear function, return its coefficients in
// the form of a map from id -> Matrix where id is either a variable id or
// kConstCoefficientId, defined above.
CoeffMap get_coefficients(const Expression& expr);

// Returns true if CoeffMap represents a constant function (e.g. only has
// coefficients for the const term)
bool is_constant(const CoeffMap& coeffs);

#endif  // CVXCANON_EXPRESSION_LINEAR_EXPRESSION_H
