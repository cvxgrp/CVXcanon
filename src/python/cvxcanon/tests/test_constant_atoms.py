"""Test atoms by calling with a constant value.

From cvxpy, modified for solving with cvxcanon.
"""

from cvxpy.error import SolverError
from cvxpy.expressions.constants import Constant, Parameter
from cvxpy.expressions.variables import Variable
from cvxpy.problems.problem import Problem
from cvxpy.settings import OPTIMAL, OPTIMAL_INACCURATE, SCS
from cvxpy.tests.test_constant_atoms import atoms, SOLVER_TO_TOL, check_solver
import cvxpy.interface as intf
import numpy as np

from cvxcanon import cvxpy_solver

SOLVERS_TO_TRY = [SCS]

def check_solver_cvxcanon(prob, solver_name):
    # Usual cvxpy checks
    if not check_solver(prob, solver_name):
        return False

    # CVXcanon solver checks (e.g. do we have all transforms implemented)
    if not cvxpy_solver.validate(prob, solver=solver_name):
        print("CVXcanon missing transforms")
        return False

    return True

def run_atom(atom, problem, obj_val, solver):
    assert problem.is_dcp()
    print(problem.objective)
    print(problem.constraints)
    if check_solver_cvxcanon(problem, solver):
        print("solver", solver)
        tolerance = SOLVER_TO_TOL[solver]

        problem_data = problem.get_problem_data(SCS)
        print(problem_data["dims"])
        print("A:")
        print(problem_data["A"].todense())
        print("b:", problem_data["b"])
        print("c:", problem_data["c"])

        try:
            status, result = cvxpy_solver.solve(problem, solver=solver)
        except SolverError as e:
            raise e

        assert status in [OPTIMAL, OPTIMAL_INACCURATE], status
        print(result)
        print(obj_val)
        assert( -tolerance <= (result - obj_val)/(1+np.abs(obj_val)) <= tolerance )

def test_atom():
    for atom_list, objective_type in atoms:
        for atom, size, args, obj_val in atom_list:
            for row in range(size[0]):
                for col in range(size[1]):
                    for solver in SOLVERS_TO_TRY:
                        # Atoms with Constant arguments.
                        const_args = [Constant(arg) for arg in args]
                        yield (run_atom,
                               atom,
                               Problem(objective_type(atom(*const_args)[row,col])),
                               obj_val[row,col].value,
                               solver)
                        # Atoms with Variable arguments.
                        variables = []
                        constraints = []
                        for idx, expr in enumerate(args):
                            variables.append( Variable(*intf.size(expr) ))
                            constraints.append( variables[-1] == expr)
                        objective = objective_type(atom(*variables)[row,col])
                        yield (run_atom,
                               atom,
                               Problem(objective, constraints),
                               obj_val[row,col].value,
                               solver)
                        # Atoms with Parameter arguments.
                        parameters = []
                        for expr in args:
                            parameters.append( Parameter(*intf.size(expr)) )
                            parameters[-1].value = intf.DEFAULT_INTF.const_to_matrix(expr)
                        objective = objective_type(atom(*parameters)[row,col])
                        yield (run_atom,
                               atom,
                               Problem(objective),
                               obj_val[row,col].value,
                               solver)
