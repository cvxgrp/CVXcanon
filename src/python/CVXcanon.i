%module CVXcanon

%{
	#define SWIG_FILE_WITH_INIT
	#include "CVXcanon.hpp"
%}

%include "numpy.i"
%include "std_vector.i"
%include "std_map.i"

/* Must call this before using NUMPY-C API */
%init %{
	import_array();
%}

/* Typemap for the addDenseData C++ routine in LinOp.hpp */
%apply (double* IN_FARRAY2, int DIM1, int DIM2) {(double* matrix, int rows, int cols)};

/* Typemap for the addSparseData C++ routine in LinOp.hpp */
%apply (double* INPLACE_ARRAY1, int DIM1) {(double *data, int data_len),
	(double *row_idxs, int rows_len),
	(double *col_idxs, int cols_len)};

%include "LinOp.hpp"

/* Typemap for the getV, getI, getJ, and getConstVec C++ routines in 
	 problemData.hpp */
%apply (double* ARGOUT_ARRAY1, int DIM1) {(double* values, int num_values)}
%include "ProblemData.hpp"

/* Useful wrappers for the LinOp class */
namespace std {
   %template(IntVector) vector<int>;
   %template(DoubleVector) vector<double>;
   %template(IntVector2D) vector< vector<int> >;
   %template(DoubleVector2D) vector< vector<double> >;
   %template(IntIntMap) map<int, int>;
   %template(LinOpVector) vector< LinOp * >;
}

/* Wrapper for entry point into CVXCanon Library */
ProblemData build_matrix(std::vector< LinOp* > constraints, std::map<int, int> id_to_col);

