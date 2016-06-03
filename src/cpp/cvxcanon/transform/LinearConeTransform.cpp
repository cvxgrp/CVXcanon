
#include "LinearConeTransform.hpp"

#include <unordered_map>
#include <vector>

#include "cvxcanon/expression/Expression.hpp"
#include "cvxcanon/expression/ExpressionShape.hpp"
#include "cvxcanon/expression/ExpressionUtil.hpp"
#include "cvxcanon/expression/TextFormat.hpp"
#include "cvxcanon/util/MatrixUtil.hpp"
#include "glog/logging.h"

typedef Expression(*TransformFunction)(
    const Expression& expr,
    std::vector<Expression>* constraints);

Expression transform_abs(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  Expression t = epi_var(expr, "abs");
  constraints->push_back(leq(x, t));
  constraints->push_back(leq(neg(x), t));
  return t;
}

Expression transform_quad_over_lin(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  const Expression& y = expr.arg(1);
  Expression t = epi_var(expr, "qol");
  constraints->push_back(
      soc(hstack({add(y, neg(t)), reshape(mul(constant(2), x), 1, dim(x))}),
          add(y, t)));
  constraints->push_back(leq(constant(0), y));
  return t;
}

Expression transform_p_norm(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const double p = expr.attr<PNormAttributes>().p;
  const int axis = expr.attr<PNormAttributes>().axis;
  const Expression& x = expr.arg(0);
  if (p == 1) {
    return sum_entries(transform_abs(abs(x), constraints));
  } else if (p == 2) {
    Expression t = epi_var(expr, "p_norm_2");
    if (axis == kNoAxis) {
      constraints->push_back(soc(reshape(x, 1, dim(x)), t));
    } else if (axis == 0) {
      constraints->push_back(soc(transpose(x), transpose(t)));
    } else if (axis == 1) {
      constraints->push_back(soc(x, t));
    } else {
      LOG(FATAL) << "invalid axis " << axis;
    }

    return t;
  }

  LOG(FATAL) << "Not implemented "
             << "(p: " << expr.attr<PNormAttributes>().p << ")";
}

Expression transform_power(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const double p = expr.attr<PowerAttributes>().p;
  const Expression& x = expr.arg(0);
  if (p == 1) {
    return x;
  } else if (p == 2) {
    Expression t = epi_var(expr, "power_2");
    Expression vec_t = reshape(t, dim(x), 1);
    Expression vec_x = reshape(x, dim(x), 1);
    constraints->push_back(
        soc(hstack({add(constant(1), neg(vec_t)), mul(constant(2), vec_x)}),
            add(constant(1), vec_t)));
    return t;
  }

  LOG(FATAL) << "Not implemented "
             << "(p: " << expr.attr<PowerAttributes>().p << ")";
}

Expression transform_huber(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  CHECK_EQ(1, expr.args().size());
  const Expression& x = expr.arg(0);
  const Expression& M = expr.attr<HuberAttributes>().M;
  Expression n = epi_var(expr, "huber_n");
  Expression s = epi_var(expr, "huber_s");

  // n**2, 2*M*|s|
  Expression n2 = transform_power(power(n, 2), constraints);
  Expression abs_s = transform_abs(abs(s), constraints);
  Expression M_abs_s = mul(M, abs_s);
  Expression t = add(n2, mul(constant(2), M_abs_s));

  // x == s + n
  constraints->push_back(eq(x, add(n, s)));
  return t;
}

Expression transform_exp(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  Expression t = epi_var(expr, "exp");
  Expression ones = constant(
      ones_matrix(size(expr).dims[0],
                  size(expr).dims[1]));
  constraints->push_back(exp_cone(x, ones, t));
  return t;
}

Expression transform_entr(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  Expression t = epi_var(expr, "entr");
  const Expression& x = expr.arg(0);
  Expression ones = constant(
      ones_matrix(size(expr).dims[0],
                  size(expr).dims[1]));
  constraints->push_back(exp_cone(t, x, ones));
  return t;
}

Expression transform_kl_div(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  Expression x = promote_add(expr.arg(0), size(expr));
  Expression y = promote_add(expr.arg(1), size(expr));
  Expression t = epi_var(expr, "kl_div");
  constraints->push_back(exp_cone(t, x, y));
  constraints->push_back(leq(constant(0), y));
  // -t - x  + y
  return add(y, neg(add(x, t)));
}

Expression transform_log(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  Expression t = epi_var(expr, "log");
  Expression ones = constant(
      ones_matrix(size(expr).dims[0],
                  size(expr).dims[1]));
  constraints->push_back(exp_cone(t, ones, x));
  return t;
}

Expression transform_log1p(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  return transform_log(log(add(x, constant(1))), constraints);
}

Expression transform_max_elemwise(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  Expression t = epi_var(expr, "max_elemwise");
  for (const Expression& x : expr.args()) {
    constraints->append(leq(promote_add(x, size(expr)), t));
  }
  return t;
}

