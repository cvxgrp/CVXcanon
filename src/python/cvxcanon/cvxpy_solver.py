
import cvxpy
from cvxcanon import cvxcanon_swig
from cvxcanon import expression

STATUS_MAP = {
    cvxcanon_swig.OPTIMAL: cvxpy.OPTIMAL,
    cvxcanon_swig.INFEASIBLE: cvxpy.INFEASIBLE,
    cvxcanon_swig.UNBOUNDED: cvxpy.UNBOUNDED,
    cvxcanon_swig.ERROR: cvxpy.SOLVER_ERROR,
}

def solve(cvxpy_problem, **kwargs):
    problem = expression.convert_problem(cvxpy_problem)
    options = cvxcanon_swig.SolverOptions()
    solution = cvxcanon_swig.solve(problem, options)

    # TODO(mwytock): Copy over variable values, likely when we switch from SWIG
    # to Cython.

    return STATUS_MAP[solution.status], solution.objective_value
