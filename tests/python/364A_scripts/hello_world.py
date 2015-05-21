from cvxpy import *
import numpy

x = Variable(2)
settings.USE_CVXCANON = True
constraints = [ 2 * x[0] + x[1] >= 1, x[0] + 3 * x[1] >= 1, \
x[0] >= 0, x[1] >= 0 ]

objectives  =   [x[0] + x[1], -x[0] - x[1], x[0],\
max_elemwise( x[0], x[1] ), x[0] ** 2 + 9 * x[1] ** 2 ]    

for n, obj in enumerate(objectives):
	prob = Problem(Minimize(obj), constraints)
	prob.solve()
	print "Solution to objective " + str(n)
	print x.value
	print obj.value
	print