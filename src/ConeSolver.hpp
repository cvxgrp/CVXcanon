// Interface for cone solvers

#ifndef CONE_SOLVER_H
#define CONE_SOLVER_H

class ConeProblem {
  // A, b, c
};

class ConeSolution {
  // x
};

class ConeSolver {
 public:
  virtual ~ConeSolver() {}
  virtual ConeSolution solve(const ConeProblem& problem) = 0;
};

#endif  // CONE_SOLVER_H
