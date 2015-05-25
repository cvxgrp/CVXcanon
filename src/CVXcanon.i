%module CVXcanon

%{
#define SWIG_FILE_WITH_INIT
#include "CVXcanon.hpp"
%}

%include "numpy.i"
%include "std_vector.i"
%include "std_map.i"
%include "ProblemData.hpp"
%include "LinOp.hpp"

namespace std {
   %template(IntVector) vector<int>;
   %template(DoubleVector) vector<double>;
   %template(DoubleVector2D) vector< vector<double> >;
   %template(IntIntMap) map<int, int>;
   %template(LinOpVector) vector< LinOp * >;
}

%init %{
	import_array();
%}

/* Typemap for the addDenseData(double* matrix, int rows, int cols) C/C++ routine */
%apply (double* IN_ARRAY2, int DIM1, int DIM2) {(double* matrix, int rows, int cols)};

/* Typemap for the addSparseData(double * data, int data_len, double *rows, int rows_len,
										 	 double *cols, int cols_len) C/C++ routine */
%apply (double* INPLACE_ARRAY1, int DIM1) {(double* data, int data_len),
																					 (double *rows, int rows_len),
																					 (double *cols, int cols_len)};

ProblemData build_matrix(std::vector< LinOp* > constraints, std::map<int, int> id_to_col);