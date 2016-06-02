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

# For debugging a single test
from cvxpy import *

import cvxopt
import math
import numpy.linalg as LA

v = cvxopt.matrix([-1,2,-2], tc='d')
v_np = np.matrix([-1.,2,-2]).T

log_sum_exp_axis_0 = lambda x: log_sum_exp(x, axis=0)
log_sum_exp_axis_1 = lambda x: log_sum_exp(x, axis=1)


atoms = [
    ([
        (abs, (2, 2), [ [[-5,2],[-3,1]] ],
            Constant([[5,2],[3,1]])),
        (diag, (2, 1), [ [[-5,2],[-3,1]] ], Constant([-5, 1])),
        (diag, (2, 2), [ [-5, 1] ], Constant([[-5, 0], [0, 1]])),
        (exp, (2, 2), [ [[1, 0],[2, -1]] ],
            Constant([[math.e, 1],[math.e**2, 1.0/math.e]])),
        (huber, (2, 2), [ [[0.5, -1.5],[4, 0]] ],
            Constant([[0.25, 2],[7, 0]])),
        (lambda x: huber(x, 2.5), (2, 2), [ [[0.5, -1.5],[4, 0]] ],
             Constant([[0.25, 2.25],[13.75, 0]])),
        (inv_pos, (2, 2), [ [[1,2],[3,4]] ],
            Constant([[1,1.0/2],[1.0/3,1.0/4]])),
        (lambda x: (x + Constant(0))**-1, (2, 2), [ [[1,2],[3,4]] ],
            Constant([[1,1.0/2],[1.0/3,1.0/4]])),
        (kl_div, (1, 1), [math.e, 1], Constant([1])),
        (kl_div, (1, 1), [math.e, math.e], Constant([0])),
        (kl_div, (2, 1), [ [math.e,1], 1], Constant([1,0])),
        (lambda x: kron(np.matrix("1 2; 3 4"), x), (4, 4), [np.matrix("5 6; 7 8")],
            Constant(np.kron(np.matrix("1 2; 3 4").A, np.matrix("5 6; 7 8").A))),
        (lambda_max, (1, 1), [ [[2,0],[0,1]] ], Constant([2])),
        (lambda_max, (1, 1), [ [[2,0,0],[0,3,0],[0,0,1]] ], Constant([3])),
        (lambda_max, (1, 1), [ [[5,7],[7,-3]] ], Constant([9.06225775])),
        (lambda x: lambda_sum_largest(x, 2), (1, 1), [ [[1, 2, 3], [2,4,5], [3,5,6]] ], Constant([11.51572947])),
        (log_sum_exp, (1, 1), [ [[5, 7], [0, -3]] ], Constant([7.1277708268])),
        (log_sum_exp_axis_0, (1,2), [ [[5, 7, 1], [0, -3, 6]] ], Constant([7.12910890, 6.00259878]).T),
        (log_sum_exp_axis_1, (3,1), [ [[5, 7, 1], [0, -3, 6]] ], Constant([5.00671535, 7.0000454, 6.0067153])),
        (logistic, (2, 2),
         [
             [[math.log(5), math.log(7)],
              [0,           math.log(0.3)]] ],
         Constant(
             [[math.log(6), math.log(8)],
              [math.log(2), math.log(1.3)]])),
        (matrix_frac, (1, 1), [ [1, 2, 3],
                            [[1, 0, 0],
                             [0, 1, 0],
                             [0, 0, 1]] ], Constant([14])),
        (matrix_frac, (1, 1), [ [1, 2, 3],
                            [[67, 78, 90],
                             [78, 94, 108],
                             [90, 108, 127]] ], Constant([0.46557377049180271])),
        (matrix_frac, (1, 1), [ [[1, 2, 3],
                                 [4, 5, 6]],
                                [[67, 78, 90],
                                 [78, 94, 108],
                                 [90, 108, 127]] ], Constant([0.768852459016])),
        (max_elemwise, (2, 1), [ [-5,2],[-3,1],0,[-1,2] ], Constant([0,2])),
        (max_elemwise, (2, 2), [ [[-5,2],[-3,1]],0,[[5,4],[-1,2]] ],
            Constant([[5,4],[0,2]])),
        (max_entries, (1, 1), [ [[-5,2],[-3,1]] ], Constant([2])),
        (max_entries, (1, 1), [ [-5,-10] ], Constant([-5])),
        (lambda x: max_entries(x, axis=0), (1, 2), [ [[-5,2],[-3,1]] ], Constant([2, 1]).T),
        (lambda x: max_entries(x, axis=1), (2, 1), [ [[-5,2],[-3,1]] ], Constant([-3, 2])),
        (lambda x: norm(x, 2), (1, 1), [v], Constant([3])),
        (lambda x: norm(x, "fro"), (1, 1), [ [[-1, 2],[3, -4]] ],
            Constant([5.47722557])),
        (lambda x: norm(x,1), (1, 1), [v], Constant([5])),
        (lambda x: norm(x,1), (1, 1), [ [[-1, 2], [3, -4]] ],
            Constant([10])),
        (lambda x: norm(x,"inf"), (1, 1), [v], Constant([2])),
        (lambda x: norm(x,"inf"), (1, 1), [ [[-1, 2], [3, -4]] ],
            Constant([4])),
        (lambda x: norm(x,"nuc"), (1, 1), [ [[2,0],[0,1]] ], Constant([3])),
        (lambda x: norm(x,"nuc"), (1, 1), [ [[3,4,5],[6,7,8],[9,10,11]] ],
            Constant([23.173260452512931])),
        (lambda x: norm(x,"nuc"), (1, 1), [ [[3,4,5],[6,7,8]] ],
            Constant([14.618376738088918])),
        (lambda x: sum_largest(abs(x), 3), (1, 1), [ [1,2,3,-4,-5] ], Constant([5+4+3])),
        (lambda x: mixed_norm(x,1,1), (1, 1), [ [[1,2],[3,4],[5,6]] ],
            Constant([21])),
        (lambda x: mixed_norm(x,1,1), (1, 1), [ [[1,2,3],[4,5,6]] ],
            Constant([21])),
        (lambda x: mixed_norm(x,2,1), (1, 1), [ [[3,3],[4,4]] ],
            Constant([10])),
        (lambda x: mixed_norm(x,1,'inf'), (1, 1), [ [[1,4],[5,6]] ],
            Constant([10])),

        (pnorm, (1, 1), [[1, 2, 3]], Constant([3.7416573867739413])),
        (lambda x: pnorm(x, 1), (1, 1), [[1.1, 2, -3]], Constant([6.1])),
        (lambda x: pnorm(x, 2), (1, 1), [[1.1, 2, -3]], Constant([3.7696153649941531])),
        (lambda x: pnorm(x, 2, axis=0), (1, 2), [ [[1,2],[3,4]] ], Constant([math.sqrt(5), 5.]).T),
        (lambda x: pnorm(x, 2, axis=1), (2, 1), [ [[1,2],[4,5]] ], Constant([math.sqrt(17), math.sqrt(29)])),
        (lambda x: pnorm(x, 'inf'), (1, 1), [[1.1, 2, -3]], Constant([3])),
        (lambda x: pnorm(x, 3), (1, 1), [[1.1, 2, -3]], Constant([3.3120161866074733])),
        (lambda x: pnorm(x, 5.6), (1, 1), [[1.1, 2, -3]], Constant([3.0548953718931089])),
        (lambda x: pnorm(x, 1.2), (1, 1), [[[1, 2, 3], [4, 5, 6]]], Constant([15.971021676279573])),

        (pos, (1, 1), [8], Constant([8])),
        (pos, (2, 1), [ [-3,2] ], Constant([0,2])),
        (neg, (2, 1), [ [-3,3] ], Constant([3,0])),

        (lambda x: power(x, 0), (1, 1), [7.45], Constant([1])),
        (lambda x: power(x, 1), (1, 1), [7.45], Constant([7.45])),
        (lambda x: power(x, 2), (1, 1), [7.45], Constant([55.502500000000005])),
        (lambda x: power(x, -1), (1, 1), [7.45], Constant([0.1342281879194631])),
        (lambda x: power(x, -.7), (1, 1), [7.45], Constant([0.24518314363015764])),
        (lambda x: power(x, -1.34), (1, 1), [7.45], Constant([0.06781263100321579])),
        (lambda x: power(x, 1.34), (1, 1), [7.45], Constant([14.746515290825071])),

        (quad_over_lin, (1, 1), [ [[-1,2,-2], [-1,2,-2]], 2], Constant([2*4.5])),
        (quad_over_lin, (1, 1), [v, 2], Constant([4.5])),
        (lambda x: norm(x, 2), (1, 1), [ [[2,0],[0,1]] ], Constant([2])),
        (lambda x: norm(x, 2), (1, 1), [ [[3,4,5],[6,7,8],[9,10,11]] ], Constant([22.368559552680377])),
        (lambda x: scalene(x, 2, 3), (2, 2), [ [[-5,2],[-3,1]] ], Constant([[15,4],[9,2]])),
        (square, (2, 2), [ [[-5,2],[-3,1]] ], Constant([[25,4],[9,1]])),
        (sum_entries, (1,1), [ [[-5,2],[-3,1]] ], Constant(-5)),
        (lambda x: sum_entries(x, axis=0), (1,2), [ [[-5,2],[-3,1]] ], Constant([[-3], [-2]])),
        (lambda x: sum_entries(x, axis=1), (2,1), [ [[-5,2],[-3,1]] ], Constant([-8, 3])),
        (lambda x: (x + Constant(0))**2, (2, 2), [ [[-5,2],[-3,1]] ], Constant([[25,4],[9,1]])),
        (lambda x: sum_largest(x, 3), (1, 1), [ [1,2,3,4,5] ], Constant([5+4+3])),
        (lambda x: sum_largest(x, 3), (1, 1), [ [[3,4,5],[6,7,8],[9,10,11]] ], Constant([9+10+11])),
        (sum_squares, (1, 1), [ [[-1, 2],[3, -4]] ], Constant([30])),
        (trace, (1, 1), [ [[3,4,5],[6,7,8],[9,10,11]] ], Constant([3 + 7 + 11])),
        (trace, (1, 1), [ [[-5,2],[-3,1]]], Constant([-5 + 1])),
        (tv, (1, 1), [ [1,-1,2] ], Constant([5])),
        (tv, (1, 1), [ [[1],[-1],[2]] ], Constant([5])),
        (tv, (1, 1), [ [[-5,2],[-3,1]] ], Constant([math.sqrt(53)])),
        (tv, (1, 1), [ [[-5,2],[-3,1]], [[6,5],[-4,3]], [[8,0],[15,9]] ],
            Constant([LA.norm([7, -1, -8, 2, -10, 7])])),
        (tv, (1, 1), [ [[3,4,5],[6,7,8],[9,10,11]] ], Constant([4*math.sqrt(10)])),
        (upper_tri, (3, 1), [ [[3,4,5],[6,7,8],[9,10,11]] ], Constant([6, 9, 10])),
        # Advanced indexing.
        (lambda x: x[[1,2], [0,2]], (2, 1), [ [[3,4,5],[6,7,8],[9,10,11]] ], Constant([4, 11])),
        (lambda x: x[[1,2]], (2, 1), [ [[3,4,5],[6,7,8]] ], Constant([[4,5], [7,8]])),
        (lambda x: x[np.array([[3,4,5],[6,7,8]]).T % 2 == 0], (2, 1), [ [[3,4,5],[6,7,8]] ], Constant([6,4,8])),
    ], Minimize),
]

def check_solver_cvxcanon(prob, solver_name):
    # Usual cvxpy checks
    if not check_solver(prob, solver_name):
        return False

    # CVXcanon solver checks (e.g. do we have all transforms implemented)
    if not cvxpy_solver.validate(prob, solver=solver_name):
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
        print problem_data["dims"]
        print "A:"
        print problem_data["A"].todense()
        print "b:", problem_data["b"]
        print "c:", problem_data["c"]

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
