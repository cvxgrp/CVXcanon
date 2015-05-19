#include "CVXcanon.hpp"

#include <iostream>
#include <map>
#include "LinOp.hpp"
#include "LinOpOperations.hpp"
#include "ProblemData.hpp"

std::map<int, std::vector<Matrix> > mul_by_const(Matrix & coeff_mat, std::map<int, std::vector<Matrix> > & rh_coeffs){
	std::map<int, std::vector<Matrix> > result;
	for(auto & kv : rh_coeffs){
		int id = kv.first;
		for(unsigned i = 0; i < kv.second.size(); i++){
			Matrix rh = kv.second[i];
			printf("LHS rows: %d, Columns: %d\n", coeff_mat.rows(), coeff_mat.cols());
			printf("RHS rows: %d, Columns: %d\n", rh.rows(), rh.cols());
			result[id].push_back( coeff_mat * rh );
		}
	} 
	return result;
}


std::map<int, std::vector<Matrix> > get_coefficient(LinOp &lin){
	std::map<int, std::vector<Matrix> > coeffs;
	if(lin.type == VARIABLE){ 				// If a lin op is a variable, we hit one of our base cases
		std::map<int, Matrix> new_coeffs = get_variable_coeffs(lin);
		for( auto & kv : new_coeffs){
			coeffs[kv.first].push_back(kv.second);
		}
	}
	else if( lin.hasConstantType()){								// If it is a constant, we hit our other base case
		std::map<int, Matrix> new_coeffs = get_const_coeffs(lin);	// id here will be CONSTANT_TYPE
		for( auto & kv : new_coeffs){
			coeffs[kv.first].push_back(kv.second);
		}
	}
	else{
		std::vector<Matrix> coeff_mat = get_func_coeffs(lin); // The function coefficient by which we multiply the arguments
		for(unsigned i = 0; i < lin.args.size(); i++){		  // in order
			Matrix coeff = coeff_mat[i];
			std::map<int, std::vector<Matrix> > rh_coeffs = get_coefficient(*lin.args[i]);
			std::map<int, std::vector< Matrix> > new_coeffs = mul_by_const(coeff, rh_coeffs);
			for( auto & kv : new_coeffs ) {
				coeffs[kv.first].insert( coeffs[kv.first].end(), 
											kv.second.begin(),
											kv.second.end() );
			}			
		}
	}
	return coeffs;
}


int get_horiz_offset(int id, std::map<int, int> & offsets, int & horiz_offset, LinOp & lin){
	if( !offsets.count(id) ){
		offsets[id] = horiz_offset;
		horiz_offset += lin.size[0] * lin.size[1];
	}
	return offsets[id];
}



/* function: add_matrix_to_vectors
* 
* This function adds a matrix to our sparse matrix triplet
* representation, by using eigen's sparse matrix iterator 
* This function takes horizontal and vertical offset, which indicate
* the offset of this block within our larger matrix.
*/

void add_matrix_to_vectors(Matrix & block, std::vector<double> & V, std::vector<int>  &I,
					 	std::vector<int> & J, int vert_offset, int & horiz_offset){
	for ( int k=0; k < block.outerSize(); ++k ){
		for ( Matrix::InnerIterator it(block,k); it; ++it ){
		    V.push_back(it.value());
		    I.push_back(it.row() + vert_offset);   	// pushing back the current row index
		    J.push_back(it.col() + horiz_offset);   // pushing back current column index
		}
	}
}

void extend_constant_vec(std::vector<double> &const_vec, Matrix &block){
	for ( int k=0; k < block.outerSize(); ++k ){
		for ( Matrix::InnerIterator it(block,k); it; ++it ){
	    	const_vec.push_back(it.value());
		}
	}
}

void process_constraint(LinOp & lin, std::vector<double> &V,
					    std::vector<int> &I, std::vector<int> &J,
						std::vector<double> &constant_vec, int vert_offset, 
						std::map<int, int> &id_to_col, int & horiz_offset){
	std::map<int, std::vector<Matrix> > coeffs = get_coefficient(lin);	// Getting the coefficient for the current constraint 	
	for(auto & kv : coeffs){
		int id = kv.first;									// We determine the horiz offset by the id
		std::vector<Matrix> blocks = coeffs[id];							//
		for(unsigned i = 0; i < blocks.size(); i++){
			Matrix block = blocks[i];		
			if ( id == CONSTANT_ID ){							// If this LinOp is a we add these coefficients  
				 	extend_constant_vec(constant_vec, block);	// to our constant vec.
			}
			else {
				int offset = get_horiz_offset(id, id_to_col, horiz_offset, lin);
				add_matrix_to_vectors(block, V, I, J, vert_offset, offset);
			}
		}
	}
}

int getTotalConstraintLength(std::vector< LinOp* > constraints){
	int result = 0;
	for(unsigned i = 0; i < constraints.size(); i++){
		result += constraints[i]->size[0]*constraints[i]->size[1];
	}
	return result;
}

ProblemData build_matrix(std::vector< LinOp* > constraints){
	ProblemData probData;
	int numRows = getTotalConstraintLength(constraints);
	probData.data.reserve(numRows);
	int vert_offset = 0;
	int horiz_offset  = 0;
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