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

    def test_sum(self):
        # create two scalar variables
        x1 = Variable(3, 3)
        x2 = Variable(3, 3)

        A = np.random.randn(3, 3)

        settings.USE_CVXCANON = False
        p = Problem(Minimize(0), [x1 + x2 == A])
        cvxpy_data = p.get_problem_data(ECOS)

        settings.USE_CVXCANON = True
        p = Problem(Minimize(0), [x1 + x2 == A])
        cvx_canon_data = p.get_problem_data(ECOS)

        self.assertDataEqual(cvxpy_data, cvx_canon_data)


if __name__ == '__main__':
    unittest.main()
