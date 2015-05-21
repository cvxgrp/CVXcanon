import numpy as np
from cvxpy import *
import matplotlib.pyplot as plt


np.random.seed(0)
(m, n) = (2000, 1000)
A = np.random.rand(m, n); A = np.asmatrix(A)
b = A.dot(np.ones((n, 1)))/2; b = np.asmatrix(b)
c = - np.random.rand(n, 1); c = np.asmatrix(c)
settings.USE_CVXCANON = False
# Solving the LP relaxation 
x = Variable(n)
ts = np.arange(0,1,.01)
obj = Minimize(c.T * x)

constraints = [A*x <= b, 0 <= x, x <= 1]

prob = Problem(obj, constraints)
L = prob.solve()

vals = []
maximum_failure = []
feasible_vals = []
feasible_ts = []
feasible_t = []

for t in ts:
	x_hat = np.zeros( (n,1) )
	for i in range(n):
		if(x.value[i] >= t):
			x_hat[i] = 1.0

	vals.append(c.T * x_hat)
	
	if np.all(A * x_hat <= b):
		feasible_t.append(True)
		feasible_vals.append(c.T * x_hat)
		feasible_ts.append(t)
	else:
		feasible_t.append(False)

	maximum_failure.append(np.amax(A*x_hat - b))

min_t = np.argmin(feasible_vals)
print "Best feasible t, ", feasible_ts[min_t]

for i in range(len(vals)):
	if(feasible_t[i]):
		plt.scatter(ts[i], vals[i], marker='o')
	else:
		plt.scatter(ts[i], vals[i], marker='x')

plt.xlabel('t')
plt.ylabel('Objective value')
plt.title('LP Relaxation')
plt.show()

print maximum_failure

plt.plot(ts, maximum_failure)
plt.xlabel('t')
plt.ylabel('Maximum failure')
plt.show()