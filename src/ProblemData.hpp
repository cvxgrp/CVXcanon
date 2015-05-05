#include "Utils.hpp"
#include <vector>
#include <map>

class ProblemData{	
	public:
		Matrix A;
		Vector data;
		std::map<int, int> id_to_col;
		std::map<int, int> const_to_row;
};