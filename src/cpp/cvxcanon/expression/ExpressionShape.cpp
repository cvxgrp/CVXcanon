
#include <unordered_map>
#include <vector>

#include "cvxcanon/expression/ExpressionShape.hpp"
#include "cvxcanon/expression/ExpressionUtil.hpp"
#include "cvxcanon/expression/TextFormat.hpp"

Size get_scalar_shape(const Expression& expr) {
  return {{1, 1}};
}

Size get_elementwise_shape(const Expression& expr) {
  return size(expr.arg(0));
}

Size get_axis_shape(const Expression& expr, int axis) {
  const Expression& x = expr.arg(0);
  if (axis == kNoAxis) {
    return {{1, 1}};
  } else if (axis == 0) {
    return {{1, size(x).dims[1]}};
  } else if (axis == 1) {
    return {{size(x).dims[0], 1}};
  }
  LOG(FATAL) << "invalid axis: " << axis;
}

Size get_add_shape(const Expression& expr) {
  Size expr_size = {{1, 1}};
  for (const Expression& arg : expr.args()) {
    if (dim(arg) != 1) {
      // Assume sizes are compatible
      expr_size = size(arg);
    }
  }
  return expr_size;
}

Size get_sum_entries_shape(const Expression& expr) {
  return get_axis_shape(expr, expr.attr<SumEntriesAttributes>().axis);
}

Size get_p_norm_shape(const Expression& expr) {
  return get_axis_shape(expr, expr.attr<PNormAttributes>().axis);
}

Size get_log_sum_exp_shape(const Expression& expr) {
  return get_axis_shape(expr, expr.attr<LogSumExpAttributes>().axis);
}

Size get_max_entries_shape(const Expression& expr) {
  return get_axis_shape(expr, expr.attr<MaxEntriesAttributes>().axis);
}


Size get_mul_shape(const Expression& expr) {
  assert(expr.args().size() == 2);
  Size lhs_size = size(expr.arg(0));
  Size rhs_size = size(expr.arg(1));

  // promotion
  if (is_scalar(lhs_size))
    return rhs_size;
  if (is_scalar(rhs_size))
    return lhs_size;

  assert(lhs_size.dims[1] == rhs_size.dims[0]);
  return {{lhs_size.dims[0], rhs_size.dims[1]}};
}

Size get_const_shape(const Expression& expr) {
  return expr.attr<ConstAttributes>().size();
}

Size get_var_shape(const Expression& expr) {
  return expr.attr<VarAttributes>().size;
}

Size get_hstack_shape(const Expression& expr) {
  int cols = 0;
  for (const Expression& arg : expr.args())
    cols += size(arg).dims[1];
  return {{size(expr.arg(0)).dims[0], cols}};
}

Size get_vstack_shape(const Expression& expr) {
  int rows = 0;
  for (const Expression& arg : expr.args())
    rows += size(arg).dims[0];
  return {{rows, size(expr.arg(0)).dims[1]}};
}

Size get_reshape_shape(const Expression& expr) {
  return expr.attr<ReshapeAttributes>().size;
}

Size get_index_shape(const Expression& expr) {
  std::vector<int> dims;
  for (const Slice& slice : expr.attr<IndexAttributes>().keys) {
    const int n = size(expr.arg(0)).dims[dims.size()];
    const int start = slice.start < 0 ? n + slice.start : slice.start;
    const int stop  = slice.stop  < 0 ? n + slice.stop  : slice.stop;
    const int size_i = (stop - start) / slice.step;
    dims.push_back(size_i < 0 ? 0 : size_i);
  }
  return {dims};
}

Size get_diag_mat_shape(const Expression& expr) {
  const int n = size(expr.arg(0)).dims[0];
  return {{n, 1}};
}

Size get_diag_vec_shape(const Expression& expr) {
  const int n = size(expr.arg(0)).dims[0];
  return {{n, n}};
}

