from cvxpy.expressions.expression import *
from cvxpy.expressions.constants import Constant
from cvxpy.expressions.variables import Variable
from cvxpy import Problem, Minimize
from cvxpy import *


settings.USE_CVXCANON = False

c = Constant([1,2,3,4])
x = Variable(4)
p = Problem(Minimize(0), [x[::-1] == c])

print p.get_problem_data(ECOS)

p.solve()

print x.value

settings.USE_CVXCANON = True

c = Constant([1,2,3,4])
x = Variable(4)
p = Problem(Minimize(0), [x[::-1] == c])

print p.get_problem_data(ECOS)

p.solve()

print x.value

