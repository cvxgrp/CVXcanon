
from cvxcanon import cvxcanon_swig
from cvxcanon import expression

def solve(cvxpy_problem, **kwargs):
    problem = expression.convert_problem(cvxpy_problem)
    options = cvxcanon_swig.SolverOptions()
    solution = cvxcanon_swig.solve(problem, options)
    # TODO(mwytock): Copy over variable values, likely when we switch from SWIG
    # to Cython.
    return solution.objective_value
