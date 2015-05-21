import numpy as np
from cvxpy import *
import matplotlib.pyplot as plt


np.random.seed(1)
N = 100
settings.USE_CVXCANON = True
# create an increasing input signal
xtrue = np.zeros(N)
xtrue[0:40] = 0.1
xtrue[49] = 2
xtrue[69:80] = 0.15
xtrue[79] = 1
xtrue = np.cumsum(xtrue)

# pass the increasing input through a moving-average filter 
# and add Gaussian noise
h = np.array([1, -0.85, 0.7, -0.3])
k = h.size
yhat = np.convolve(h,xtrue)
y = yhat[0:-3].reshape(N,1) + np.random.randn(N,1)

xtrue = np.asmatrix(xtrue.reshape(N,1))
y = np.asmatrix(y)

v = Variable(N)
x = Variable(N)

constraints = [x >= 0]
for i in range(N - 1):
	constraints.append( x[i+1] >= x[i] )

constraints.append( y == ( conv(h,x)[0:-3] + v ) )

obj = Minimize( norm(v) )
prob = Problem(obj, constraints)
prob.solve()

print x.value
print v.value

plt.figure(1)
plt.plot(xtrue)
plt.plot(x.value)
plt.legend(["True signal", "MLE signal"])
plt.title("Maximum likelihood reconstruction of FIR filter with constraints")
plt.show()



constraints.append( y == ( conv(h,x)[0:-3] + v ) )
obj = Minimize( norm(v) )
prob = Problem(obj, constraints)
prob.solve()

plt.figure(1)
plt.plot(xtrue)
plt.plot(x.value)
plt.legend(["True signal", "MLE signal"])
plt.title("Maximum likelihood reconstruction of FIR filter no constraints")
plt.show()