Expression transform_geo_mean(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  LOG(FATAL) << "not implemented";
}

Expression transform_lambda_max(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& A = expr.arg(0);
  const int n = size(A).dims[0];
  Expression t = epi_var(expr, "lambda_max");
  Expression prom_t = promote_add(t, {n, 1});
  // I*t - A
  constraints->push_back(sdp(add(diag_vec(prom_t), neg(A))));
  return t;
}

Expression transform_log_det(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& A = expr.arg(0);  // n by n matrix
  const int n = size(A).dims[0];

  Expression X = epi_var_size(expr, "log_det_X", {{2*n, 2*n}});
  Expression Z = epi_var_size(expr, "log_det_Z", {{n, n}});
  Expression d = epi_var_size(expr, "log_det_d", {{n, 1}});

  // TODO(mwytock): cvxpy uses semidef for X?

  // Require that X and A are PSD.
  constraints->push_back(sdp(A));
  constraints->push_back(sdp(X));

  // Fix Z as upper triangular, d as diag(Z).
  Expression Z_lower_tri = upper_tri(transpose(Z));
  constraints->push_back(eq(Z_lower_tri, constant(0)));
  constraints->push_back(eq(d, diag_mat(Z)));

  // Fix X using the fact that A must be affine by the DCP rules.
  // X[0:n, 0:n] == D
  // X[0:n, n:2*n] == Z,
  // X[n:2*n, n:2*n] == A
  constraints->push_back(eq(index(X, 0, n, 0, n), diag_vec(d)));
  constraints->push_back(eq(index(X, 0, n, n, 2*n), Z));
  constraints->push_back(eq(index(X, n, 2*n, n, 2*n), A));

  // Add the objective sum(log(d)
  return sum_entries(transform_log(log(d), constraints));
}

Expression transform_log_sum_exp(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  const int axis = expr.attr<LogSumExpAttributes>().axis;
  Expression t = epi_var(expr, "log_sum_exp");
  Expression x_minus_t = add(x, neg(promote_add_axis(t, axis)));
  Expression exp_x_minus_t = transform_exp(exp(x_minus_t), constraints);
  constraints->push_back(leq(sum_entries(exp_x_minus_t, axis), constant(1)));
  return t;
}

Expression transform_matrix_frac(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& X = expr.arg(0);  // n by m matrix
  const Expression& P = expr.arg(1);  // n by n matrix
  const int n = size(X).dims[0];
  const int m = size(X).dims[1];

  // Create a matrix with Schur complement T - X.T*P^-1*X.
  Expression M = epi_var_size(expr, "matrix_frac_M", {{n+m, n+m}});
  Expression T = epi_var_size(expr, "matrix_frac_T", {{m, m}});

  // Fix M using the fact that P must be affine by the DCP rules.
  // M[0:n, 0:n] == P
  // M[0:n, n:n+m] == X
  // M[n:n+m, n:n+m] == T
  constraints->push_back(eq(index(M, 0, n, 0, n), P));
  constraints->push_back(eq(index(M, 0, n, n, n+m), X));
  constraints->push_back(eq(index(M, n, n+m, n, n+m), T));
  constraints->push_back(sdp(M));
  return trace(T);
}

