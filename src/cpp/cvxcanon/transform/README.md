# Problem transforms

This module defines the generic interface for problems transforms
```
class ProblemTransform {
 public:
  virtual Problem apply(const Problem& problem) = 0;
};
```
Typically, these transform an optimization problem to a mathematically
equivalent form which has some particular property. For example, the
`LinearConeTranform` transforms any convex problem to cone problem with linear
objective.