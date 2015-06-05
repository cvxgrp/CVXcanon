import numpy
from setuptools import setup
from distutils.core import Extension

canon = Extension('_CVXcanon',
	sources=['CVXcanon.i','../CVXcanon.cpp','../LinOpOperations.cpp'],
	swig_opts=['-c++','-I../','-outcurrentdir'],
	include_dirs=[numpy.get_include(),'../'],
	extra_compile_args=['-std=c++11'],
	extra_link_args=[]);

setup(
	name='CVXcanon',
	version='0.1.1',
    author='Jack Zhu, John Miller, Paul Quigley',
    author_email='jackzhu@stanford.edu, millerjp@stanford.edu, piq93@stanford.edu',
	ext_modules=[canon],
    py_modules=['canonInterface','CVXcanon']
)