from setuptools import setup, Extension, find_packages
from setuptools.command.install import install
from distutils.command.build import build
import numpy
import os

canon = Extension(
    '_CVXcanon',
    sources=['src/CVXcanon.cpp', 'src/LinOpOperations.cpp', 'src/python/CVXcanon_wrap.cpp'],
    include_dirs=['src/', 'src/python/', 'include/Eigen', numpy.get_include()]
)

base_dir = os.path.dirname(__file__)
about = {}
with open(os.path.join(base_dir, "src", "python", "_version__.py")) as f:
    exec(f.read(), about)

setup(
    name='CVXcanon',
    version=about["__version__"],
    setup_requires=['numpy'],
    author='Jack Zhu, John Miller, Paul Quigley',
    author_email='jackzhu@stanford.edu, millerjp@stanford.edu, piq93@stanford.edu',
    ext_modules=[canon],
    package_dir={'': 'src/python'},
    py_modules=['canonInterface', 'CVXcanon', '_version__'],
    description='A low-level library to perform the matrix building step in cvxpy, a convex optimization modeling software.',
    license='GPLv3',
    url='https://github.com/jacklzhu/CVXcanon',
    install_requires=[
        'numpy',
        'scipy',
    ]
)
