// Interface for all solvers

#ifndef SOLVER_H
#define SOLVER_H

class SolverOptions {
};

class Solution {
};

class Solver {
 public:
  virtual ~Solver() {}
  virtual Solution solve(const Problem& problem) = 0;
};

#endif  // SOLVER_H
