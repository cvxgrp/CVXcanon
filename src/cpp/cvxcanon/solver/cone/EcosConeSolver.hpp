// Interface to the splitting conic solver (SCS)
//
// See: https://github.com/cvxgrp/scs
//
// TODO(mwytock): It is expected that this interface will be implemented
// directly in the SCS code base at some point and called via a plugin
// architecture, see comment about pure C interface in README.md.

#ifndef CVXCANON_SOLVER_CONE_ECOS_CONE_SOLVER_H
#define CVXCANON_SOLVER_CONE_ECOS_CONE_SOLVER_H

#include <memory>
#include <vector>

#include "cvxcanon/solver/cone/ConeSolver.hpp"

class EcosConeSolver : public ConeSolver {
 public:
  ConeSolution solve(const ConeProblem& problem) override;

 private:
};

#endif  // CVXCANON_SOLVER_CONE_ECOS_CONE_SOLVER_H
