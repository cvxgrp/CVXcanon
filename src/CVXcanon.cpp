#include "CVXcanon.hpp"

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


void add_matrix_to_vectors(Matrix & block, std::vector<double> & V, std::vector<int>  &I,
					 	std::vector<int> & J, int vert_offset, int horiz_offset){
	for ( int k=0; k < block.outerSize(); ++k ){
  		for ( Matrix::InnerIterator it(block,k); it; ++it ){
		    V.push_back(it.value());
		    I.push_back(it.row() + vert_offset);   // row index
		    J.push_back(it.col() + horiz_offset);   // col index (here it is equal to k)
  		}
  	}
}

void process_constraint(LinOp & lin, std::vector<double> V,
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
			add_matrix_to_vectors(block, V, I, J, offset, vert_offset);
		}
	}
}



int getTotalConstraintLength(std::vector< LinOp* > constraints){
	int result = 0;
	for(unsigned i = 0; i < constraints.size(); i++)
		result += constraints[i]->size[0]*constraints[i]->size[1];
	return result;
}

void add_problem_data(Vector constant_vec, std::vector<double> & problem_data){
	for(unsigned i = 0; i < constant_vec.rows(); i++){
		problem_data.push_back(constant_vec[i]);
	}
}

ProblemData build_matrix(std::vector< LinOp* > constraints){
	ProblemData probData;
	int numRows = getTotalConstraintLength(constraints);
	probData.data.resize(numRows, 1);
	int vert_offset = 0;
	int horiz_offset  = 0;
	Vector data;
	for(unsigned i = 0; i < constraints.size(); i++){
		LinOp constr = *constraints[i];
		process_constraint(constr, probData.V, probData.I, probData.J,
						   data, vert_offset, 
						   probData.id_to_col, & horiz_offset);

		probData.const_to_row[i] = vert_offset;
		vert_offset += constr.size[0] * constr.size[1]; 		
	}
	add_problem_data(data, probData.data);
	return probData;
}


// int main()
// {
//   std::cout << "Hello World!\n";
// }