
#include "SymbolicConeSolver.hpp"

int build_constraint(
    const Expression& expr,
    ConeProblem* cone_problem,
    int offset) {
  int dim_expr = dim(expr);
  AffineCoefficients coeffs = build_coefficients(expr);
  // Fill in
  return dim_expr;
}

void build_objective(const Expression& expr, ConeProblem* cone_problem) {
  AffineCoefficients coeffs = build_coefficients(expr);
  CHECK_EQ(1, coeffs.rows());
  cone_problem->c = coeffs.A.row(0);
}

ConeProblem build_cone_problem(
    const Problem& problem,
    const VariableOffsetMap& var_offsets) {
  ConeProblem cone_problem;
  build_objective(problem.objective, &cone_problem);

  int offset = 0;
  for (const Expression& constr : problem.constraints)
    offset += build_constraint(constr, offset, &cone_problem);

  return cone_problem;
}

Solution build_solution(
    const ConeSolution& cone_solution,
    const VariableOffsetMap& var_offsets) {
  Solution solution;
  for (int var_id : var_offsets.variable_ids()) {
    solution.values[var_id] = cone_solution.x.segment(
        var_offsets.get(var_id), var_offsets.size(var_id));
  }
  return solution;
}

Solution SymbolicConeSolver::solve(const Problem& problem) {
  VariableOffsetMap var_offsets = build_variable_offset_map(problem);
  ConeProblem problem = build_cone_problem(problem, var_offsets);
  ConeSolution solution = cone_solver_->solve(problem);
  return build_solution(solution, var_offsets);
}
