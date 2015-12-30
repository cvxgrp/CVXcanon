#ifndef SOLVERS_H
#define SOLVERS_H

#import <ecos.h>
#import <stdio.h>
#import "ProblemData.hpp"
#import "Solution.hpp"
#import "CVXcanon.hpp"


// Just to show that ecos compiles with CVXcanon

void testInfeasible()
{
	idxint n 		= 1; //Number of primal variables
	idxint m 		= 2; //Number of constraints, i.e. dimension 1 of the matrix G and length of vector h
	idxint p 		= 0; //Number of equality constraints, i.e. dimension 1 of matrix A and length of vector b
	idxint l 		= 2; //Dimension of the positive orthant Rl+
	idxint ncones 	= 0; //Number of second-order cones present in problem
	idxint *q = NULL;	 //[1] = {3};		 //Array of length ncones; q[i] defines the dimension of the cone i
	idxint e 		= 0; //Number of exponential cones present in problem
	
	// G and A matrices in CCS
	pfloat Gpr[] = {-1.000000000000000000e+00, 1.000000000000000000e+00};
	idxint Gjc[] = {0.0, 2}; //indptr
	idxint Gir[] = {0.0, 1};   //indices
	pfloat *Apr = NULL;//[0];
	idxint *Ajc = NULL;//[0];
	idxint *Air = NULL;//[0];
	pfloat c[] = {-1}; //Array of length n
	pfloat h[] = {-2.000000000000000000e+00, 1.000000000000000000e+00}; //Array of length m
	pfloat *b = NULL;			//Array of length p. Can be NULL if no equality constraints are present
	 
	pwork* work = ECOS_setup( n, m, p, l, ncones, q, e,
		               		  Gpr, Gjc, Gir,
		               		  Apr, Ajc, Air,
		               		  c, h, b);

	idxint flag = ECOS_solve(work);

	ECOS_cleanup(work, 0);

	printf("ECOS finished with %ld\n", flag);
}


/*

Objective and Constraint Linop Trees for the CVXpy problem:

x = Variable(1)
objective = Minimize(x)
constraints = [x >= 1]
prob = Problem(objective, constraints)

Objective:
LinOp(type='variable', size=(1, 1), args=[], data=0)

Constraint:
[LinLeqConstr(expr=
	LinOp(type='sum', size=(1, 1), args=[
		LinOp(type='scalar_const', size=(1, 1), args=[], data=1), 
		LinOp(type='neg', size=(1, 1), args=[
			LinOp(type='variable', size=(1, 1), args=[], data=0)]
		, data=None)]
	, data=None)
, constr_id=2, size=(1, 1))]

*/

void testCVXCanon()
{
	LinOp objective;
	objective.type = VARIABLE;
	objective.size.push_back(1);
	objective.size.push_back(1);
	objective.sparse = false;
	objective.dense_data = Eigen::MatrixXd(1,1);
	objective.dense_data(0,0) = 0;

	// Constraint root
	LinOp constraint;
	constraint.type = LEQ;
	constraint.size.push_back(1);
	constraint.size.push_back(1);
	constraint.sparse = false;
	constraint.dense_data = Eigen::MatrixXd(1,1);
	constraint.dense_data(0,0) = 2;

	// Constant (1)
	LinOp constant;
	constant.type = SUM;
	constant.size.push_back(1);
	constant.size.push_back(1);
	constant.sparse = false;

	// Variable (x)
	LinOp x;
	x.type = VARIABLE;
	x.size.push_back(1);
	x.size.push_back(1);
	x.sparse = false;
	x.dense_data = Eigen::MatrixXd(1,1);
	x.dense_data(0,0) = 0;

	// Negative
	LinOp neg;
	neg.type = NEG;
	neg.size.push_back(1);
	neg.size.push_back(1);
	neg.sparse = false;
	neg.args.push_back(&x);

	// Sum
	LinOp sum;
	sum.type = SUM;
	sum.size.push_back(1);
	sum.size.push_back(1);
	sum.sparse = false;

	sum.args.push_back(&constant);
	sum.args.push_back(&neg);

	constraint.args.push_back(&sum);

	std::vector< LinOp* > constraints;
	constraints.push_back(&constraint);

	std::map<std::string, double> arguments;
	Solution s = solve(MINIMIZE, &objective, constraints, arguments);
}

int main()
{
	testInfeasible();
	testCVXCanon();
}

#endif