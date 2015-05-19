from cvxpy import *
import numpy
<<<<<<< .merge_file_X23cYo
x = Variable(2)
y = Variable(2)
print Problem(Minimize(0), [x + y <= [1, 0]]).solve()
=======

x = Variable(1)
print Problem(Minimize(x),[x >= 4]).solve()
>>>>>>> .merge_file_wOMV3o
'''
x = Variable(2)

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
'''