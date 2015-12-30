#ifndef ECOSPROBLEM_H
#define ECOSPROBLEM_H

#include <vector>
#include <map>
#include <ecos.h>
#include "CVXcanon.hpp"
#include "LinOp.hpp"
#include "Solution.hpp"

class EcosProblem {
public:
	EcosProblem(Sense sense, LinOp * objective, 
							std::map<OperatorType, std::vector<LinOp *> > constr_map,
							std::map<OperatorType, std::vector<int> > dims_map,
							std::map<int, int> var_offsets,
							int num_variables);

	 ~EcosProblem();
	Solution solve(std::map<std::string, double> solver_options);

private:
	/* ECOS problem */ 
	pwork* problem; 

	/* Dimensions */
	long n;
	long m;
	long p;
	long l;
	long ncones;
	std::vector<long> q;
	long e;

	/* Problem Matrics in CCS format */
	/* Inequality Constraints */
	std::vector<double> Gpr;
	std::vector<long> Gir;
	std::vector<long> Gjc;
	std::vector<double> h;

	/* Equality Constraints */
	std::vector<double> Apr;
	std::vector<long> Air;
	std::vector<long> Ajc;
	std::vector<double> b;

	/* Objective */
	std::vector<double> c;
	double offset;
};

#endif