Size get_transpose_shape(const Expression& expr) {
  const Expression& x = expr.arg(0);
  const int m = size(x).dims[0];
  const int n = size(x).dims[1];
  return {{n, m}};
}

Size get_kron_shape(const Expression& expr) {
  const Expression& A = expr.arg(0);
  const Expression& B = expr.arg(1);
  const int m = size(A).dims[0]*size(B).dims[0];
  const int n = size(A).dims[1]*size(B).dims[1];
  return {{m, n}};
}

Size get_trace_shape(const Expression& expr) {
  return {{1, 1}};
}

Size get_upper_tri_shape(const Expression& expr) {
  const Expression& X = expr.arg(0);
  const int m = size(X).dims[0];
  const int n = size(X).dims[1];
  return {{m*(n-1)/2, 1}};
}

typedef Size(*ShapeFunction)(const Expression& expr);

std::unordered_map<int, ShapeFunction> kShapeFunctions = {
  // Linear functions
  {Expression::ADD, &get_add_shape},
  {Expression::DIAG_MAT, &get_diag_mat_shape},
  {Expression::DIAG_VEC, &get_diag_vec_shape},
  {Expression::HSTACK, &get_hstack_shape},
  {Expression::INDEX, &get_index_shape},
  {Expression::KRON, &get_kron_shape},
  {Expression::MUL, &get_mul_shape},
  {Expression::NEG, &get_elementwise_shape},
  {Expression::RESHAPE, &get_reshape_shape},
  {Expression::SUM_ENTRIES, &get_sum_entries_shape},
  {Expression::TRACE, &get_trace_shape},
  {Expression::TRANSPOSE, &get_transpose_shape},
  {Expression::UPPER_TRI, &get_upper_tri_shape},
  {Expression::VSTACK, &get_vstack_shape},

  // Elementwise functions
  {Expression::ABS, &get_elementwise_shape},
  {Expression::ENTR, &get_elementwise_shape},
  {Expression::EXP, &get_elementwise_shape},
  {Expression::HUBER, &get_elementwise_shape},
  {Expression::KL_DIV, &get_elementwise_shape},
  {Expression::LOG, &get_elementwise_shape},
  {Expression::LOG1P, &get_elementwise_shape},
  {Expression::LOGISTIC, &get_elementwise_shape},
  {Expression::MAX_ELEMWISE, &get_elementwise_shape},
  {Expression::POWER, &get_elementwise_shape},

  // General non linear functions
  {Expression::GEO_MEAN, &get_scalar_shape},
  {Expression::LAMBDA_MAX, &get_scalar_shape},
  {Expression::LOG_DET, &get_scalar_shape},
  {Expression::LOG_SUM_EXP, &get_log_sum_exp_shape},
  {Expression::MATRIX_FRAC, &get_scalar_shape},
  {Expression::MAX_ENTRIES, &get_max_entries_shape},
  {Expression::NORM_NUC, &get_scalar_shape},
  {Expression::P_NORM, &get_p_norm_shape},
  {Expression::QUAD_OVER_LIN, &get_scalar_shape},
  {Expression::SIGMA_MAX, &get_scalar_shape},
  {Expression::SUM_LARGEST, &get_scalar_shape},

  // Leaf nodes
  {Expression::CONST, &get_const_shape},
  {Expression::VAR, &get_var_shape},
};

Size size(const Expression& expr) {
  // TODO(mwytock): Add caching (w/ storage on Expression) so that we dont
  // repeatedly recompute size for the same node.
  auto iter = kShapeFunctions.find(expr.type());
  CHECK(iter != kShapeFunctions.end())
      << "no shape for " << format_expression(expr);
  Size retval = iter->second(expr);
  VLOG(3) << "size: " << retval.dims[0] << " x " << retval.dims[1] << ", "
          << format_expression(expr);
  return retval;
}

int dim(const Expression& expr) {
  Size s = size(expr);
  return s.dims[0]*s.dims[1];
}
