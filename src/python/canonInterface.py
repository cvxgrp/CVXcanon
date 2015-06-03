import CVXcanon
import numpy as np
from cvxpy.lin_ops.lin_op import *
import scipy.sparse
from collections import deque


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

    problemData = CVXcanon.build_matrix(args, id_to_col_C)
    V = problemData.getV(len(problemData.V))
    I = problemData.getI(len(problemData.I))
    J = problemData.getJ(len(problemData.J))
    const_vec = problemData.getConstVec(len(problemData.const_vec))

    return V, I, J, const_vec.reshape(-1, 1)


def set_matrix_data(linC, linPy):
    if isinstance(linPy.data, LinOp):  # huge shitman special casing...
        if linPy.data.type is 'sparse_const':
            rows = linPy.data.data.shape[0]
            cols = linPy.data.data.shape[1]
            coo = scipy.sparse.coo_matrix(linPy.data.data)
            linC.set_sparse_data(coo.data, coo.row.astype(float),
                                 coo.col.astype(float), rows, cols)
        elif linPy.data.type is 'dense_const':
            linC.set_dense_data(np.asfortranarray(linPy.data.data))
        else:
            raise NotImplementedError()
    else:
        linC.set_dense_data(np.asfortranarray(linPy.data))


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
        linC.size.push_back(int(linPy.size[0]))
        linC.size.push_back(int(linPy.size[1]))

        # Loading the data into our array
        if linPy.data is None:
            pass
        elif isinstance(linPy.data, tuple) and isinstance(linPy.data[0], slice):  # Tuple of slices
            for i, sl in enumerate(linPy.data):
                vec = CVXcanon.DoubleVector()
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

                linC.slice.push_back(vec)
        elif isinstance(linPy.data, float) or isinstance(linPy.data, int):
            linC.set_dense_data(np.asfortranarray(np.matrix(linPy.data)))
        elif isinstance(linPy.data, LinOp) and linPy.data.type is 'scalar_const':
            linC.set_dense_data(np.asfortranarray(np.matrix(linPy.data.data)))
        else:
            set_matrix_data(linC, linPy)

    return root_linC
