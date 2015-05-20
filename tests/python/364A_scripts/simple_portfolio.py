import numpy as np
from cvxpy import *
import matplotlib.pyplot as plt


np.random.seed(1)
n = 20
pbar = np.ones((n,1))*.03 + np.r_[np.random.rand(n-1,1), np.zeros((1,1))]*.12;
S = np.random.randn(n, n); S = np.asmatrix(S)
S = S.T*S
S = S/max(np.abs(np.diag(S)))*.2
S[:, -1] = np.zeros((n, 1))
S[-1, :] = np.zeros((n, 1)).T
x_unif = np.ones((n, 1))/n; x_unit = np.asmatrix(x_unif)

x = Variable(n)

unif_return = np.dot(x_unif.T, pbar)
objective = Minimize( quad_form(x, S) )
constraints = [sum_entries(x) == 1, sum_entries(pbar.T * x) >= unif_return]

prob = Problem(objective, constraints)
result = prob.solve()
print "Uniform return:", unif_return
print "No additional constraints"
print "Risk:", result

print "No shorts"
constraints.append(x >= 0 )
prob = Problem(objective, constraints)
result = prob.solve()
print "No additional constraints"
print "Risk:", result


print "Short limit"
constraints.pop()
constraints.append( sum_entries(neg(x)) <= .5)
prob = Problem(objective, constraints)
result = prob.solve()
print "No additional constraints"
print "Risk:", result

# Optimal trade-off curve, no shorts
minimum_return = Parameter(sign="positive")
constraints = [sum_entries(x) == 1, sum_entries(pbar.T * x) >= minimum_return,\
x >= 0]

prob = Problem(objective, constraints)
min_returns = np.logspace(-5, 0, num = 1000)
risk = []
returns = []

for val in min_returns:
	returns.append(val)
	minimum_return.value = val
	result = prob.solve()
	risk.append(result)

plt.plot(risk, returns)
plt.xlabel('Risk')
plt.ylabel('Return')
plt.title('Optimal portfolio with no shorting')
plt.show()

# Optimal trade-off curve, limited shorts
minimum_return = Parameter(sign="positive")
constraints = [sum_entries(x) == 1, sum_entries(pbar.T * x) >= minimum_return,\
sum_entries(neg(x)) <= .5]

prob = Problem(objective, constraints)
min_returns = np.logspace(-5, 0, num=1000)
risk = []
returns = []

for val in min_returns:
	returns.append(val)
	minimum_return.value = val
	result = prob.solve()
	risk.append(result)

plt.plot(risk, returns)
plt.xlabel('Risk')
plt.ylabel('Return')
plt.title('Optimal portfolio with limited shorts')
plt.show()