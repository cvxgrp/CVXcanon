// Build matrices and call a ConeSolver

#ifndef SYMBOLIC_CONE_SOLVER_H
#define SYMBOLIC_CONE_SOLVER_H

#include "Solver.hpp"

class SymbolicConeSolver : public Solver {
 public:
  SymbolicConeSolver(std::unique_ptr<ConeSolver> solver);
  Solution solve(const Problem& problem) override;

 private:
  std::unique_ptr<ConeSolver> cone_solver_;
};

#endif  // SYMBOLIC_CONE_SOLVER_H
