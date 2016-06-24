# Makefile for cvxcanon, supporting OS X and Linux
#
# NOTE(mwytock): You must run tools/build_third_party.sh first
#
# TODO(mwytock): A bunch of things to add, for example:
# - execute tools/build_third_party.sh (if necessary)
# - automatic dependency generation (e.g. for header file changes)
#
# Basic idea would be that Makefile should be able to build all of the C++ code
# and then language-specific packaging frameworks (e.g. setup.py) should be able
# to use its outputs (libcvxcanon.a) to build the appropriate extensions.

# Directories
src_dir = src/cpp
build_dir = build-cc
tools_dir = tools
gtest_dir = third_party/googletest/googletest
eigen_dir = third_party
ecos_dir = third_party/ecos
deps_dir = build-deps

# Optimization flags, use OPTFLAGS=-g when debugging
OPTFLAGS = -DNDEBUG -O3

CFLAGS += $(OPTFLAGS)
CXXFLAGS += $(OPTFLAGS) -std=c++14
CXXFLAGS += -I$(src_dir) -I$(eigen_dir) -I$(deps_dir)/include
CXXFLAGS += -I$(gtest_dir)/include

# TODO(mwytock): Add these compiler flags
#CXXFLAGS += -Wall -Wextra -Werror
#CXXFLAGS += -Wno-sign-compare -Wno-unused-parameter

# System-specific configuration
SYSTEM = $(shell uname -s)

LDLIBS = -L$(deps_dir)/lib -lecos -lglog -lscsdir

ifeq ($(SYSTEM),Linux)
CFLAGS += -fPIC
CXXFLAGS += -fPIC
LDLIBS += -lpthread -lblas
endif

common_cc = \
	cvxcanon/CVXcanon.cpp \
	cvxcanon/expression/Expression.cpp \
	cvxcanon/expression/ExpressionShape.cpp \
	cvxcanon/expression/ExpressionUtil.cpp \
	cvxcanon/expression/LinearExpression.cpp \
	cvxcanon/expression/TextFormat.cpp \
	cvxcanon/linop/LinOpOperations.cpp \
	cvxcanon/solver/Solver.cpp \
	cvxcanon/solver/SymbolicConeSolver.cpp \
	cvxcanon/solver/cone/EcosConeSolver.cpp \
	cvxcanon/solver/cone/ScsConeSolver.cpp \
	cvxcanon/transform/LinearConeTransform.cpp \
	cvxcanon/util/Init.cpp \
	cvxcanon/util/MatrixUtil.cpp \
	cvxcanon/util/Utils.cpp

common_test_cc = \
	cvxcanon/util/TestMain.cpp

tests = \
	cvxcanon/expression/TextFormatTest

libs = cvxcanon

# Stop make from deleting intermediate files
.SECONDARY:

build_libs = $(libs:%=$(build_dir)/lib%.a)
build_tests = $(tests:%=$(build_dir)/%)

# Targets

all: $(build_libs)

clean:
	rm -rf $(build_dir)

test: $(build_tests)
	@$(tools_dir)/run_tests.sh $(build_tests)


# Compilation rules

build_sub_dirs = $(addprefix $(build_dir)/, $(dir $(common_cc)))
$(build_dir):
	mkdir -p $(build_sub_dirs)


$(build_dir)/%.o: $(src_dir)/%.cpp | $(build_dir)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

$(build_dir)/cvxcanon/solver/cone/EcosConeSolver.o: $(src_dir)/cvxcanon/solver/cone/EcosConeSolver.cpp | $(build_dir)
	$(COMPILE.cc) $(OUTPUT_OPTION) -I$(ecos_dir)/external/SuiteSparse_config $<

gtest_srcs = $(gtest_dir)/src/*.cc $(gtest_dir)/src/*.h

# -Wno-missing-field-intializers avoids error on OS X
$(build_dir)/gtest-all.o: $(gtest_srcs)
	$(COMPILE.cc) -I$(gtest_dir) -Wno-missing-field-initializers -c $(gtest_dir)/src/gtest-all.cc -o $@

common_obj = $(common_cc:%.cpp=$(build_dir)/%.o)

$(build_dir)/libcvxcanon.a: $(common_obj)
	$(AR) rcs $@ $^
ifeq ($(SYSTEM),Darwin)
	ranlib $@
endif

common_test_obj = $(common_test_cc:%.cpp=$(build_dir)/%.o)
common_test_obj += $(common_obj)
common_test_obj += $(build_dir)/gtest-all.o

$(build_dir)/%Test: $(build_dir)/%Test.o $(common_test_obj)
	 $(LINK.cc) $^ $(LDLIBS) -o $@
