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
		std::vector<double> V;
		std::vector<int> I;
		std::vector<int> J;
		int dataRows;
		int dataCols;

		std::vector< int > size;
		std::vector< LinOp* > args;

		double * data_ptr = NULL;
		Matrix test;
		
		std::vector<std::vector<double> > data;

		bool hasConstantType(){
			return  type == SCALAR_CONST || 
			type == DENSE_CONST || type == SPARSE_CONST;
		}

		void addDenseData(double* matrix, int rows, int cols){
			dataRows = rows;
			dataCols = cols;
			data_ptr = matrix;
			for (int i=0; i < rows; i++){
  			for(int j=0; j < cols; j++){
  				I.push_back(i);
  				J.push_back(j);
  				V.push_back(matrix[j * rows + i]);  			
  			}
  		}
		}

		void addSparseData(double *data, int data_len, double *rows, int rows_len,
										 	 double *cols, int cols_len){
			assert(rows_len == data_len && cols_len == data_len);
			for(int i = 0; i < data_len; i++){
				V.push_back(data[i]);
				I.push_back(int(rows[i]));
				J.push_back(int(cols[i]));
			}
		}
};
#endif