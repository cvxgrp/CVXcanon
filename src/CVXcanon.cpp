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
 *										SDP, 		// semi-definite cone **** NOT CURRENTLY SUPPORTED
 */
std::map<OperatorType, std::vector<LinOp *> > filter_constraints(std::vector<LinOp *> constraints){
	std::map<OperatorType, std::vector<LinOp *> > constr_map;
	constr_map.insert(std::pair<OperatorType, std::vector<LinOp *> >(EQ, std::vector<LinOp *>()));
	constr_map.insert(std::pair<OperatorType, std::vector<LinOp *> >(LEQ, std::vector<LinOp *>()));
	constr_map.insert(std::pair<OperatorType, std::vector<LinOp *> >(SOC, std::vector<LinOp *>()));
	constr_map.insert(std::pair<OperatorType, std::vector<LinOp *> >(EXP, std::vector<LinOp *>()));

	for (int i = 0; i < constraints.size(); i++){
		LinOp constraint = *constraints[i];
		// assuming node has exactly one argument, the root of the linOp Tree
		LinOp *tree = constraint.args[0]
		constr_map[constraint.type].push_back(tree);
	}

	return constr_map;
}

int accumulate_size(std::vector<LinOp *> constraints){
	int size = 0;
	for (int i = 0; i < constraints.size(); i++){
		LinOp constr = constraints[i]
		size += constr.size[0] * constr.size[1];
	}
	return size;
}

std::map<OperatorType, std::vector<int> > compute_dimensions(std::map<OperatorType, std::vector<LinOp *> > constr_map){
	std::map<OperatorType, std::vector<int> > dims;
	dims.insert(std::pair<OperatorType, std::vector<int> >(EQ, std::vector<int>()));
	dims.insert(std::pair<OperatorType, std::vector<int> >(LEQ, std::vector<int>()));
	dims.insert(std::pair<OperatorType, std::vector<int> >(SOC, std::vector<int>()));
	dims.insert(std::pair<OperatorType, std::vector<int> >(EXP, std::vector<int>()));

	// equality
	int EQ_dim = accumulate_size(constr_map[EQ]);
	dims[EQ].push_back(EQ_dim);

	// positive orthant
	int LEQ_dim = accumulate_size(constr_map[LEQ]);
	dims[LEQ].push_back(LEQ_dim);

	// dims[SOC] defines the dimension of the i-th SOC 
	std::vector<LinOp *> soc_constr = constr_map[SOC];
	for (int i = 0; i < soc_constr.size(); i++){
		LinOp constr = *soc_constr[i]
		dims[SOC].push_back(constr.size[0]);
	}

	// EXP
	int num_exp_cones = accumulate_size(constr_map[EXP]);
	dims[EXP].push_back(num_exp_cones);

	return dims;
}

// returns a vector of all variable 
std::vector<std::vector<int> > get_expr_vars(LinOp * expr){
	// TODO!
	return NULL;
}

std::map<int, int> get_var_offsets(LinOp * objective, std::vector<LinOp*> constraints){
	// TODO!
	return NULL;
}

std::vector<LinOp *> concatenate(std::vector<LinOp *> A, std::vector<LinOp *> B, std::vector<LinOp *> C){
	std::vector<LinOp *> ABC;
	ABC.reserve(A.size() + B.size() + C.size());
	ABC.insert(ABC.end(), A.begin(), A.end());
	ABC.insert(ABC.end(), B.begin(), B.end());
	ABC.insert(ABC.end(), C.begin(), C.end());
	return ABC;
}

pwork* initialize_problem(Sense sense, LinOp * objective, 
												 	std::map<OperatorType, std::vector<LinOp *> > constr_map,
													std::map<OperatorType, std::vector<int> > dims_map,
													std::map<int, int> var_offsets){
	/* get problem data */
	std::vector<LinOp *> objVec;
	objVec.push_back(objective);
	std::vector<LinOp *> ineqConstr = concatenate(constr_map[LEQ], constr_map[SOC], constr_map[EXP]);

	ProblemData objData = build_matrix(objVec, var_offsets);
	ProblemData eqData = build_matrix(constr_map[EQ], var_offsets);
	ProblemData ineqData = build_matrix(ineqConstr, var_offsets);

	/* Problem Dimensions */
	// TODO!!! 
	idxint n 	= 1; //Number of primal variables
	idxint m 	= 2; //Number of constraints, i.e. dimension 1 of the matrix G and length of vector h
	idxint p 	= 0; //Number of equality constraints, i.e. dimension 1 of matrix A and length of vector b
	idxint l 	= 2; //Dimension of the positive orthant Rl+
	idxint ncones = 0; //Number of second-order cones present in problem
	idxint *q = NULL;	 //[1] = {3};		 //Array of length ncones; q[i] defines the dimension of the cone i
	idxint e 	= 0; //Number of exponential cones present in problem
	
	/* Extract G and A matrices in CCS */
	// TODO!!!
	pfloat Gpr[] = {-1.000000000000000000e+00, 1.000000000000000000e+00};
	idxint Gjc[] = {0.0, 2}; //indptr
	idxint Gir[] = {0.0, 1};   //indices
	pfloat *Apr = NULL;//[0];
	idxint *Ajc = NULL;//[0];
	idxint *Air = NULL;//[0];
	pfloat c[] = {-1}; //Array of length n
	pfloat h[] = {-2.000000000000000000e+00, 1.000000000000000000e+00}; //Array of length m
	pfloat *b = NULL;			//Array of length p. Can be NULL if no equality constraints are present

	// TODO: CHANGE C depending on the sense!
	pwork* problem = ECOS_setup(n, m, p, l, ncones, q, e,
													 		Gpr, Gjc, Gir,
													 		Apr, Ajc, Air,
													 		c, h, b);
	return problem;
}

Solution solve(Sense sense, LinOp* objective, std::vector<LinOp *> constraints,
							 std::map<std::string, double> arguments){
	/* Pre-process constraints */
	std::map<OperatorType, std::vector<LinOp *> > constr_map = filter_constraints(contraints);
	std::map<OperatorType, std::vector<int> > dims_map = compute_dimensions(constr_map);
	std::map<int, int> var_offsets = get_var_offsets(LinOp* objective, constraints);

	/* Instiantiate problem data (convert appropriate linOp trees to sparse matrix form) */
	pwork* problem = initialize_problem(sense, objective, constr_map, dims_map, var_offsets);

	/* Call ECOS and solve the problem */
	idxint status = ECOS_solve(problem);

	/* post-process ECOS call and build solution object */
	
	return Solution();
}