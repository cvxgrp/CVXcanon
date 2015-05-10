#include "Utils.hpp"
#include <vector>
#include <map>

class ProblemData{	
	public:
		std::vector<Matrix> V;
		std::vector<int> I; 
		std::vector<int> J;
		Vector data;
		std::map<int, int> id_to_col;
		std::map<int, int> const_to_row;
		~ProblemData(){ };
};