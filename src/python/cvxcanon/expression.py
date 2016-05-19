"""Handles translating CVXPY expressions to CVXcanon expressions."""

import numpy as np

import cvxpy
import cvxpy.atoms.affine.add_expr
import cvxpy.atoms.affine.binary_operators
import cvxpy.atoms.affine.unary_operators
import cvxpy.atoms.pnorm
import cvxpy.atoms.quad_over_lin
import cvxpy.expressions.constants.constant
import cvxpy.expressions.variables.variable

from cvxcanon import cvxcanon_swig as cvxcanon

SENSE_MAP = {
    cvxpy.problems.objective.Maximize: cvxcanon.Problem.MAXIMIZE,
    cvxpy.problems.objective.Minimize: cvxcanon.Problem.MINIMIZE,
}

TYPE_MAP = {
    cvxpy.atoms.affine.add_expr.AddExpression: cvxcanon.Expression.ADD,
    cvxpy.atoms.affine.binary_operators.MulExpression: cvxcanon.Expression.MUL,
    cvxpy.atoms.affine.unary_operators.NegExpression: cvxcanon.Expression.NEG,
    cvxpy.atoms.pnorm: cvxcanon.Expression.P_NORM,
    cvxpy.atoms.quad_over_lin: cvxcanon.Expression.QUAD_OVER_LIN,
    cvxpy.constraints.eq_constraint.EqConstraint: cvxcanon.Expression.EQ,
    cvxpy.constraints.leq_constraint.LeqConstraint: cvxcanon.Expression.LEQ,
    cvxpy.expressions.constants.constant.Constant: cvxcanon.Expression.CONST,
    cvxpy.expressions.variables.variable.Variable: cvxcanon.Expression.VAR,
}

def get_var_attributes(variable):
    attr = cvxcanon.VarAttributes()
    attr.id = variable.id
    attr.size.dims.push_back(variable.size[0])
    attr.size.dims.push_back(variable.size[1])
    return attr

def get_const_attributes(constant):
    attr = cvxcanon.ConstAttributes()
    attr.set_dense_data(np.asfortranarray(np.atleast_2d(constant.value)))
    return attr

def get_pnorm_attributes(pnorm):
    attr = cvxcanon.PNormAttributes()
    attr.p = pnorm.p
    return attr

ATTRIBUTE_MAP = {
    cvxpy.expressions.constants.constant.Constant: get_const_attributes,
    cvxpy.expressions.variables.variable.Variable: get_var_attributes,
    cvxpy.atoms.pnorm: get_pnorm_attributes,
}

def convert_expression(cvxpy_expr):
    args = cvxcanon.ExpressionVector()
    for arg in cvxpy_expr.args:
        args.push_back(convert_expression(arg))
    t = type(cvxpy_expr)
    expr = cvxcanon.Expression(
        TYPE_MAP[t],
        args,
        ATTRIBUTE_MAP.get(t, lambda x: None)(cvxpy_expr))
    return expr

def convert_problem(cvxpy_problem):
    problem = cvxcanon.Problem()
    problem.sense = SENSE_MAP[type(cvxpy_problem.objective)]
    problem.objective = convert_expression(cvxpy_problem.objective.args[0])
    for cvxpy_constr in cvxpy_problem.constraints:
        problem.constraints.push_back(convert_expression(cvxpy_constr))
    return problem
