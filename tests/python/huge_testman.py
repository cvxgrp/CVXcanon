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
	exec("from cvxpy import *\nsettings.USE_CVXCANON=False\n" + open(filename).read())
	oldtime = TIME
	OLD_ANSWERS = ANSWERS


	settings.USE_CVXCANON = True
	exec("from cvxpy import *\nsettings.USE_CVXCANON=True\n" + open(filename).read())
	newtime =  TIME
	NEW_ANSWERS = ANSWERS
	same_lengths = len(NEW_ANSWERS) == len(OLD_ANSWERS)
	same_vals = [ OLD_ANSWERS[i] == NEW_ANSWERS[i] \
	for i in range(min(len(OLD_ANSWERS), len(NEW_ANSWERS)))] 
	if not( same_lengths and all(same_vals) ):
		print "**** TEST " + filename + " FAILED: Different Answers *****"
	else:
		print "***** TEST " + filename + " SUCCESSS, SAME ANSWERS ******"

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


