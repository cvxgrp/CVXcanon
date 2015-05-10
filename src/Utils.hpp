// Some useful defines for Matricies/etc.

#include "../include/Eigen/Sparse"
#include "../include/Eigen/Core"

#define NULL_MATRIX Eigen::SparseMatrix<double>(0,0)

typedef Eigen::Matrix<int, Eigen::Dynamic, 1> Vector;
typedef Eigen::SparseMatrix<double> Matrix;
typedef std::map<int, Matrix> CoeffMap;
typedef Eigen::Triplet<double> Triplet;