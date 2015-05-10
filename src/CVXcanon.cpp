#include <iostream>
#include <map>
#include "LinOp.hpp"
#include "ProblemData.hpp"


std::map<int, Matrix> mul_by_const(Matrix & coeff_mat, std::map<int, Matrix> & rh_coeffs){
	std::map<int, Matrix> result;
	for(auto & kv : rh_coeffs){
		int id = kv.first;
		result[id] = coeff_mat * rh_coeffs[id];
	} 
	return result;
}


std::map<int, Matrix> get_coefficient(LinOp &lin){
	std::map<int, Matrix> coeffs;
	if(lin.type == VARIABLE){
		coeffs = get_variable_coeffs(lin);
	}
	else if( lin.hasConstantType()){
		coeffs = get_const_coeffs(lin);
	}
	else{
		std::vector<Matrix> coeff_mat = get_func_coeffs(lin);
		for(unsigned i = 0; i < lin.args.size(); i++){
			Matrix coeff = coeff_mat[i];
			std::map<int, Matrix> rh_coeffs = get_coefficient(*lin.args[i]);
			std::map<int, Matrix> new_coeffs = mul_by_const(coeff, rh_coeffs);
			coeffs.insert(new_coeffs.begin(), new_coeffs.end());	
		}
	}
	return coeffs;
}


int get_horiz_offset(int id, std::map<int, int> & offsets, int * horiz_offset, LinOp & lin){
	if( !offsets.count(id) ){
		offsets[id] = *horiz_offset;
		* horiz_offset += lin.size[0] * lin.size[1];
	}
	return offsets[id];
}

void process_constraint(LinOp & lin, std::vector<Matrix> V,
					    std::vector<int> I, std::vector<int> J,
						Vector constant_vec, int vert_offset, 
						std::map<int, int> id_to_col, int * horiz_offset){
	
	std::map<int, Matrix> coeffs = get_coefficient(lin);
	for(auto & kv : coeffs){
		int id = kv.first;
		Matrix block = coeffs[id];
		int vert_start = vert_offset;
		if ( lin.hasConstantType() ){
			constant_vec.middleRows(vert_start, block.rows() * block.cols()) = block; 			// Double check this part 
		}
		else {
			int offset = get_horiz_offset(id, id_to_col, horiz_offset, lin);
			V.push_back(block);
			I.push_back(vert_start + block.rows()); 	// Copying this from Steven Diamond's code, but doesn't seem right...
			J.push_back(offset + block.cols());
		}
	}
}



int getTotalConstraintLength(std::vector< LinOp* > constraints){
	int result = 0;
	for(unsigned i = 0; i < constraints.size(); i++)
		result += constraints[i]->size[0]*constraints[i]->size[1];
	return result;
}

ProblemData build_matrix(std::vector< LinOp* > constraints){
	ProblemData probData;
	int numRows = getTotalConstraintLength(constraints);
	probData.data.resize(numRows, 1);
	int vert_offset = 0;
	int * horiz_offset;
	*horiz_offset = 0;
	for(unsigned i = 0; i < constraints.size(); i++){
		LinOp constr = *constraints[i];
		process_constraint(constr, probData.V, probData.I, probData.J,
						   probData.data, vert_offset, 
						   probData.id_to_col, horiz_offset);

		probData.const_to_row[i] = vert_offset;
		vert_offset += constr.size[0] * constr.size[1]; 		
	}
	return probData;
}


int main()
{
  std::cout << "Hello World!\n";
}