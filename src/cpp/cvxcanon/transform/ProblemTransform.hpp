// Interface for problem transforms.

#ifndef CVXCANON_TRANSFORM_PROBLEM_TRANSFORM_H
#define CVXCANON_TRANSFORM_PROBLEM_TRANSFORM_H

#include "cvxcanon/expression/Expression.hpp"

class ProblemTransform {
 public:
  virtual ~ProblemTransform() {}
  virtual bool accepts(const Problem& problem) { return true; }
  virtual Problem apply(const Problem& problem) = 0;
};

#endif  // CVXCANON_TRANSFORM_PROBLEM_TRANSFORM_H
