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
		OperatorType type;
		int dataRows;
		int dataCols;

		std::vector< int > size;
		std::vector< LinOp* > args;

		Matrix sparse_data;
		bool sparse = false;
		Eigen::MatrixXd dense_data;
		
		std::vector<std::vector<double> > data;

		bool hasConstantType(){
			return  type == SCALAR_CONST || 
			type == DENSE_CONST || type == SPARSE_CONST;
		}

		void addDenseData(double* matrix, int rows, int cols){
			Eigen::Map<Eigen::MatrixXd> mf(matrix, rows, cols);
			dense_data = mf;
		}

		void addSparseData(double *data, int data_len, double *rows, int rows_len,
										 	 double *cols, int cols_len){
			assert(rows_len == data_len && cols_len == data_len);
			sparse = true;
			Matrix coeffs (dataRows, dataCols);
			std::vector<Triplet> tripletList;
			tripletList.reserve(data_len);
			for(int idx = 0; idx < data_len; idx++){
				tripletList.push_back(Triplet(int(rows[idx]), int(cols[idx]), data[idx]));
			}
			coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
			coeffs.makeCompressed();
			sparse_data = coeffs; 
		}
};
#endif