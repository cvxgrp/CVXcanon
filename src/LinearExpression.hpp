#ifndef LINEAR_EXPRESSION_H
#define LINEAR_EXPRESSION_H

#include <map>

const int kConstCoefficientId = -1;
std::map<int, SparseMatrix> get_coefficients(const Expression& expr);

#endif  // LINEAR_EXPRESSION_H
