
#include "TextFormat.hpp"

#include <unordered_map>

#include "cvxcanon/expression/Expression.hpp"
#include "cvxcanon/util/Utils.hpp"

std::unordered_map<int, std::string> kSenseNames = {
  {Problem::MAXIMIZE, "maximize"},
  {Problem::MINIMIZE, "minimize"},
};

std::unordered_map<int, std::string> kExpressionNames = {
  // Linear functions
  {Expression::ADD, "add"},
  {Expression::DIAG_MAT, "diag_mat"},
  {Expression::DIAG_VEC, "diag_vec"},
  {Expression::HSTACK, "hstack"},
  {Expression::INDEX, "index"},
  {Expression::KRON, "kron"},
  {Expression::MUL, "mul"},
  {Expression::NEG, "neg"},
  {Expression::RESHAPE, "reshape"},
  {Expression::SUM_ENTRIES, "sum_entries"},
  {Expression::TRACE, "trace"},
  {Expression::UPPER_TRI, "upper_tri"},
  {Expression::VSTACK, "vstack"},

  // Elementwise functions
  {Expression::ABS, "abs"},
  {Expression::ENTR, "entr"},
  {Expression::EXP, "exp"},
  {Expression::HUBER, "huber"},
  {Expression::KL_DIV, "kl_div"},
  {Expression::LOG, "log"},
  {Expression::LOG1P, "log1p"},
  {Expression::LOGISTIC, "logistic"},
  {Expression::MAX_ELEMWISE, "max_elemwise"},
  {Expression::POWER, "power"},

  // General nonlinear functions
  {Expression::GEO_MEAN, "geo_mean"},
  {Expression::LAMBDA_MAX, "lambda_max"},
  {Expression::LOG_DET, "log_det"},
  {Expression::LOG_SUM_EXP, "log_sum_exp"},
  {Expression::MATRIX_FRAC, "matrix_frac"},
  {Expression::MAX_ENTRIES, "max_entries"},
  {Expression::NORM_NUC, "norm_nuc"},
  {Expression::P_NORM, "p_norm"},
  {Expression::QUAD_OVER_LIN, "quad_over_lin"},
  {Expression::SIGMA_MAX, "sigma_max"},
  {Expression::SUM_LARGEST, "sum_largest"},

  // Constraints
  {Expression::EQ, "eq"},
  {Expression::LEQ, "leq"},
  {Expression::SOC, "soc"},

  // Leaf nodes
  {Expression::CONST, "const"},
  {Expression::PARAM, "param"},
  {Expression::VAR, "var"},
};

std::string format_expression(const Expression& expr) {
  std::string retval = find_or_die(
      kExpressionNames, static_cast<int>(expr.type()));
  if (!expr.args().empty()) {
    retval += "(";
    for (int i = 0; i < expr.args().size(); i++) {
      if (i != 0) retval += ", ";
      retval += format_expression(expr.arg(i));
    }
    retval += ")";
  }
  return retval;
}

std::string format_problem(const Problem& problem) {
  std::string retval;
  retval += find_or_die(kSenseNames, static_cast<int>(problem.sense));
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
