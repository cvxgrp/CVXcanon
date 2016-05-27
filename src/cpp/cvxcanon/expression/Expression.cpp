
#include "cvxcanon/expression/Expression.hpp"
#include "cvxcanon/util/MatrixUtil.hpp"

void ConstAttributes::set_dense_data(double* matrix, int rows, int cols) {
  dense_data = Eigen::Map<DenseMatrix>(matrix, rows, cols);
  VLOG(2) << "set_dense_data: " << rows << " x " << cols << "\n"
          << matrix_debug_string(dense_data);
}

Size ConstAttributes::size() const {
  return {{static_cast<int>(dense_data.rows()),
           static_cast<int>(dense_data.cols())}};
}
