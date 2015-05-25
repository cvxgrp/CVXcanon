import unittest
import sys 
sys.path.append("../../src/python/")
import canonInterface

from cvxpy import *
import numpy as np
from cvxpy.tests.base_test import *
import scipy


class TestLinOps(BaseTest):
    def assertDataEqual(self, original_data, canon_data):
        for key in ['A', 'b', 'c', 'G', 'h']:
            M1, M2 = original_data[key], canon_data[key]
            if isinstance(M1, scipy.sparse.csc.csc_matrix):
                M1 = M1.todense()
                M2 = M2.todense()

            if key == 'b' or key == 'h' or key == 'c':
                M1 = M1.reshape(-1, 1)
                M2 = M2.reshape(-1, 1)

            self.assertTrue(np.linalg.norm(M1 - M2, 'fro') < 1e-10)

    def assertConstraintsMatch(self, constraints):
        settings.USE_CVXCANON = False
        p = Problem(Minimize(0), constraints)
        cvxpy_data = p.get_problem_data(ECOS)

        settings.USE_CVXCANON = True
        p = Problem(Minimize(0), constraints)
        cvx_canon_data = p.get_problem_data(ECOS)

        self.assertDataEqual(cvxpy_data, cvx_canon_data)

    def test_sum(self):
        rows, cols = 3, 3
        x1 = Variable(rows, cols)
        x2 = Variable(rows, cols)
        A = np.random.randn(rows, cols)
        self.assertConstraintsMatch([x1 + x2 == A])

    def test_promote(self):
        rows, cols = 10, 10
        b = Variable()
        v1 = Variable(rows, 1)
        self.assertConstraintsMatch([v1 == b])

    def test_mul(self):
        rows, cols = 17, 26

        A = np.random.randn(rows, cols)
        b = Variable(rows, 1)
        x = Variable(cols, 1)
        y = Variable(rows, 1)
        self.assertConstraintsMatch([A*x == b, A*x == y])

    def test_rmul(self):
        rows, cols = 15, 17

        A = np.random.randn(rows, cols)
        b = Variable(1, cols)
        x = Variable(1, rows)
        y = Variable(1, cols)
        self.assertConstraintsMatch([x * A == b, x * A == y])

    def test_mul_elemwise(self):
        rows, cols = 15, 17
        A = np.random.randn(rows, cols)
        x1 = Variable(rows, cols)
        M = Variable(rows, cols)
        self.assertConstraintsMatch([mul_elemwise(A, x1) == M])

    def test_div(self):
        x = Variable()
        self.assertConstraintsMatch([x / 4 == 3])

    def test_neg(self):
        rows, cols = 3, 3
        x1 = Variable(rows, cols)
        x2 = Variable(rows, cols)
        A = np.random.randn(rows, cols)
        self.assertConstraintsMatch([x1 - x2 == A])

    def test_index(self):
        print "WARNING: INDEXING TEST NOT RUN!"
        # rows, cols = 15, 17
        # X = Variable(rows, cols)

        # TODO: Our indexing code breaks on a lot of these test-> seg faults :(
        # for trial in xrange(10):
        #     xi = np.random.choice(rows)
        #     xj = np.random.choice(rows)
        #     xk = np.random.choice(rows)
        #     ry = np.random.choice(cols)
        #     if xk == 0:
        #         xk = 1
        #     if np.random.rand() < 0.5:
        #         xk *= -1

        #     yi = np.random.choice(cols)
        #     yj = np.random.choice(cols)
        #     yk = np.random.choice(cols)
        #     rx = np.random.choice(rows)
        #     if yk == 0:
        #         yk = 1
        #     if np.random.rand() < 0.5:
        #         yk *= -1

        #     constr = [X[xi:xj:xk, ry] == 0, X[rx, yi:yj:yk]]
        #     self.assertConstraintsMatch(constr)

    def test_transpose(self):
        rows, cols = 15, 17
        X = Variable(rows, cols)
        A = np.random.randn(rows, cols)
        self.assertConstraintsMatch([X.T == A.T])

    def test_sum_entries(self):
        rows, cols = 15, 17
        X = Variable(rows, cols)
        self.assertConstraintsMatch([sum_entries(X) == 4.5])

    def test_trace(self):
        n = 15
        X = Variable(n, n)
        self.assertConstraintsMatch([trace(X) == 3])

    def test_reshape(self):
        rows, cols = 15, 17
        X = Variable(rows, cols)

        while True:
            n = np.random.choice(rows + cols)
            if n == 0:
                n = 1
            if rows * cols % n == 0:
                break
        m = (rows * cols) / n

        A = np.random.randn(m, n)

        constr = [reshape(X, m, n) == A]
        self.assertConstraintsMatch(constr)

    def test_diag_vec(self):
        n = 15
        x = Variable(n, 1)
        constr = [diag(x) == np.eye(n)]
        self.assertConstraintsMatch(constr)

    def test_diag_mat(self):
        n = 15
        X = Variable(n, n)
        v = np.random.randn(n)
        constr = [diag(X) == v]
        self.assertConstraintsMatch(constr)

    def test_upper_tri(self):
        n = 15
        X = Variable(n, n)
        constr = [upper_tri(X) == 0]
        self.assertConstraintsMatch(constr)

    def test_conv(self):
        n, m = 15, 17
        c = np.random.randn(m, 1)
        x = Variable(n, 1)
        y = np.random.randn(n + m - 1, 1)

        constr = [conv(c, x) == y]
        self.assertConstraintsMatch(constr)

    def test_hstack(self):
        rows, cols = 15, 17
        X1 = Variable(rows, cols)
        X2 = Variable(rows, cols)
        X3 = Variable(rows, cols)
        X4 = Variable(rows, cols)
        X5 = Variable(rows, cols)

        A = np.random.randn(rows, cols * 5)
        constr = [hstack(X1, X2, X3, X4, X5) == A]
        self.assertConstraintsMatch(constr)

    def test_vstack(self):
        rows, cols = 15, 17
        X1 = Variable(rows, cols)
        X2 = Variable(rows, cols)
        X3 = Variable(rows, cols)
        X4 = Variable(rows, cols)
        X5 = Variable(rows, cols)

        A = np.random.randn(5 * rows, cols)
        constr = [vstack(X1, X2, X3, X4, X5) == A]
        self.assertConstraintsMatch(constr)
        

if __name__ == '__main__':
    unittest.main()