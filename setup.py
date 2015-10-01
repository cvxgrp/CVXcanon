from setuptools import setup, Extension, find_packages
from setuptools.command.install import install
from distutils.command.build import build
import numpy

canon = Extension(
    'CVXcanon._CVXcanon',
    sources=['CVXcanon/CVXcanon.i', 'src/CVXcanon.cpp', 'src/LinOpOperations.cpp'],
    swig_opts=['-c++', '-outdir', 'CVXcanon', '-I./src/', '-I./CVXcanon/'],
    include_dirs=['src/', 'CVXcanon/', 'include/Eigen', numpy.get_include()]
)


# Ensure the swig generated CVXcanon.py module is present before python
# copies sources to the build directory
class CustomBuild(build):
    def run(self):
        self.run_command('build_ext')
        build.run(self)


# Ensure the swig generated CVXcanon.py module is present before python
# copies sources to the build directory
class CustomInstall(install):
    def run(self):
        self.run_command('build_ext')
        self.do_egg_install()


setup(
    name='CVXcanon',
    version='0.0.19',
    # use_scm_version={
    #     'version_scheme': 'guess-next-dev',
    #     'local_scheme': 'dirty-tag',
    #     'write_to': 'CVXcanon/_version.py'
    # },

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
    cmdclass={'build': CustomBuild, 'install': CustomInstall},
    install_requires=[
        'numpy',
        'scipy',
        #'cvxpy', CIRCULAR DEPENDENCY!
    ]
)
