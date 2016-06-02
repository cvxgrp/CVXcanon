
#include "cvxcanon/expression/Expression.hpp"

#include <glog/logging.h>

#include "cvxcanon/util/MatrixUtil.hpp"

void ConstAttributes::set_dense_data(double* matrix, int rows, int cols) {
  sparse = false;
  dense_data = Eigen::Map<DenseMatrix>(matrix, rows, cols);
  VLOG(2) << "set_dense_data: " << rows << " x " << cols << "\n"
          << matrix_debug_string(dense_data);
}

void ConstAttributes::set_sparse_data(
    double* data, int data_len,
    double* row_idxs, int rows_len,
    double* col_idxs, int cols_len,
    int rows, int cols) {
  CHECK_EQ(rows_len, data_len);
  CHECK_EQ(cols_len, data_len);

  Matrix sparse_coeffs(rows, cols);
  std::vector<Triplet> tripletList;
  tripletList.reserve(data_len);
  for (int idx = 0; idx < data_len; idx++) {
    tripletList.push_back(
        Triplet(int(row_idxs[idx]), int(col_idxs[idx]), data[idx]));
  }

  sparse = true;
  sparse_data = SparseMatrix(rows, cols);
  sparse_data.setFromTriplets(tripletList.begin(), tripletList.end());
  sparse_data.makeCompressed();
}

Size ConstAttributes::size() const {
  int m, n;
  if (sparse) {
    m = sparse_data.rows();
    n = sparse_data.cols();
  } else {
    m = dense_data.rows();
    n = dense_data.cols();
  }
  return {{m, n}};
}
