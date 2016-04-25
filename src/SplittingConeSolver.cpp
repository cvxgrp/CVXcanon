
#include "SplittingConeSolver.hpp"

void SplittingConeSolver::build_scs_problem(const Problem& problem) {
  data_.A = A_matrix_.get();
  data_.b = b_.data();
  data_.c = problem.c.data();
}

ConeSolution SplittingConeSolver::build_cone_solution() {
  ConeSolution solution;
  return solution;
}

ConeSolution SplittingConeSolver::solve(const ConeProblem& problem) {
  build_scs_problem(problem);
  scs(data_, cone_, &sol_, &info_);
  return build_cone_solution();
}
