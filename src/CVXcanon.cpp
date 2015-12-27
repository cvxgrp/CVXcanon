//    This file is part of CVXcanon.
//
//    CVXcanon is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    CVXcanon is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with CVXcanon.  If not, see <http://www.gnu.org/licenses/>.

#include "CVXcanon.hpp"
#include <map>
#include <ecos.h>
#include "BuildMatrix.hpp"
#include "LinOp.hpp"

/**
 * Takes as list of contraints as input. Returns a map from constraint
 * type to list of linOp trees for each type.
 * 
 * Constraint Types: 	EQ, 		// equality constraint
 *										LEQ, 		// non-negative orthant
 *										SOC, 		// second-order cone
 *										EXP, 		// exponential cone
 *										SDP, 		// semi-definite cone
 */
std::map<OperatorType, std::vector<LinOp *> > filter_constraints(std::vector<LinOp *> constraints){
	std::map<OperatorType, std::vector<LinOp *> > constr_map;
	for (unsigned i = 0; i < constraints.size(); i++){
		LinOp constr_root = *constraints[i];

		// Type of Constraint and corresponding expression tree
		// assuming node has exactly one argument, the root of the linOp Tree
		OperatorType type = constr_root.type;
		LinOp *tree = constr_root.args[0]

		// check constraint type and add root of LinOp tree to appropriate vector
		map<OperatorType, std::vector<LinOp *> >::iterator iter = constr_map.find(type);
		if(iter != constr_map.end()){
			iter->second.push_back(tree); 
		} else {
			std::vector<linOp *> > vec;
			vec.push_back(tree);
			constr_map.insert(std::pair<OperatorType, std::vector<linOp *> > (type, vec));
		}
	}
	return constr_map;
}

std::map<OperatorType, std::vector<int> > compute_dimensions(std::map<OperatorType, std::vector<LinOp *> > constr_map){
	std::map<OperatorType, std::vector<int> > dims;
	std::map<OperatorType, std::vector<LinOp *> >::iterator it = constr_map.begin();
	for( ; it != constr_map.end(); it++){
		OperatorType constr_type = it->first;
		
		// TODO: Seperate handling for each constraint type.... look at cvxpy for one possible
		// implementation of how to do this...
	}
	return dims;
}



Solution solve(Sense sense, LinOp* objective, std::vector<LinOp *> constraints,
							 std::map<std::string, double> arguments){
	/* Pre-process constraints */
	std::map<OperatorType, std::vector<LinOp *> > constr_map = filter_constraints(contraints);
	std::map<OperatorType, std::vector<int> > dim_map = compute_dimensions(constr_map);


	/* Instiantiate problem data (convert appropriate linOp trees to sparse matrix form) */

	/* Call ECOS and solve the problem */

	/* post-process ECOS call and build solution object */
	
	return Solution();
}