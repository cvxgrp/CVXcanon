"""Handles translating CVXPY expressions to CVXcanon expressions."""

import numpy as np
import scipy.sparse as sp

from cvxcanon import cvxcanon_swig
from cvxcanon.cvxcanon_swig import Expression, Problem

import cvxpy.atoms.affine.add_expr
import cvxpy.atoms.affine.binary_operators
import cvxpy.atoms.affine.unary_operators
import cvxpy.atoms.pnorm
import cvxpy.atoms.quad_over_lin
import cvxpy.expressions.constants.constant
import cvxpy.expressions.variables.variable


SENSE_MAP = {
    cvxpy.problems.objective.Maximize: Problem.MAXIMIZE,
    cvxpy.problems.objective.Minimize: Problem.MINIMIZE,
}

TYPE_MAP = {
    cvxpy.atoms.affine.add_expr.AddExpression: Expression.ADD,
    cvxpy.atoms.affine.binary_operators.MulExpression: Expression.MUL,
    cvxpy.atoms.affine.diag.diag_mat: Expression.DIAG_MAT,
    cvxpy.atoms.affine.diag.diag_vec: Expression.DIAG_VEC,
    cvxpy.atoms.affine.hstack.hstack: Expression.HSTACK,
    cvxpy.atoms.affine.index.index: Expression.INDEX,
    cvxpy.atoms.affine.kron.kron: Expression.KRON,
    cvxpy.atoms.affine.reshape.reshape: Expression.RESHAPE,
    cvxpy.atoms.affine.sum_entries.sum_entries: Expression.SUM_ENTRIES,
    cvxpy.atoms.affine.trace.trace: Expression.TRACE,
    cvxpy.atoms.affine.unary_operators.NegExpression: Expression.NEG,
    cvxpy.atoms.affine.upper_tri.upper_tri: Expression.UPPER_TRI,
    cvxpy.atoms.affine.vstack.vstack: Expression.VSTACK,
    cvxpy.atoms.elementwise.abs.abs: Expression.ABS,
    cvxpy.atoms.elementwise.entr.entr: Expression.ENTR,
    cvxpy.atoms.elementwise.exp.exp: Expression.EXP,
    cvxpy.atoms.elementwise.huber.huber: Expression.HUBER,
    cvxpy.atoms.elementwise.kl_div.kl_div: Expression.KL_DIV,
    cvxpy.atoms.elementwise.log.log: Expression.LOG,
    cvxpy.atoms.elementwise.log1p.log1p: Expression.LOG1P,
    cvxpy.atoms.elementwise.logistic.logistic: Expression.LOGISTIC,
    cvxpy.atoms.elementwise.max_elemwise.max_elemwise: Expression.MAX_ELEMWISE,
    cvxpy.atoms.elementwise.power.power: Expression.POWER,
    cvxpy.atoms.geo_mean: Expression.GEO_MEAN,
    cvxpy.atoms.lambda_max: Expression.LAMBDA_MAX,
    cvxpy.atoms.log_det: Expression.LOG_DET,
    cvxpy.atoms.log_sum_exp: Expression.LOG_SUM_EXP,
    cvxpy.atoms.matrix_frac: Expression.MATRIX_FRAC,
    cvxpy.atoms.max_entries: Expression.MAX_ENTRIES,
    cvxpy.atoms.norm_nuc.normNuc: Expression.NORM_NUC,
    cvxpy.atoms.pnorm: Expression.P_NORM,
    cvxpy.atoms.quad_over_lin: Expression.QUAD_OVER_LIN,
    cvxpy.atoms.sigma_max: Expression.SIGMA_MAX,
    cvxpy.atoms.sum_largest: Expression.SUM_LARGEST,
    cvxpy.constraints.eq_constraint.EqConstraint: Expression.EQ,
    cvxpy.constraints.leq_constraint.LeqConstraint: Expression.LEQ,
    cvxpy.expressions.constants.constant.Constant: Expression.CONST,
    cvxpy.expressions.constants.parameter.Parameter: Expression.PARAM,
    # NOTE(mwytock): This maps all variables to a VAR expression, constraints
    # will be added explicitly according to VARIABLE_CONSTRAINT_MAP, below
    cvxpy.expressions.variables.variable.Variable: Expression.VAR,
    cvxpy.expressions.variables.semidef_var.SemidefUpperTri: Expression.VAR,
}

def get_slice(sl, arg_dim):
    """Convert the python slice operator to swig form, handling None."""
    slice_swig = cvxcanon_swig.Slice()

    # step
    if sl.step is not None:
        slice_swig.step = sl.step
    else:
        slice_swig.step = 1

    # start
    if sl.start is not None:
        if sl.start >= 0:
            slice_swig.start = sl.start
        else:
            slice_swig.start = sl.start + arg_dim
        slice_swig.start = min(slice_swig.start, arg_dim-1)
    elif slice_swig.step < 0:
        slice_swig.start = arg_dim - 1
    else:
        slice_swig.start = 0

    # stop
    if sl.stop is not None:
        if sl.stop >= 0:
            slice_swig.stop = sl.stop
        else:
            slice_swig.stop = sl.stop + arg_dim
        slice_swig.stop = min(slice_swig.stop, arg_dim)
    elif slice_swig.step < 0:
        slice_swig.stop = -1
    else:
        slice_swig.stop = arg_dim


    return slice_swig

