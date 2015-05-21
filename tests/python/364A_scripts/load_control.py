# data for optimal evacuation problem
from cvxpy import *
import numpy as np
import matplotlib.pyplot as plt
import math


m = .1
theta = 15.0 / 180.0 * math.pi
settings.USE_CVXCANON = True
T_max = 200
p_int = np.asmatrix([0.0,0.0])
p_des = np.asmatrix([10.0,2.0])
h = .1

lc = np.asmatrix([-math.sin(theta), math.cos(theta)]).T
rc = np.asmatrix([math.sin(theta), math.cos(theta)]).T
g = np.asmatrix([0.0, -9.8]).T 
print lc, rc, g

k_max = 100

last_feasible_k = None
last_feasible_p = None
last_feasible_t = None

k_min = 0
k = (k_min+k_max)/2
while k_min < k_max:
	k += 1
	print k
	F = Variable(2, k)
	Tl = Variable(k)
	Tr = Variable(k)
	p = Variable(2,k)
	v = Variable(2,k)
	constraints = [ p[:, 0] == p_int.T, p[:, k-1] == p_des.T, \
	v[:,0] == 0, v[:,k-1] == 0 ]
	
	for i in range(k - 1):
		constraints.append(p[:,i+1] == p[:,i] + h * v[:,i] )
		constraints.append(v[:,i+1] == p[:,i] + (h / m ) * (Tl[i] * lc + Tr[i] * rc + m*g) )


	constraints.append(0 <= Tl)
	constraints.append(0 <= Tr)
	constraints.append(T_max >= Tl)
	constraints.append(T_max >= Tr)


	obj = Minimize(0)
	prob = Problem(obj, constraints)
	val = prob.solve(solver="ECOS")
	print val
	if val == float('inf'):
		pass
		#k_min = k + 1
	else:
		#k_max = k - 1
		last_feasible_k = k
		last_feasible_p = p.T.value
		last_feasible_t = [Tl.value, Tr.value]

plt.plot(last_feasible_p[:,0], last_feasible_p[:,1])
plt.title("Trajectory")
plt.show()



plt.plot(last_feasible_t[0]/50.0)
plt.hold(True)
plt.plot(last_feasible_t[1]/50.0)
plt.show()
plt.title("Trajectory")

print k


