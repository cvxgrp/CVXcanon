// Data structures representing the AST for an optimization problem

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>

class Size {
 public:
  Size() {}
  Size(std::vector<int> d) : dims(d) {}
  std::vector<int> dims;
};

// Expression trees
class Expression {
 public:
  enum Type {
    // Linear functions
    ADD,
    INDEX,
    MUL,
    NEG,

    // Non-linear functions
    ABS,
    P_NORM,
    QUAD_OVER_LIN,

    // Constraints
    EQ,
    LEQ,

    // Leaf nodes
    CONST,
    VAR,
  };
  Type type;

  std::vector<Expression> args;
  Size size;

  // P_NORM
  int p;
};

class Problem {
 public:
  enum Sense {
    MAXIMIZE,
    MINIMIZE,
  };
  Sense sense;
  Expression objective;
  std::vector<Expression> constraints;
};

#endif  // EXPRESSION_H
