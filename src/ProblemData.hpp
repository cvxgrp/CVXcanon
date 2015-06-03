#ifndef PROBLEMDATA_H
#define PROBLEMDATA_H

#include <vector>
#include <map>

class ProblemData{	
	public:
		std::vector<double> V;
		std::vector<int> I; 
		std::vector<int> J;
		std::vector<double> const_vec;
		std::map<int, int> id_to_col;
		std::map<int, int> const_to_row;

		// loads data into a NUMPY array VALUES.
		// probably specifid to the python wrapper?
		void getV(double* values, int numValues){
			for(int i = 0; i < numValues; i++){
				values[i] = V[i];
			}
		}
		void getI(double* values, int numValues){
			for(int i = 0; i < numValues; i++){
				values[i] = I[i];
			}
		}
		void getJ(double* values, int numValues){
			for(int i = 0; i < numValues; i++){
				values[i] = J[i];
			}
		}

		void getConstVec(double* values, int numValues){
			for(int i = 0; i < numValues; i++){
				values[i] = const_vec[i];
			}
		}
};

#endif