import CVXcanon
import numpy as np
from  cvxpy.lin_ops.lin_op import *
from pdb import set_trace as bp
import scipy.sparse


def get_sparse_matrix(constrs, id_to_col=None):
    linOps = [constr.expr for constr in constrs]
    args = CVXcanon.LinOpVector()

    id_to_col_C = CVXcanon.IntIntMap()
    if id_to_col is None:
        id_to_col = {}

    for id, col in id_to_col.items():
        id_to_col_C[id] = col

    # make sure things stay in scope..
    tmp = []
    for lin in linOps:
        tree = build_lin_op_tree(lin, tmp)
        tmp.append(tree)
        args.push_back(tree)

    print "Calling C++ code"
    problemData = CVXcanon.build_matrix(args, id_to_col_C)
    print "Returned from C++ code"

    V, I, J, b = ([], [], [], [])
    for i in range(problemData.V.size()):
        V.append(problemData.V[i])
        I.append(problemData.I[i])
        J.append(problemData.J[i])

    for i in range(problemData.data.size()):
        b.append(problemData.data[i])

    # print V, I, J, b
    return (V, I, J, np.array(b).reshape(-1, 1))


def push_dense(linC, linPy):
  if isinstance(linPy.data, LinOp):  # huge shitman special casing...
    if linPy.data.type is 'sparse_const':
      coo = scipy.sparse.coo_matrix(linPy.data.data)
      linC.addSparseData(coo.data, coo.row, coo.col)
    elif linPy.data.type is 'dense_const':
      data = np.ascontiguousarray(linPy.data.data)
      linC.addDenseData(data)
    else:
      raise NotImplementedError()
  else:
    data = linPy.data
    rows, cols = data.shape
    for row in xrange(rows):
      vec = CVXcanon.DoubleVector()
      for col in xrange(cols):
        vec.push_back(data[row, col])
      linC.data.push_back(vec)


def build_lin_op_tree(linPy, tmp):
  linC = CVXcanon.LinOp()
  # Setting the type of our lin op
  linC.type = eval("CVXcanon." + linPy.type.upper())
  # Loading the data into our array
  if linPy.data is None:
    pass
  elif isinstance(linPy.data, tuple)\
  and isinstance(linPy.data[0], slice): # Tuple of slices
    for i, sl in enumerate(linPy.data):
      vec = CVXcanon.DoubleVector()
      #bp()
      if (sl.start is None):
        vec.push_back(0)
      elif(sl.start < 0):
        vec.push_back(linPy.args[0].size[i] + (sl.start))
      else:
        vec.push_back(sl.start)
      if(sl.stop is None):
        vec.push_back(linPy.args[0].size[i])
      elif(sl.stop < 0):
        vec.push_back(linPy.args[0].size[i] + (sl.stop))
      else:
        vec.push_back(sl.stop)
      if sl.step is None:
        vec.push_back(1.0)
      else:
        vec.push_back(sl.step)
      linC.data.push_back(vec)     
  elif isinstance(linPy.data, float) or isinstance(linPy.data, int):
    vec = CVXcanon.DoubleVector()
    vec.push_back(linPy.data)
    linC.data.push_back(vec)
  elif isinstance(linPy.data, LinOp) and linPy.data.type is 'scalar_const':
    vec = CVXcanon.DoubleVector()
    vec.push_back(linPy.data.data)
    linC.data.push_back(vec)
  else:
    push_dense(linC, linPy)
  # Setting size
  linC.size.push_back( int(linPy.size[0]) )
  linC.size.push_back( int(linPy.size[1]) )

  # Updating the arguments
  for argPy in linPy.args:
    tree = build_lin_op_tree(argPy, tmp)
    tmp.append(tree)
    linC.args.push_back(tree)

  return linC