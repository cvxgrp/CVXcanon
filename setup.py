from setuptools import setup, Extension, find_packages
from setuptools.command.install import install
from distutils.command.build import build
import numpy

canon = Extension(
    'CVXcanon._CVXcanon',
    sources=['src/CVXcanon.cpp', 'src/LinOpOperations.cpp', 'CVXcanon/CVXcanon_wrap.cpp'],
    include_dirs=['src/', 'CVXcanon/', 'include/Eigen', numpy.get_include()]
)

setup(
    name='CVXcanon',
    version='0.0.21',
    setup_requires=['numpy'],
    author='Jack Zhu, John Miller, Paul Quigley',
    author_email='jackzhu@stanford.edu, millerjp@stanford.edu, piq93@stanford.edu',
    ext_modules=[canon],
    packages=find_packages(),
    py_modules=['canonInterface'],
    description='A low-level library to perform the matrix building step in cvxpy, a convex optimization modeling software.',
    license='GPLv3',
    url='https://github.com/jacklzhu/CVXcanon',
    install_requires=[
        'numpy',
        'scipy',
    ]
)
