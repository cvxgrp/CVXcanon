//    This file is part of CVXcanon.
//
//    CVXcanon is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    CVXcanon is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with CVXcanon.  If not, see <http://www.gnu.org/licenses/>.

// Some useful defines for Matricies/etc.

#include "../include/Eigen/Sparse"
#include "../include/Eigen/Core"

#define NULL_MATRIX Eigen::SparseMatrix<double>(0,0)

typedef Eigen::Matrix<int, Eigen::Dynamic, 1> Vector;
typedef Eigen::SparseMatrix<double> Matrix;
typedef std::map<int, Matrix> CoeffMap;
typedef Eigen::Triplet<double> Triplet;