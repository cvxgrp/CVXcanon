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
#include <set>
#include <ecos.h>
#include "BuildMatrix.hpp"
#include "LinOp.hpp"
#include "LinOpOperations.hpp"

/**
 * Takes as list of contraints as input. Returns a map from constraint
 * type to list of linOp trees for each type.
 * 
 * Constraint Types: 	EQ, 		// equality constraint
 *										LEQ, 		// non-negative orthant
 *										SOC, 		// second-order cone
 *										EXP, 		// exponential cone *** TODO: Needs special formatting (elemwise) for ECOS! **
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
		LinOp *tree = constraint.args[0];
		constr_map[constraint.type].push_back(tree);
	}

	return constr_map;
}

int accumulate_size(std::vector<LinOp *> constraints){
	int size = 0;
	for (int i = 0; i < constraints.size(); i++){
		LinOp constr = *constraints[i];
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
		LinOp constr = *soc_constr[i];
		dims[SOC].push_back(constr.size[0]);
	}

	// EXP
	int num_exp_cones = accumulate_size(constr_map[EXP]);
	dims[EXP].push_back(num_exp_cones);

	return dims;
}

struct Variable {
	int id;
	std::vector<int> size;
	bool operator < (const Variable &other) const { return id < other.id; }
};

// returns a vector of all variable 
std::vector<Variable> get_expr_vars(LinOp &expr){
	std::vector<Variable> vars; 
	if(expr.type == VARIABLE){
		vars.push_back(Variable());
		vars[0].id = get_id_data(expr);
		vars[0].size = expr.size;
	} else {
		for(int i = 0; i < expr.args.size(); i++){
			std::vector<Variable> new_vars = get_expr_vars(*expr.args[i]);
			vars.insert(vars.end(), new_vars.begin(), new_vars.end());
		}
	}
	return vars;
}

// Potential ISSUE: Remove the root linop since it specifics constraints before
// going to var offsets?
int get_var_offsets(LinOp *objective, std::vector<LinOp*> constraints,
										std::map<int, int> &var_offsets){
	std::vector<Variable> vars = get_expr_vars(*objective);
	for(int i = 0; i < constraints.size(); i++){
		std::vector<Variable> constr_vars = get_expr_vars(*constraints[i]);
		vars.insert(vars.end(), constr_vars.begin(), constr_vars.end());
	}

	// remove duplicates and sort by ID to ensure variables always have same order
	std::set<int> s(vars.begin(), vars.end());
	vars.assign(s.begin(), s.end());
	std::sort(vars.begin(), vars.end());

	int vert_offset = 0; // number of variables in the problem
	for(int i = 0; i < vars.size(); i++){
		Variable var = vars[i];
		var_offsets[var.id] = vert_offset;
		vert_offset += var.size[0] * var.size[1];
	}

	return vert_offset;
}

std::vector<LinOp *> concatenate(std::vector<LinOp *> A, std::vector<LinOp *> B, std::vector<LinOp *> C){
	std::vector<LinOp *> ABC;
	ABC.reserve(A.size() + B.size() + C.size());
	ABC.insert(ABC.end(), A.begin(), A.end());
	ABC.insert(ABC.end(), B.begin(), B.end());
	ABC.insert(ABC.end(), C.begin(), C.end());
	return ABC;
}

std::vector<double> negate(std::vector<double> &vec){
	std::vector<double> neg_vec;
	for(int i = 0; i < vec.size(); i++){
		neg_vec.push_back(-vec[i]);
	}
	return neg_vec;
}

std::vector<double> get_obj_vec(Sense sense, ProblemData objData, int n){
	std::vector<double> c;
	for(int i = 0; i < n; i++){
		c.push_back(0.0);
	}

	int s = 1;
	if(sense == MAXIMIZE){
		s = -1;
	} 

	std::vector<int> idxs = objData.J;
	for(int i = 0; i < idxs.size(); i++){
		int idx = idxs[i];
		c[idx] = s * objData.V[idx];
	}
	return c;
}

pwork* initialize_problem(Sense sense, LinOp * objective, 
												 	std::map<OperatorType, std::vector<LinOp *> > constr_map,
													std::map<OperatorType, std::vector<int> > dims_map,
													std::map<int, int> var_offsets,
													int num_variables, double offset){
	/* get problem data */
	std::vector<LinOp *> objVec;
	objVec.push_back(objective);
	std::vector<LinOp *> ineqConstr = concatenate(constr_map[LEQ], constr_map[SOC], constr_map[EXP]);

	ProblemData objData = build_matrix(objVec, var_offsets);
	ProblemData eqData = build_matrix(constr_map[EQ], var_offsets);
	ProblemData ineqData = build_matrix(ineqConstr, var_offsets);

	/* Problem Dimensions */
	idxint n 	= num_variables;
	idxint m 	= ineqData.num_constraints;
	idxint p 	= dims[EQ];
	idxint l 	= dims[LEQ];
	idxint ncones = dims[SOC].size();
	if(ncones > 0){
		idxint *q = &dims[SOC][0]; // TODO: Verify this is portable
	} else {
		idxint *q = NULL;	 //Array of length ncones; q[i] defines the dimension of the cone i	
	}
	idxint e 	= dims[EXP];
	
	/* Extract G and A matrices in CCS */
	ineqData.toCSC();
	pfloat *Gpr = &ineqData.vals[0]; 
	idxint *Gjc = &ineqData.col_ptrs[0];
	idxint *Gir = &ineqData.row_idxs[0];
	pfloat *h = &negate(ineqData.const_vec)[0];

	eqData.toCSC();
	if(dims[EQ] == 0) {
		pfloat *Apr = NULL;
		idxint *Ajc = NULL;
		idxint *Air = NULL;
		pfloat *b = NULL;
	} else {
		pfloat *Apr = &eqData.vals[0];
		idxint *Ajc = &eqData.col_ptrs[0];
		idxint *Air = &eqData.row_idxs[0];
		pfloat *b = &negate(eqData.const_vec)[0];
	}

	double offset = objData.const_vec[0];
	pfloat *c = &get_obj_vec(sense, objData, num_variables)[0];

	pwork* problem = ECOS_setup(n, m, p, l, ncones, q, e,
													 		Gpr, Gjc, Gir,
													 		Apr, Ajc, Air,
													 		c, h, b);
	return offset;
}

Solution solve(Sense sense, LinOp* objective, std::vector<LinOp *> constraints,
							 std::map<std::string, double> arguments){
	/* Pre-process constraints */
	std::map<OperatorType, std::vector<LinOp *> > constr_map = filter_constraints(contraints);
	std::map<OperatorType, std::vector<int> > dims_map = compute_dimensions(constr_map);

	std::map<int, int> var_offsets;
	int num_variables = get_var_offsets(objective, constraints, &var_offsets);

	/* Instantiate problem data (convert appropriate linOp trees to sparse matrix form) */
	double offset;
	pwork* problem = initialize_problem(sense, objective, constr_map,
																			dims_map, var_offsets, num_variables, &offset);

	/* Call ECOS and solve the problem */
	idxint status = ECOS_solve(problem);

	cout << 'OPTIMAL VALUE: ' << problem->best_cx + offset << endl;

	/* post-process ECOS call and build solution object */
	// TODO. Need to add the offset to the primal value
	return Solution();
}