from cvxpy import *
from os import listdir
import numpy
import glob
import os
import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt



def run_testfile(filename):
	print filename


	settings.USE_CVXCANON = False	
	start = time.time()
	exec("from cvxpy import *\nsettings.USE_CVXCANON=False\n" + open(filename).read())
	oldtime = time.time() - start

	settings.USE_CVXCANON = True
	start = time.time()
	exec("from cvxpy import *\nsettings.USE_CVXCANON=True\n" + open(filename).read())
	newtime =  time.time() - start

	print "oldtime: ", oldtime, ", newtime: ", newtime
	return (oldtime, newtime)

files = glob.glob("./364A_scripts/*.py")

oldtime = [];
newtime = [];
for testfile in files:
	print "testing: ", testfile
	try:
		(o, n) = run_testfile(testfile);
		oldtime += [o]
		newtime += [n]
	except (RuntimeError, TypeError, NameError, AttributeError):
		pass


plt.scatter(oldtime, newtime)
plt.show()


