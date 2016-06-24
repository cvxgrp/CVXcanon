
#include "cvxcanon/solver/cone/EcosConeSolver.hpp"

#include <unordered_map>
#include <vector>

#include "cvxcanon/util/MatrixUtil.hpp"

namespace ecos {
#include "ecos/include/ecos.h"
}

ConeSolution EcosConeSolver::solve(const ConeProblem& problem) {
  ConeSolution solution;

  ecos::settings settings;
  ecos::pwork pwork;
  ecos::stats stats;

  // TODO(mwytock): solve using ecos, e.g. ECOS_setup()...

  ECOS_solve(&pwork);

  return solution;
}
