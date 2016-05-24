
#ifndef LINEAR_CONE_TRANSFORM_H
#define LINEAR_CONE_TRANSFORM_H

#include "cvxcanon/transform/ProblemTransform.hpp"

class LinearConeTransform : public ProblemTransform {
 public:
  Problem apply(const Problem& problem) override;
};

#endif  // LINEAR_CONE_TRANSFORM_H
