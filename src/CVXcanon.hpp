#include <vector>
#include "LinOp.hpp"

class SparseMatrix{
	public:
		std::vector<int> x_vec;
		std::vector<int> y_vec;
		std::vector<double> coeff_vec;
		void addValue(int x, int y, double coeff){
			x_vec.add(x); y_vec.add(y); coeff_vec.add(coeff);
		}
};

// Top Level Entry point
SparseMatrix get_matrix(std::vector< LinOp* > constraints);


