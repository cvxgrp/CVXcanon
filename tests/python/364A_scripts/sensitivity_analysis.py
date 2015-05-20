import numpy as np
from cvxpy import *
import matplotlib.pyplot as plt

x = Variable()
u = Parameter()

limits = np.arange(-10, 10, .1)
objective = Minimize( x**2 )
constraints = [ x**2 - 6 * x + 8 <= u ]

prob = Problem(objective, constraints)
vals = []

for limit in limits:
	u.value = limit
	result = prob.solve()
	vals.append(result)

plt.plot(limits, vals)
plt.xlabel('u')
plt.ylabel('p *')
plt.title('Sensitivity analysis')
plt.show()