#include "lin_op.h"
#include <vector>

class LinOpTree{
	public:		
		LinOp linOp;
		std::vector<LinOpTree *> children;
};