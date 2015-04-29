#include <vector>


class LinOp{
	enum operatorType {
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
		PARAM,
		NO_OP,
		CONSTANT_ID
	};
	
	public:
		operatorType type;
		void * data;
		int size[2];
		std::vector<int> args;
};