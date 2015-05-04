#include <vector>
#include "utils.hpp"

class LinOp{	
	public:
		operatorType type;
		void* data;
		int size[2];
		std::vector< LinOp* > args;
};

Matrix type_to_coeff(operatorType){
	switch(operatorType){
		case VARIABLE:
			//@ TODO FOR JOHN
			break;
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
		case SCALAR_CONST:
			//@ TODO FOR JOHN
			break;
		case DENSE_CONST:
			//@ TODO FOR JOHN
			break;
		case SPARSE_CONST:
			//@ TODO FOR JOHN
			break;
		case PARAM:
			//@ TODO FOR JOHN
			break;
		case NO_OP:
			//@ TODO FOR JOHN
			break;
		case CONSTANT_ID:
			//@ TODO FOR JOHN
			break;
	}
	return NULL;
}