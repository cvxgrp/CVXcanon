import numpy as np
from cvxpy import *


numVariables = 4


def cumProb( p, numbers, values):
 	sum = None
	
	for i in range(2 ** numVariables):

		rules = []

		for j in range(len(numbers)):
			rules.append( ((i & ( 1 << ( numbers[j] -  1 ) )) >> ( numbers[j] -  1 )) == values[j] )  

		if all(rules):
			if(sum is None):
				sum = p[i]
			else:
				sum += p[i]
	print sum
	return sum


p = Variable( 2 ** numVariables )

constraints = [sum(p) == 1]

constraints.append(p >= 0)

constraints.append( cumProb( p, [1], [1]) == .9)
constraints.append( cumProb( p, [2], [1]) == .9 )
constraints.append( cumProb( p, [3], [1]) == .1 )
constraints.append( cumProb( p, [3], [1]) == .1 )

constraints.append( cumProb(p, [3, 4, 1], [1, 0, 1]) == \
.7 * cumProb(p, [3], [1])  )

constraints.append( cumProb(p, [2, 3, 4], [1, 0, 1]) == \
.6 * cumProb(p, [2, 3], [1, 0])  )


objective = Maximize( cumProb(p, [4], [1] ) )
prob = Problem(objective, constraints)
print "Maximum probability, ", prob.solve()


objective = Minimize( cumProb(p, [4], [1] ))
prob = Problem(objective, constraints)
print "Minimum probability, ", prob.solve()