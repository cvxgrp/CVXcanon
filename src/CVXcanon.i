%module CVXcanon

%{
#define SWIG_FILE_WITH_INIT
#include "CVXcanon.hpp"
%}

%include "std_vector.i"
%include "std_map.i"
%include "ProblemData.hpp"
%include "LinOp.hpp"

namespace std {
   %template(IntVector) vector<int>;
   %template(DoubleVector) vector<double>;
   %template(DoubleVector2D) vector< vector<double> >;
   %template(IntIntMap) map<int, int>;
   %template(LinOpVector) vector< LinOp * >;
}

ProblemData build_matrix(std::vector< LinOp* > constraints, std::map<int, int> id_to_col);