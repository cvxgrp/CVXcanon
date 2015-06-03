#ifndef LINOP_H
#define LINOP_H

#include <vector>
#include <cassert>
#include <iostream>
#include "Utils.hpp"


static const int CONSTANT_ID = -1;


enum operatortype {
	VARIABLE,
	PROMOTE,
	MUL,
	RMUL,
	MUL_ELEM,
	DIV,
	SUM,
	NEG,
	INDEX,
	TRANSPOSE,
	SUM_ENTRIES,
	TRACE,
	RESHAPE,
	DIAG_VEC,
	DIAG_MAT,
	UPPER_TRI,
	CONV,
	HSTACK,
	VSTACK,
	SCALAR_CONST,
	DENSE_CONST,
	SPARSE_CONST,
	NO_OP
};

typedef operatortype OperatorType;

class LinOp{	
	public:
		/*  */
		OperatorType type;
		std::vector< int > size;
		std::vector< LinOp* > args;

		/* Sparse Data Fields */
		bool sparse = false; /* True only if linOp has sparse_data*/
		Matrix sparse_data;
		int rows;
		int cols;

		/*  */
		Eigen::MatrixXd dense_data;
		
		/* Slice Data */
		std::vector<std::vector<double> > slice;

		bool has_constant_type(){
			return  type == SCALAR_CONST || 
			type == DENSE_CONST || type == SPARSE_CONST;
		}

		void set_dense_data(double* matrix, int rows, int cols){
			dense_data = Eigen::Map<Eigen::MatrixXd> (matrix, rows, cols);
		}

		void set_sparse_data(double *data, int data_len, double *row_idxs,
												 int rows_len, double *col_idxs, int cols_len){
			assert(rows_len == data_len && cols_len == data_len);
			sparse = true;
			Matrix sparse_coeffs(rows, cols);  
			std::vector<Triplet> tripletList;
			tripletList.reserve(data_len);
			for(int idx = 0; idx < data_len; idx++){
				tripletList.push_back(Triplet(int(row_idxs[idx]), int(col_idxs[idx]), data[idx]));
			}
			sparse_coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
			sparse_coeffs.makeCompressed();
			sparse_data = sparse_coeffs;
		}
};
#endif