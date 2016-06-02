#ifndef CVXCANON_EXPRESSION_TEXT_FORMAT_H
#define CVXCANON_EXPRESSION_TEXT_FORMAT_H

#include <string>

#include "cvxcanon/expression/Expression.hpp"

std::string format_expression(const Expression& expr);
std::string format_problem(const Problem& problem);

std::string tree_format_expression(
    const Expression& expr, const std::string& pre = "");

#endif  // CVXCANON_EXPRESSION_TEXT_FORMAT_H
