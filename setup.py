from setuptools import setup, Extension, find_packages
import numpy

canon = Extension(
    'CVXcanon._CVXcanon',
    sources=['CVXcanon/CVXcanon.i', 'src/CVXcanon.cpp', 'src/LinOpOperations.cpp'],
    swig_opts=['-c++', '-outdir', 'CVXcanon', '-I./src/', '-I./CVXcanon/'],
    include_dirs=['src/', 'CVXcanon/', 'include/Eigen', numpy.get_include()]
)


setup(
    name='CVXcanon',
    use_scm_version={
        'version_scheme': 'guess-next-dev',
        'local_scheme': 'dirty-tag',
        'write_to': 'CVXcanon/_version.py'
    },

    setup_requires=[
        'setuptools>=18.0',
        'setuptools-scm>1.5.4',
        'numpy'
    ],
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
        #'cvxpy', CIRCULAR DEPENDENCY!
    ]
)
