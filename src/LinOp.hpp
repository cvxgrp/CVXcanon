#include <vector>
#include "Utils.hpp"

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
	NO_OP,
	CONSTANT_ID
};

typedef operatortype OperatorType;

class LinOp{	
	public:
		OperatorType type;
		void* data;
		int size[2];
		std::vector< LinOp* > args;
		
		bool hasConstantType(){
			return  type == SCALAR_CONST || 
			type == DENSE_CONST || type == SPARSE_CONST;
		}
};


std::map<int,Matrix> variable_coeffs(LinOp lin){
	//@ TODO FOR JOHN
	return SPARSE_CONST;
}


std::map<int,Matrix> const_coeffs(LinOp lin){
		case SCALAR_CONST:
			//@ TODO FOR JOHN
			break;
		case DENSE_CONST:
			//@ TODO FOR JOHN
			break;
		case SPARSE_CONST:
			//@ TODO FOR JOHN
			break;					
}

std::vector<Matrix> func_coeffs(LinOp lin){
	switch(op){
		case PROMOTE:
			//@ TODO FOR JOHN
			break;
		case MUL:
			//@ TODO FOR JOHN
			break;
		case RMUL:
			//@ TODO FOR JOHN
			break;
		case MUL_ELEM:
			//@ TODO FOR JOHN
			break;
		case DIV:
			//@ TODO FOR JOHN
			break;
		case SUM:
			//@ TODO FOR JOHN
			break;
		case NEG:
			//@ TODO FOR JOHN
			break;
		case INDEX:
			//@ TODO FOR JOHN
			break;
		case TRANSPOSE:
			//@ TODO FOR JOHN
			break;
		case SUM_ENTRIES:
			//@ TODO FOR JOHN
			break;
		case TRACE:
			//@ TODO FOR JOHN
			break;
		case RESHAPE:
			//@ TODO FOR JOHN
			break;
		case DIAG_VEC:
			//@ TODO FOR JOHN
			break;
		case DIAG_MAT:
			//@ TODO FOR JOHN
			break;
		case UPPER_TRI:
			//@ TODO FOR JOHN
			break;
		case CONV:
			//@ TODO FOR JOHN
			break;
		case HSTACK:
			//@ TODO FOR JOHN
			break;
		case VSTACK:
			//@ TODO FOR JOHN
			break;
		case NO_OP:
			//@ TODO FOR JOHN
			break;
		case CONSTANT_ID:
			//@ TODO FOR JOHN
			break;
	}
	return NULL_MATRIX;
}