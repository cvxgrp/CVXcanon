import CVXcanonPy
import numpy as np

def get_sparse_matrix(constrs):
	linOps = [constr.expr for constr in constrs]
	args = CVXcanonPy.LinOpVector()
	for lin in linOps:
		tree = build_lin_op_tree(lin)
		args.push_back(tree)

	print "Calling C++ code"
	problemData = CVXcanonPy.build_matrix(args)

	V, I, J, b = ([], [], [], [])
	for i in range(problemData.V.size()):
		V.append(problemData.V[i])
		I.append(problemData.I[i])
		J.append(problemData.J[i])
		b.append(problemData.data[i])

	return (V, I, J, np.array(b))

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
	linC.size.push_back( int(linPy.size[0]) )
	linC.size.push_back( int(linPy.size[1]) )

	# Updating the arguments
	for argPy in linPy.args:
		linC.args.push_back(build_lin_op_tree(argPy))

	return linC