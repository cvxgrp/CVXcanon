#include "EcosProblem.hpp"
#include <map>
#include <ecos.h>
#include "ProblemData.hpp"
#include "BuildMatrix.hpp"
#include "LinOp.hpp"
#include "LinOpOperations.hpp"

/******************
 * HELPER FUNCTIONS
********************/
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
		neg_vec.push_back(-1 * vec[i]);
	}
	return neg_vec;
}

std::vector<double> get_obj_vec(ProblemData objData, int n){
	std::vector<double> c(n, 0);
	std::vector<int> idxs = objData.J;
	for(int i = 0; i < idxs.size(); i++){
		c[idxs[i]] = objData.V[idxs[i]];
	}
	return c;
}

std::vector<long> to_long(std::vector<int> v_int){
	std::vector<long> v_long;
	for(int i = 0; i < v_int.size(); i++){
		v_long.push_back((long)v_int[i]);
	}
	return v_long;
}

solverStatus canonicalize_status(idxint status){
	switch(status){
		case 0:
			return OPTIMAL;
		case 1:
			return INFEASIBLE;
		case 2:
			return UNBOUNDED;
		case 10:
			return OPTIMAL_INACCURATE;
		case 11:
			return INFEASIBLE_INACCURATE;
		case 12:
			return UNBOUNDED_INACCURATE;
		case -1:
			return SOLVER_ERROR;
		case -2:
			return SOLVER_ERROR;
		case -3:
			return SOLVER_ERROR;
		case -4:
			return SOLVER_ERROR;
		case -7:
			return SOLVER_ERROR;
	}
	return SOLVER_ERROR;
}

/*********************
 * Public Functions
 *********************/
/* Constructor */
EcosProblem::EcosProblem(Sense sense, LinOp * objective, 
												 	std::map<OperatorType, std::vector<LinOp *> > constr_map,
													std::map<OperatorType, std::vector<int> > dims_map,
													std::map<int, int> var_offsets,
													int num_variables) {
	/* get problem data */
	std::vector<LinOp *> objVec;
	objVec.push_back(objective);
	std::vector<LinOp *> ineqConstr = concatenate(constr_map[LEQ], constr_map[SOC], constr_map[EXP]);

	ProblemData objData = build_matrix(objVec, var_offsets);
	ProblemData eqData = build_matrix(constr_map[EQ], var_offsets);
	ProblemData ineqData = build_matrix(ineqConstr, var_offsets);

	/* Problem Dimensions */
	n = num_variables;
	m = ineqData.num_constraints;
	p = dims_map[EQ][0];
	l = dims_map[LEQ][0];
	ncones = dims_map[SOC].size();
	q = to_long(dims_map[SOC]);
	e = dims_map[EXP][0];
	
	/* Extract G and A matrices in CCS */
	ineqData.toCSC(num_variables);
	Gpr = ineqData.vals;
	Gir = to_long(ineqData.row_idxs);
	Gjc = to_long(ineqData.col_ptrs);
	h = negate(ineqData.const_vec);

	eqData.toCSC(num_variables);
	Apr = eqData.vals;
	Air = to_long(eqData.row_idxs);
	Ajc = to_long(eqData.col_ptrs);
	b = negate(eqData.const_vec);


	offset = objData.const_vec[0];
	c = get_obj_vec(objData, num_variables);
	if(sense == MAXIMIZE){
		c = negate(c);
	}
	problem = ECOS_setup(n, m, p, l, ncones, &q[0], e,
											 &Gpr[0], &Gjc[0], &Gir[0],
											 &Apr[0], &Ajc[0], &Air[0],
											 &c[0], &h[0], &b[0]);

}
EcosProblem::~EcosProblem(){
	ECOS_cleanup(problem, 0); // TODO: Figure out what options for keepvars?
}

Solution EcosProblem::solve(std::map<std::string, double> solver_options){
	idxint status = ECOS_solve(problem); // TODO: Incorporate solver options!
	Solution soln;
	soln.status = canonicalize_status(status);
	soln.optimal_value = problem->best_cx + offset;

	/* TODO: Extract and reformat primal and dual variables */
	return soln;
}