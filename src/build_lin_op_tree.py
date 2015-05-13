# Given a lin-op tree linPy
# Returns a C lin-op tree object
import CVXcanonPy

def build_lin_op_tree(linPy):
	linC = CVXcanonPy.LinOp()
	# Setting the type of our lin op
	linC.type =  eval("CVXcanonPy.LinOp." + linPy.type.upper) 
	# Loading the data into our array
	if not linPy.data:
		pass
	elif isinstance(linPy.data, tuple)\
	and isinstance(linPy.data[0], slice): # Tuple of slices 
		for sl in linPy.data:
			linC.data.push_back( (sl.start, sl.stop, sl.step)) 		 
	elif isinstance(linPy.data, float):
		linC.data.push_back((linPy.data))
	else:
		for idx, row in enumerate(lin.data):
			linC.data.push_back(tuple(row))
	# Setting size
	linC.size.push_back( linPy.size[0] )
	linC.size.push_back( linPy.size[1] )
	# Updating the arguments
	for argPy in linPy.args:
		linC.args.push_back(build_lin_op_tree(argPy))
