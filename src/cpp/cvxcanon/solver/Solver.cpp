
#include "Solver.hpp"

#include <glog/logging.h>

#include "cvxcanon/expression/TextFormat.hpp"
#include "cvxcanon/solver/SymbolicConeSolver.hpp"
#include "cvxcanon/solver/cone/SplittingConeSolver.hpp"
#include "cvxcanon/transform/LinearConeTransform.hpp"

Solution solve(const Problem& problem, const SolverOptions& solver_options) {
  // TODO(mwytock): Allow for different transforms/solvers as per SolveOptions

  VLOG(1) << "input problem:\n" << format_problem(problem);
  LinearConeTransform transform;
  Problem cone_problem = transform.apply(problem);

  VLOG(1) << "cone problem:\n" << format_problem(cone_problem);
  SymbolicConeSolver solver(std::make_unique<SplittingConeSolver>());
  return solver.solve(cone_problem);
}