Expression promote_add_axis(const Expression& x, int axis) {
  if (axis == kNoAxis) {
    return promote_add(t, size(x));
  } else if (axis == 0) {
    Expression ones = constant(ones_matrix(size(x).dims[0], 1));
    return mul(ones, t);
  } else if (axis == 1) {
    Expression ones = constant(ones_matrix(1, size(x).dims[1]));
    return mul(t, ones);
  } else {
    LOG(FATAL) << "invalid axis " << axis;
}

Expression transform_max_entries(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const int axis = expr.attr<MaxEntriesAttributes>().axis;
  const Expression& x = expr.arg(0);
  Expression t = epi_var(expr, "max_entries");
  Expression prom_t = promote_add_axis(t, axis);
  constraints->push_back(leq(x, prom_t));
  return t;
}

Expression transform_norm_nuc(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& A = expr.arg(0);
  const int m = size(A).dims[0];
  const int n = size(A).dims[1];

  // Create the equivalent problem:
  //   minimize (trace(U) + trace(V))/2
  //   subject to:
  //     [U A; A.T V] is positive semidefinite

  Expression X = epi_var_size(expr, "norm_nuc_X", {{m+n, m+n}});
  constraints->push_back(eq(index(X, 0, m, m, m+n), A));
  constraints->push_back(sdp(X));
  return mul(constant(0.5), trace(X));
}

Expression transform_sigma_max(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& A = expr.arg(0);
  const int n = size(A).dims[0];
  const int m = size(A).dims[1];

  // Create a matrix with Schur complement I*t - (1/t)*A.T*A.
  Expression X = epi_var_size(expr, "sigma_max_X", {{n+m, n+}});
  Expression t = epi_var(expr, "sigma_max");

  // Fix X using the fact that A must be affine by the DCP rules.
  // X[0:n, 0:n] == I_n*t
  // X[0:n, n:n+m] == A
  // X[n:n+m, n:n+m] == I_m*t
  constraints->push_back(eq(index(X, 0, n, 0, n), promote_multiply(t, n)));
  constraints->push_back(eq(index(X, 0, n, n, n+m), A));
  constraints->push_back(eq(index(X, n, n+m, n, n+m), promote_multiply(t, m)));
  constraints->push_back(sdp(X));
  return t;
}


// min sum_entries(t) + kq
// s.t. x <= t + q
//      0 <= t
Expression transform_sum_largest(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  const Expression& x = expr.arg(0);
  const int k = expr.attr<SumLargestAttributes>().k;
  Expression q = epi_var(expr, "sum_largest_q");
  Expression t = epi_var_size(expr, "sum_largest_t", size(x));

  constraints->push_back(leq(x, add(t, q)));
  constraints->push_back(leq(constrant(0), t));
  return add(sum_entries(t), mul(constant(k), q));
}


std::unordered_map<int, TransformFunction> kTransforms = {
  // Elementwise functions
  {Expression::ABS, &transform_abs},
  {Expression::ENTR, &transform_entr},
  {Expression::EXP, &transform_exp},
  {Expression::HUBER, &transform_huber},
  {Expression::KL_DIV, &transform_kl_div},
  {Expression::LOG, &transform_log},
  {Expression::LOG1P, &transform_log1p},
  {Expression::MAX_ELEMWISE, &transform_max_elemwise},
  {Expression::POWER, &transform_power},

  // General functions
  {Expression::GEO_MEAN, &transform_geo_mean},
  {Expression::LAMBDA_MAX, &transform_lambda_max},
  {Expression::LOG_DET, &transform_log_det},
  {Expression::LOG_SUM_EXP, &transform_log_sum_exp},
  {Expression::MATRIX_FRAC, &transform_matrix_frac},
  {Expression::MAX_ENTRIES, &transform_max_entries},
  {Expression::NORM_NUC, &transform_norm_nuc},
  {Expression::P_NORM, &transform_p_norm},
  {Expression::QUAD_OVER_LIN, &transform_quad_over_lin},
  {Expression::SIGMA_MAX, &transform_sigma_max},
  {Expression::SUM_LARGEST, &transform_sum_largest},
};

Expression transform_expression(
    const Expression& expr,
    std::vector<Expression>* constraints) {
  // First transform children
  std::vector<Expression> linear_args;
  for (const Expression& arg : expr.args())
    linear_args.push_back(transform_expression(arg, constraints));

  // New expression, with linear args
  Expression output = Expression(expr.type(), linear_args, expr.attr_ptr());

  // Now transform non-linear functions, if necessary
  VLOG(2) << "transform_func: " << format_expression(expr);
  auto iter = kTransforms.find(expr.type());
  if (iter != kTransforms.end())
    output = iter->second(output, constraints);

  return output;
}

Problem LinearConeTransform::apply(const Problem& problem) {
  std::vector<Expression> constraints;
  Expression linear_objective = transform_expression(
      problem.objective, &constraints);
  for (const Expression& constr : problem.constraints) {
    constraints.push_back(
        transform_expression(constr, &constraints));
  }
  return {problem.sense, linear_objective, constraints};
}

// Return true if we have a transform for this expression itself
bool have_transform_self(const Expression& expr) {
  if (is_leaf(expr) || is_linear(expr) || is_constraint(expr))
    return true;

  auto iter = kTransforms.find(expr.type());
  if (iter == kTransforms.end()) {
    VLOG(1) << "No transform for " << format_expression(expr);
    return false;
  }

  // Special case for P_NORM and POWER, transforms dependent on p
  // TODO(mwytock): Remove this when we implement the full gm_constrs() logic.
  if (expr.type() == Expression::P_NORM) {
    const double p = expr.attr<PNormAttributes>().p;
    return p == 1 || p == 2;
  }
  if (expr.type() == Expression::POWER) {
    const double p = expr.attr<PowerAttributes>().p;
    LOG(INFO) << p;
    return p == 1 || p == 2;
  }

  return true;
}

// Return true if we have a transform this expression and its children
bool have_transform(const Expression& expr) {
  if (!have_transform_self(expr))
    return false;

  for (const Expression& arg : expr.args()) {
    if (!have_transform(arg))
      return false;
  }

  return true;
}

bool LinearConeTransform::accepts(const Problem& problem) {
  for (const Expression& constr : problem.constraints)
    if (!have_transform(constr))
      return false;
  return have_transform(problem.objective);
}
