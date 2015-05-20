from cvxpy import *
import numpy as np
import matplotlib.pyplot as plt

np.random.seed(1)

T = 96
t = np.linspace(1, T, num=T).reshape(T,1)
p = np.exp(-np.cos((t-15)*2*np.pi/T)+0.01*np.random.randn(T,1))
u = 2*np.exp(-0.6*np.cos((t+40)*np.pi/T) - 0.7*np.cos(t*4*np.pi/T)+0.01*np.random.randn(T,1))


c = Variable(T)
q = Variable(T)

objective = Minimize( p.T * (u + c) )
constraints = [	q[i + 1] == q[i] + c[i] for i in range(T - 1) ]
constraints.append( q[0] == q[T-1] + c[T-1] )

Q = Parameter()
D = Parameter()
C = Parameter()


constraints.append(q <= Q)
constraints.append(c >= -D)
constraints.append(C >= c)
constraints.append(q >= 0)


# Part b
C.value = 3
D.value = 3
Q.value = 35

prob = Problem(objective, constraints)
prob.solve()


plt.figure(1)
plt.plot(t/4, p, 'g'); 
plt.plot(t/4, u, 'r');
plt.plot(t/4, c.value, 'm')
plt.plot(t/4, q.value, 'b')
plt.title('Battery charging (C=D=3)')
plt.legend(['price', 'usage', 'charge rate', 'total charge']) 
plt.show()

# Part c

results = []
limits = range(1,200)
for limit in limits:
	Q.value = limit
	results.append(prob.solve())

plt.figure(2)
plt.plot(limits,results)
plt.title('Tradeoff curve (C=D=3)')
plt.ylabel('Total cost')
plt.xlabel('Maximum charge')
plt.show()


	


results = []
C.value = 1
D.value = 1
limits = range(1,200)
for limit in limits:
	Q.value = limit
	results.append(prob.solve())

plt.figure(2)
plt.plot(limits,results)
plt.title('Tradeoff curve (C=D=1)')
plt.ylabel('Total cost')
plt.xlabel('Maximum charge')
plt.show()
