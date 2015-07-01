from setuptools import setup
from setuptools.command.build_ext import build_ext as _build_ext
from setuptools.command.install import install
from distutils.core import Extension
from distutils.command.build import build


# bootstrap the setup.py script to install numpy before compiling the
# C++ extension since we need to link with the numpy.h header
#
# This "hack" has the downside that numpy will be automatically installed
# if the user types setup.py -help etc, and numpy is not currently installed
#    - see https://github.com/scipy/scipy/blob/master/setup.py#L205 for how
#      one way to avoid this
class build_ext(_build_ext):
    def finalize_options(self):
        _build_ext.finalize_options(self)
        # Prevent numpy from thinking it is still in its setup process:
        __builtins__.__NUMPY_SETUP__ = False
        import numpy
        self.include_dirs.append(numpy.get_include())


# Ensure the swig generated CVXcanon.py module is present before python
# copies sources to the build directory
class CustomBuild(build):
    def run(self):
        self.run_command('build_ext')
        build.run(self)


class CustomInstall(install):
    def run(self):
        self.run_command('build_ext')
        self.do_egg_install()

canon = Extension(
    '_CVXcanon',
    sources=['CVXcanon.i', '../CVXcanon.cpp', '../LinOpOperations.cpp'],
    swig_opts=['-c++', '-I../', '-outcurrentdir'],
    # need to include numpy.h header (see above)
    include_dirs=['../'],
)

setup(
    name='CVXcanon',
    version='0.0.1.dev1',
    author='Jack Zhu, John Miller, Paul Quigley',
    author_email='jackzhu@stanford.edu, millerjp@stanford.edu, piq93@stanford.edu',
    ext_modules=[canon],
    py_modules=['canonInterface', 'CVXcanon'],
    description='A low-level library to perform the matrix building step in cvxpy, a convex optimization modeling software.',
    license='GPLv3',
    url='https://github.com/jacklzhu/CVXcanon',
    install_requires=['numpy'],
    cmdclass={'build_ext': build_ext, 'build': CustomBuild, 'install': CustomInstall},
    setup_requires=['numpy']
)
