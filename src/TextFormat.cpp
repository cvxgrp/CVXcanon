
#include "TextFormat.hpp"

#include <unordered_map>

#include "Expression.hpp"

std::unordered_map<int, std::string> kSenseNames = {
  {Problem::MAXIMIZE, "maximize"},
  {Problem::MINIMIZE, "minimize"},
};

std::unordered_map<int, std::string> kExpressionNames = {
  {Expression::ADD, "add"},
  {Expression::CONST, "const"},
  {Expression::EQ, "eq"},
  {Expression::LEQ, "leq"},
  {Expression::MUL, "mul"},
  {Expression::NEG, "neg"},
  {Expression::P_NORM, "p_norm"},
  {Expression::QUAD_OVER_LIN, "quad_over_lin"},
  {Expression::VAR, "var"},
};

std::string format_expression(const Expression& expr) {
  std::string retval = kExpressionNames[expr.type];
  if (!expr.args.empty()) {
    retval += "(";
    for (int i = 0; i < expr.args.size(); i++) {
      if (i != 0) retval += ", ";
      retval += format_expression(expr.args[i]);
    }
    retval += ")";
  }
  return retval;
}

std::string format_problem(const Problem& problem) {
  std::string retval;
  retval += kSenseNames[problem.sense];
  retval += "   ";
  retval += format_expression(problem.objective);
  if (!problem.constraints.empty()) {
    retval += "\nsubject to ";
    for (int i = 0; i < problem.constraints.size(); i++) {
      if (i != 0) retval += "\n           ";
      retval += format_expression(problem.constraints[i]);
    }
  }
  return retval;
}
