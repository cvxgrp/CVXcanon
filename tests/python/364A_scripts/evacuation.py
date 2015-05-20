# data for optimal evacuation problem
from cvxpy import *
import numpy as np
import matplotlib.pyplot as plt
import math


T = 20
A = np.array([[-1.,-1., 0., 0., 0., 0., 0., 0., 0.],
 [ 1., 0.,-1., 0., 0., 0., 0., 0., 0.],
 [ 0., 0., 1.,-1., 0., 0., 0., 0., 0.],
 [ 0., 1., 0., 0.,-1.,-1., 0., 0., 0.],
 [ 0., 0., 0., 0., 1., 0.,-1., 0., 0.],
 [ 0., 0., 0., 1., 0., 0., 1.,-1., 0.],
 [ 0., 0., 0., 0., 0., 1., 0., 0.,-1.],
 [ 0., 0., 0., 0., 0., 0., 0., 1., 1.]])

Q = np.array([ 1. , 1. , 1. , 1. , 1. , 0.8, 1. , 0.4])
F = np.array([ 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25])
q1 = np.array([ 1., 0., 0., 0., 0., 0., 0., 0.])
r = np.asmatrix( np.array([ 1. , 0.2, 0.2, 0.5, 0.5, 0. , 0.5, 0. ]))
s = np.array([ 1. , 0.2, 0.2, 0.5, 0.5, 0. , 0.5, 0. ])
rtild = np.array([ 0.1, 0.2, 0.1, 5. , 0.4, 0.2, 0.4, 0.4, 0.2])
stild = np.array([   2.8,   5.6,   2.8, 140. ,  11.2,   5.6,  11.2,  11.2,   5.6])
s = np.asmatrix(s)
rtild = np.asmatrix(rtild)
stild = np.asmatrix(stild)


qs = [Variable(len(Q)) for _ in range(T)]
fs = [Variable(len(F)) for _ in range(T)]

obj_func = r * qs[0] + s * square(qs[0])  + rtild * abs(fs[0]) + stild * square(fs[0])

for i in range(1,T-1):
	obj_func += r * qs[i] + s * square(qs[i])  + rtild * abs(fs[i]) + stild * square(fs[i])

obj_func +=  r * qs[T-1] + s * square(qs[T-1])

constraints = [ qs[0] == q1 ]
for i in range(T):
	constraints.append(abs(fs[i]) <= F )
	constraints.append( qs[i] <= Q)
	constraints.append(qs[i] >= 0)

for i in range(T-1):
	constraints.append(qs[i+1] == A*fs[i] + qs[i])

obj = Minimize(obj_func)
prob = Problem(obj, constraints)
val = prob.solve(solver=ECOS)

# Plotting nodes
plt.figure(1)
for i in range(len(Q)):
	plt.plot([qs[t].value[i][0,0] for t in range(T) ])
	plt.hold(True)
plt.legend(["Node " + str(i+1) for i in range(1 + len(Q))])
plt.title("Node occupancy during evacuation")
plt.xlabel("Time")
plt.ylabel("Node occupancy")
plt.show()



plt.figure(2)
for i in range(len(F)):
	plt.plot([fs[t].value[i][0,0] for t in range(T) ])
	plt.hold(True)
plt.legend(["Edge " + str(i+1) for i in range(1 + len(F))])
plt.title("Edge flow during evacuation")
plt.xlabel("Time")
plt.ylabel("Edge flow")
plt.show()



plt.figure(3)

exposures = [r * np.asmatrix(qs[i].value ) + s * np.square(np.asmatrix(qs[i].value ))\
+ rtild * np.abs(np.asmatrix(fs[i].value )) + stild * np.square(np.asmatrix(fs[i].value ) ) for i in range(T)]

exposures = [ exposures[t][0,0] for t in range(T) ]
plt.plot(   exposures ) 
plt.title("Risk exposure flow during evacuation")
plt.xlabel("Time")
plt.ylabel("Exposure")
plt.show()