def get_constant(value):
    constant = cvxcanon_swig.Constant()
    if sp.issparse(value):
        coo = sp.coo_matrix(value)
        constant.set_sparse_data(
            coo.data,
            coo.row.astype(float),
            coo.col.astype(float),
            coo.shape[0],
            coo.shape[1])
    else:
        constant.set_dense_data(np.asfortranarray(np.atleast_2d(value)))
    return constant

def get_axis(atom):
    return cvxcanon_swig.kNoAxis if atom.axis is None else atom.axis

def get_var_attributes(variable):
    attr = cvxcanon_swig.VarAttributes()
    attr.id = variable.id
    attr.size.dims.push_back(variable.size[0])
    attr.size.dims.push_back(variable.size[1])
    return attr

def get_semidef_upper_tri_attributes(variable):
    attr = get_var_attributes(variable)
    attr.variable_type = cvxcanon_swig.VarAttributes.SEMIDEF_UPPER_TRI
    return attr

def get_const_attributes(constant):
    attr = cvxcanon_swig.ConstAttributes()
    attr.constant = get_constant(constant.value)
    return attr

def get_pnorm_attributes(pnorm):
    attr = cvxcanon_swig.PNormAttributes()
    attr.p = float(pnorm.p)
    attr.axis = get_axis(pnorm)
    return attr

def get_power_attributes(power):
    attr = cvxcanon_swig.PowerAttributes()
    attr.p = float(power.p)
    return attr

def get_index_attributes(index):
    attr = cvxcanon_swig.IndexAttributes()
    for i, key in enumerate(index.key):
        attr.keys.push_back(get_slice(key, index.args[0].size[i]))
    return attr

def get_huber_attributes(huber):
    attr = cvxcanon_swig.HuberAttributes()
    attr.M = convert_expression(huber.M)
    return attr

def get_sum_entries_attributes(sum_entries):
    attr = cvxcanon_swig.SumEntriesAttributes()
    attr.axis = get_axis(sum_entries)
    return attr

def get_reshape_attributes(reshape):
    attr = cvxcanon_swig.ReshapeAttributes()
    attr.size.dims.push_back(reshape.rows)
    attr.size.dims.push_back(reshape.cols)
    return attr

def get_param_attributes(param):
    attr = cvxcanon_swig.ParamAttributes()
    attr.id = param.id
    attr.size.dims.push_back(param.size[0])
    attr.size.dims.push_back(param.size[1])
    attr.constant = get_constant(param.value)
    return attr

ATTRIBUTE_MAP = {
    cvxpy.atoms.affine.index.index: get_index_attributes,
    cvxpy.atoms.affine.reshape.reshape: get_reshape_attributes,
    cvxpy.atoms.affine.sum_entries.sum_entries: get_sum_entries_attributes,
    cvxpy.atoms.elementwise.huber.huber: get_huber_attributes,
    cvxpy.atoms.pnorm: get_pnorm_attributes,
    cvxpy.atoms.power: get_power_attributes,
    cvxpy.expressions.constants.constant.Constant: get_const_attributes,
    cvxpy.expressions.constants.parameter.Parameter: get_param_attributes,
    cvxpy.expressions.variables.semidef_var.SemidefUpperTri: get_var_attributes,
    cvxpy.expressions.variables.variable.Variable: get_var_attributes,
}

def convert_expression(cvxpy_expr):
    args = cvxcanon_swig.ExpressionVector()
    for arg in cvxpy_expr.args:
        args.push_back(convert_expression(arg))
    t = type(cvxpy_expr)
    expr = cvxcanon_swig.Expression(
        TYPE_MAP[t],
        args,
        ATTRIBUTE_MAP.get(t, lambda x: None)(cvxpy_expr))
    return expr


# TODO(mwytock): Converting variable constraints in this fashion is a bit
# cumbersome, it would be more convenient if constraint expressions were first
# class citizens in CVXPY.
def get_semidef_upper_tri_constraints(cvxpy_var):
    args = cvxcanon_swig.ExpressionVector()
    args.push_back(convert_expression(cvxpy_var))
    yield cvxcanon_swig.Expression(Expression.SDP_VEC, args, None)

VARIABLE_CONSTRAINT_MAP = {
    cvxpy.expressions.variables.semidef_var.SemidefUpperTri: get_semidef_upper_tri_constraints,
}

def convert_variable_constraints(cvxpy_var):
    t = type(cvxpy_var)
    for constr in VARIABLE_CONSTRAINT_MAP.get(t, lambda x: [])(cvxpy_var):
        yield constr

def convert_problem(cvxpy_problem):
    problem = cvxcanon_swig.Problem()
    problem.sense = SENSE_MAP[type(cvxpy_problem.objective)]
    problem.objective = convert_expression(cvxpy_problem.objective.args[0])
    for cvxpy_constr in cvxpy_problem.constraints:
        problem.constraints.push_back(convert_expression(cvxpy_constr))
    for cvxpy_var in cvxpy_problem.variables():
        for constr in convert_variable_constraints(cvxpy_var):
            problem.constraints.push_back(constr)

    return problem
