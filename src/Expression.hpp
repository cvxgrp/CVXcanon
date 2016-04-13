// Data structures representing the AST for an optimization problem

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>

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

  std::vector<Expression*> args;
  // TODO(mwytock): Other attributes
};

class Problem {
 public:
  enum Sense {
    MAXIMIZE,
    MINIMIZE,
  };
  Sense sense;
  Expression* objective;
  std::vector<Expression*> constraints;
};

#endif  // EXPRESSION_H
