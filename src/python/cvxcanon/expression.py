"""Handles translating CVXPY expressions to CVXcanon expressions."""

import numpy as np

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
    cvxpy.atoms.affine.index.index: Expression.INDEX,
    cvxpy.atoms.affine.kron.kron: Expression.KRON,
    cvxpy.atoms.affine.sum_entries.sum_entries: Expression.SUM_ENTRIES,
    cvxpy.atoms.affine.trace.trace: Expression.TRACE,
    cvxpy.atoms.affine.unary_operators.NegExpression: Expression.NEG,
    cvxpy.atoms.affine.upper_tri.upper_tri: Expression.UPPER_TRI,
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
    cvxpy.expressions.variables.variable.Variable: Expression.VAR,
}

def get_var_attributes(variable):
    attr = cvxcanon_swig.VarAttributes()
    attr.id = variable.id
    attr.size.dims.push_back(variable.size[0])
    attr.size.dims.push_back(variable.size[1])
    return attr

def get_const_attributes(constant):
    # TODO(mwytock): Handle sparse data as well
    attr = cvxcanon_swig.ConstAttributes()
    attr.set_dense_data(np.asfortranarray(np.atleast_2d(constant.value)))
    return attr

def get_pnorm_attributes(pnorm):
    attr = cvxcanon_swig.PNormAttributes()
    attr.p = pnorm.p
    return attr

def get_power_attributes(power):
    attr = cvxcanon_swig.PowerAttributes()
    attr.p = power.p
    return attr

def get_index_attributes(index):
    attr = cvxcanon_swig.IndexAttributes()
    for key in index.key:
        slice_swig = cvxcanon_swig.Slice()
        slice_swig.start = key.start
        slice_swig.stop = key.stop
        slice_swig.step = key.step
        attr.keys.push_back(slice_swig)
    return attr

ATTRIBUTE_MAP = {
    cvxpy.atoms.affine.index.index: get_index_attributes,
    cvxpy.atoms.pnorm: get_pnorm_attributes,
    cvxpy.atoms.power: get_power_attributes,
    cvxpy.expressions.constants.constant.Constant: get_const_attributes,
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

def convert_problem(cvxpy_problem):
    problem = cvxcanon_swig.Problem()
    problem.sense = SENSE_MAP[type(cvxpy_problem.objective)]
    problem.objective = convert_expression(cvxpy_problem.objective.args[0])
    for cvxpy_constr in cvxpy_problem.constraints:
        problem.constraints.push_back(convert_expression(cvxpy_constr))
    return problem
