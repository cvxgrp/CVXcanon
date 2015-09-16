from setuptools import setup
from setuptools.command.install import install
from distutils.core import Extension
from distutils.command.build import build
import numpy


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

canon = Extension(
    '_CVXcanon',
    sources=['src/python/CVXcanon.i', 'src/CVXcanon.cpp', 'src/LinOpOperations.cpp'],
    swig_opts=['-c++', '-outdir', 'src/python', '-I./src/', '-I./src/python/'],
    include_dirs=['src/', 'src/python/', 'include/Eigen', numpy.get_include()]
)


setup(
    name='CVXcanon',
    version='0.0.18',
    author='Jack Zhu, John Miller, Paul Quigley',
    author_email='jackzhu@stanford.edu, millerjp@stanford.edu, piq93@stanford.edu',
    ext_modules=[canon],
    package_dir={'': 'src/python'},
    py_modules=['canonInterface', 'CVXcanon'],
    description='A low-level library to perform the matrix building step in cvxpy, a convex optimization modeling software.',
    license='GPLv3',
    url='https://github.com/jacklzhu/CVXcanon',
    install_requires=['numpy'],
    cmdclass={'build': CustomBuild, 'install': CustomInstall},
    setup_requires=['numpy']
)
