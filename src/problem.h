#include "lin_op_tree.h"

class Problem{
	LinOpTree objective;
	std::vector<LinOpTree *> constraints;
};