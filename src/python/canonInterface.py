import CVXcanon
import numpy as np
from  cvxpy.lin_ops.lin_op import *
from pdb import set_trace as bp
import scipy.sparse
from collections import deque


def get_sparse_matrix(constrs, id_to_col=None):
    # print constrs
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

    # print "Calling C++ code"
    problemData = CVXcanon.build_matrix(args, id_to_col_C)
    V = problemData.getV(len(problemData.V))
    I = problemData.getI(len(problemData.I))
    J = problemData.getJ(len(problemData.J))
    const_vec = problemData.getConstVec(len(problemData.const_vec))

    # print "Returned from C++ code"

    # V, I, J, b = ([], [], [], [])
    # for i in range(problemData.V.size()):
    #     V.append(problemData.V[i])
    #     I.append(problemData.I[i])
    #     J.append(problemData.J[i])

    # for i in range(problemData.data.size()):
    #     b.append(problemData.data[i])
    # print V, I, J, b
    # return problemData, np.asarray(problemData.const_vec).reshape(-1, 1)
    return V, I, J, const_vec.reshape(-1, 1)


def push_dense(linC, linPy, tmp):
  if isinstance(linPy.data, LinOp):  # huge shitman special casing...
    (rows, cols) = linPy.data.data.shape
    linC.dataRows = rows 
    linC.dataCols = cols
    if linPy.data.type is 'sparse_const':
      coo = scipy.sparse.coo_matrix(linPy.data.data)
      tmp.append(coo)
      linC.addSparseData(coo.data, coo.row.astype(float), coo.col.astype(float))
    elif linPy.data.type is 'dense_const':
      mat = np.asfortranarray(linPy.data.data)
      tmp.append(mat)
      linC.addDenseData(mat)
    else:
      raise NotImplementedError()
  else:
    (rows, cols) = linPy.data.shape
    linC.dataRows = rows 
    linC.dataCols = cols
    mat = np.asfortranarray(linPy.data)
    tmp.append(mat)
    linC.addDenseData(mat)

   

def build_lin_op_tree(root_linPy, tmp):
  Q = deque()
  root_linC = CVXcanon.LinOp()
  Q.append((root_linPy, root_linC))
  while len(Q) > 0:
    linPy, linC = Q.popleft()
    # Updating the arguments
    for argPy in linPy.args:
      tree = CVXcanon.LinOp()
      tmp.append(tree)
      Q.append((argPy, tree))
      linC.args.push_back(tree)

    # Setting the type of our lin op
    linC.type = eval("CVXcanon." + linPy.type.upper())
    

    # Setting size
    linC.size.push_back( int(linPy.size[0]) )
    linC.size.push_back( int(linPy.size[1]) )


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
      linC.dataRows = 1
      linC.dataCols = 1
      mat = np.asfortranarray(np.matrix(linPy.data))
      tmp.append(mat)
      linC.addDenseData(mat)
    elif isinstance(linPy.data, LinOp) and linPy.data.type is 'scalar_const':
      vec = CVXcanon.DoubleVector()
      vec.push_back(linPy.data.data)
      linC.data.push_back(vec)
      linC.dataRows = 1
      linC.dataCols = 1
      mat = np.asfortranarray(np.matrix(linPy.data.data))
      tmp.append(mat)
      linC.addDenseData(mat)

    else:
      push_dense(linC, linPy, tmp)

  return root_linC