#include "Utils.hpp"
#include <vector>
#include <map>

class ProblemData{	
	public:
		std::vector<double> V;
		std::vector<int> I; 
		std::vector<int> J;
		std::vector<double> data;
		std::map<int, int> id_to_col;
		std::map<int, int> const_to_row;
		~ProblemData(){ };
};