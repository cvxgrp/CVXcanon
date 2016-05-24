
#ifndef PROBLEM_TRANSFORM_H
#define PROBLEM_TRANSFORM_H

#include "cvxcanon/expression/Expression.hpp"

class ProblemTransform {
 public:
  virtual ~ProblemTransform() {}
  virtual Problem apply(const Problem& problem) = 0;
};

#endif  // PROBLEM_TRANSFORM_H
