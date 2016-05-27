// Given any convex problem, transform it to linear cone form, i.e.
//
// minimize   c'x
// subject to A1x + b1 in K1,
//            A2x + b2 in K2,
//            etc.
//
// Note that the output is still represented in expression tree form as ASTs so
// that (for example) the objective is still an expression, just a linear one.
//
// For solving problems in this form see solver/SymbolicConeSolver.hpp


#ifndef CVXCANON_TRANSFORM_LINEAR_CONE_TRANSFORM_H
#define CVXCANON_TRANSFORM_LINEAR_CONE_TRANSFORM_H

#include "cvxcanon/transform/ProblemTransform.hpp"

class LinearConeTransform : public ProblemTransform {
 public:
  bool accepts(const Problem& problem) override;
  Problem apply(const Problem& problem) override;
};

#endif  // CVXCANON_TRANSFORM_LINEAR_CONE_TRANSFORM_H
