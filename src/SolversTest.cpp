#ifndef SOLVERS_H
#define SOLVERS_H

#import <ecos.h>
#import <stdio.h>
#import "ProblemData.hpp"


// Just to show that ecos compiles with CVXcanon

int main()
{
	ProblemData problemdata; // lol

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

#endif