"""CVXcanon module."""

from setuptools import setup, Extension, find_packages
from setuptools.command.install import install
from distutils.command.build import build
import numpy
import os

PYTHON_DIR = "src/python"

CVXCANON_SOURCES = [
    "src/cpp/cvxcanon/CVXcanon.cpp",
    "src/cpp/cvxcanon/expression/Expression.cpp",
    "src/cpp/cvxcanon/expression/ExpressionShape.cpp",
    "src/cpp/cvxcanon/expression/ExpressionUtil.cpp",
    "src/cpp/cvxcanon/expression/LinearExpression.cpp",
    "src/cpp/cvxcanon/expression/TextFormat.cpp",
    "src/cpp/cvxcanon/linop/LinOpOperations.cpp",
    "src/cpp/cvxcanon/solver/Solver.cpp",
    "src/cpp/cvxcanon/solver/SplittingConeSolver.cpp",
    "src/cpp/cvxcanon/solver/SymbolicConeSolver.cpp",
    "src/cpp/cvxcanon/transform/LinearConeTransform.cpp",
    "src/cpp/cvxcanon/util/MatrixUtil.cpp",
    "src/cpp/cvxcanon/util/Utils.cpp",
    "src/python/cvxcanon/cvxcanon_swig_wrap.cpp",
]

SOLVER_LIBRARIES = [
    "third_party/scs/out/libscsdir.a",
]

# Read version from file
base_dir = os.path.dirname(__file__)
about = {}
with open(os.path.join(base_dir,  PYTHON_DIR, "cvxcanon", "_version__.py")) as f:
    exec(f.read(), about)

cvxcanon_swig = Extension(
    name="cvxcanon._cvxcanon_swig",
    language="c++",
    sources=CVXCANON_SOURCES,
    extra_compile_args=["-std=c++14"],
    include_dirs=[
        "src/cpp",
        "src/python",
        numpy.get_include(),
        "third_party",
    ],
    extra_link_args=SOLVER_LIBRARIES,
)

setup(
    name="CVXcanon",
    version=about["__version__"],
    setup_requires=["numpy"],
    author="Jack Zhu, John Miller, Paul Quigley",
    author_email="jackzhu@stanford.edu, millerjp@stanford.edu, piq93@stanford.edu",
    ext_modules=[cvxcanon_swig],
    package_dir={"": PYTHON_DIR},
    packages=find_packages(PYTHON_DIR),
    py_modules=["canonInterface"],
    description="A low-level library to perform the matrix building step in cvxpy, a convex optimization modeling software.",
    license="GPLv3",
    url="https://github.com/cvxgrp/CVXcanon",
    install_requires=[
        "numpy",
        "scipy",
    ]
)
