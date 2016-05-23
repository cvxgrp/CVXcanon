"""Test solvers called from CVXcanon.

TODO(mwytock): Add comprehensive tests, all solvers, etc.
"""

import cvxpy as cp
import numpy as np

from cvxcanon import cvxpy_solver

def test_lasso():
    np.random.seed(0)

    m = 5
    n = 10
    lam = 1

    A = np.random.randn(m,n)
    b = np.random.randn(m)
    x = cp.Variable(n)
    f = cp.sum_squares(A*x - b) + lam*cp.norm1(x)
    prob = cp.Problem(cp.Minimize(f))

    problem_data = prob.get_problem_data(cp.SCS)
    print problem_data["dims"]
    print "A:"
    print problem_data["A"].todense()
    print "b:", problem_data["b"]
    print "c:", problem_data["c"]


    result0 = prob.solve()
    status, result1 = cvxpy_solver.solve(prob)

    print result0
    print result1

    tol = 1e-2
    assert status == cp.OPTIMAL
    assert -tol <= (result1 - result0)/(1+np.abs(result0)) <= tol
