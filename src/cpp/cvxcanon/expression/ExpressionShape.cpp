
#include <unordered_map>
#include <vector>

#include "cvxcanon/expression/ExpressionShape.hpp"
#include "cvxcanon/expression/ExpressionUtil.hpp"
#include "cvxcanon/expression/TextFormat.hpp"

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
  // TODO(mwytock): axis parameter
  return {{1, 1}};
}

Size get_p_norm_shape(const Expression& expr) {
  // TODO(mwytock): axis parameter
  return {{1, 1}};
}

Size get_quad_over_lin_shape(const Expression& expr) {
  return {{1, 1}};
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

Size get_elementwise_shape(const Expression& expr) {
  return size(expr.arg(0));
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

typedef Size(*ShapeFunction)(const Expression& expr);

std::unordered_map<int, ShapeFunction> kShapeFunctions = {
  // Linear functions
  {Expression::ADD, &get_add_shape},
  {Expression::DIAG_MAT, &get_diag_mat_shape},
  {Expression::DIAG_VEC, &get_diag_vec_shape},
  {Expression::HSTACK, &get_hstack_shape},
  {Expression::INDEX, &get_index_shape},
  {Expression::MUL, &get_mul_shape},
  {Expression::NEG, &get_elementwise_shape},
  {Expression::RESHAPE, &get_reshape_shape},
  {Expression::VSTACK, &get_vstack_shape},
  {Expression::TRANSPOSE, &get_transpose_shape},

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

  // Non linear functions
  {Expression::P_NORM, &get_p_norm_shape},
  {Expression::QUAD_OVER_LIN, &get_quad_over_lin_shape},
  {Expression::SUM_ENTRIES, &get_sum_entries_shape},

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
