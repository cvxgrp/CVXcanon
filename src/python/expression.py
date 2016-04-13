"""Handles translating CVXPY expressions to CVXcanon expressions."""

import CVXcanon
import cvxpy
import cvxpy.atoms.affine.add_expr
import cvxpy.atoms.affine.binary_operators
import cvxpy.atoms.affine.unary_operators
import cvxpy.atoms.pnorm
import cvxpy.atoms.quad_over_lin
import cvxpy.expressions.constants.constant
import cvxpy.expressions.variables.variable


SENSE_MAP = {
    cvxpy.problems.objective.Maximize: CVXcanon.Problem.MAXIMIZE,
    cvxpy.problems.objective.Minimize: CVXcanon.Problem.MINIMIZE,
}

EXPRESSION_TYPE_MAP = {
    cvxpy.atoms.affine.add_expr.AddExpression: CVXcanon.Expression.ADD,
    cvxpy.atoms.affine.binary_operators.MulExpression: CVXcanon.Expression.MUL,
    cvxpy.atoms.affine.unary_operators.NegExpression: CVXcanon.Expression.NEG,
    cvxpy.atoms.pnorm: CVXcanon.Expression.P_NORM,
    cvxpy.atoms.quad_over_lin: CVXcanon.Expression.QUAD_OVER_LIN,
    cvxpy.constraints.eq_constraint.EqConstraint: CVXcanon.Expression.EQ,
    cvxpy.constraints.leq_constraint.LeqConstraint: CVXcanon.Expression.LEQ,
    cvxpy.expressions.constants.constant.Constant: CVXcanon.Expression.CONST,
    cvxpy.expressions.variables.variable.Variable: CVXcanon.Expression.VAR,
}

def convert_expression(cvxpy_expr):
    expr = CVXcanon.Expression()
    expr.type = EXPRESSION_TYPE_MAP[type(cvxpy_expr)]
    for arg in cvxpy_expr.args:
        expr.args.push_back(convert_expression(arg))
    return expr

def convert_problem(cvxpy_problem):
    problem = CVXcanon.Problem()
    problem.sense = SENSE_MAP[type(cvxpy_problem.objective)]
    problem.objective = convert_expression(cvxpy_problem.objective.args[0])
    for cvxpy_constr in cvxpy_problem.constraints:
        problem.constraints.push_back(convert_expression(cvxpy_constr))
    return problem
