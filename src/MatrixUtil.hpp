
#ifndef MATRIX_UTIL_H
#define MATRIX_UTIL_H

#include "Utils.hpp"

void append_block_triplets(
    const SparseMatrix& A, int i, int j,
    std::vector<Triplet>* coeffs);

SparseMatrix sparse_matrix(int m, int n, const std::vector<Triplet>& coeffs);
SparseMatrix scalar_matrix(double alpha);
SparseMatrix ones_matrix(int m, int n);
SparseMatrix identity(int n);
SparseMatrix reshape(const SparseMatrix& A, int m, int n);

DenseVector to_vector(const DenseMatrix& A);

#endif // MATRIX_UTIL_H
