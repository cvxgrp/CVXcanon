// Interface to the splitting conic solver (SCS)
//
// See: https://github.com/cvxgrp/scs

#ifndef SPLITTING_CONIC_SOLVER_H
#define SPLITTING_CONIC_SOLVER_H

#include "ConeSolver.hpp"

class SplittingConeSolver : public ConeSolver {
 public:
  ConeSolution solve(const ConeProblem& problem) override;
};

#endif  // SPLITTING_CONIC_SOLVER_H
