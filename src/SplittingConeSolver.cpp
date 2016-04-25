
#include "SplittingConeSolver.hpp"

void SplittingConeSolver::build_scs_problem(const ConeProblem& problem) {
  data_.A = &A_matrix_;
  data_.b = b_.data();
  data_.c = const_cast<double*>(problem.c.data());
}

ConeSolution SplittingConeSolver::build_cone_solution() {
  ConeSolution solution;
  return solution;
}

ConeSolution SplittingConeSolver::solve(const ConeProblem& problem) {
  build_scs_problem(problem);
  scs::scs(&data_, &cone_, &sol_, &info_);
  return build_cone_solution();
}
