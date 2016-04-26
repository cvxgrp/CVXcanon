
#include "MatrixUtil.hpp"

void append_block_triplets(
    const SparseMatrix& A, int i, int j,
    std::vector<Triplet>* coeffs) {
  for (int k = 0; k < A.outerSize(); k++) {
    for (SparseMatrix::InnerIterator iter(A, k); iter; ++iter) {
      coeffs->push_back(Triplet(i+iter.row(), j+iter.col(), iter.value()));
    }
  }
}

SparseMatrix sparse_matrix(int m, int n, const std::vector<Triplet>& coeffs) {
  SparseMatrix A(m, n);
  A.setFromTriplets(coeffs.begin(), coeffs.end());
  return A;
}

SparseMatrix scalar_matrix(double alpha, int n) {
  return (alpha*DenseMatrix::Identity(n,n)).sparseView();
}

SparseMatrix ones_matrix(int m, int n) {
  return DenseMatrix::Constant(m, n, 1).sparseView();
}

SparseMatrix identity(int n) {
  return DenseMatrix::Identity(n,n).sparseView();
}

SparseMatrix reshape(const SparseMatrix& A, int m, int n) {
  // TODO(mwytock): More efficient way?
  DenseMatrix A_copy = A;
  return Eigen::Map<const DenseMatrix>(A_copy.data(), m, n).sparseView();
}

DenseVector to_vector(const DenseMatrix& A) {
  return Eigen::Map<const DenseVector>(A.data(), A.rows()*A.cols());
}
