// The result of calling a solver on an optimization problem.
//
// Based on http://mathprogbasejl.readthedocs.io/en/latest/solverinterface.html

#ifndef CVXCANON_SOLVER_SOLVER_STATUS_H
#define CVXCANON_SOLVER_SOLVER_STATUS_H

enum SolverStatus {
  OPTIMAL,
  INFEASIBLE,
  UNBOUNDED,
  USER_LIMIT,  // iteration limit or timeout
  ERROR
};

#endif  // CVXCANON_SOLVER_SOLVER_STATUS_H
