# Given a lin-op tree linPy
# Returns a C lin-op tree object
import CVXcanonPy
from  cvxpy.lin_ops.lin_op import *

def build_lin_op_tree(linPy):
	linC = CVXcanonPy.LinOp()
	# Setting the type of our lin op
	linC.type =  eval("CVXcanonPy." + linPy.type.upper()) 
	# Loading the data into our array
	if linPy.data is None:
		pass
	elif isinstance(linPy.data, tuple)\
	and isinstance(linPy.data[0], slice): # Tuple of slices 
		for sl in linPy.data:
			vec = CVXcanonPy.DoubleVector()
			vec.push_back(sl.start)
			vec.push_back(sl.stop)
			if sl.step is None:
				vec.push_back(1.0)
			else:
				vec.push_back(sl.step)
			linC.data.push_back(vec) 		 
	elif isinstance(linPy.data, float) or isinstance(linPy.data, int):
		vec = CVXcanonPy.DoubleVector()
		vec.push_back(linPy.data)
		linC.data.push_back(vec)
	elif isinstance(linPy.data, LinOp):
		vec = CVXcanonPy.DoubleVector()
		vec.push_back(linPy.data.data)
		linC.data.push_back(vec)
	else:
		for row in linPy.data:
			vec = CVXcanonPy.DoubleVector()
			for entry in row:
				vec.push_back(entry)
			linC.data.push_back(vec)
			
	# Setting size
	linC.size.push_back( linPy.size[0] )
	linC.size.push_back( linPy.size[1] )
	# Updating the arguments
	for argPy in linPy.args:
		linC.args.push_back(build_lin_op_tree(argPy))
