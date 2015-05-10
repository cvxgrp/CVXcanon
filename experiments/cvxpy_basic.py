import cvxpy as cvx
import numpy as np

optimal_values = np.zeros((5, 1))
# part a
x = cvx.Variable(2)
prob = cvx.Problem(cvx.Minimize(x[0]+x[1]),
              [2*x[0] + x[1] >= 1,
               x[0] + 3*x[1] >= 1,
               x >= 0])
optimal_values[0] = prob.solve()