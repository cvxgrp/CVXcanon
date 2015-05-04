// Some useful defines for Matricies/etc.

#include "../include/Eigen/Sparse"

#define NULL_MATRIX Eigen::SparseMatrix<double>(0,0)

typedef Eigen::SparseMatrix<double> Matrix;
typedef std::map<int, Matrix> CoeffMap;
