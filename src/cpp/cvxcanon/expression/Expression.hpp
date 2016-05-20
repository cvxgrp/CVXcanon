// Data structures representing the AST for an optimization problem

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <memory>

#include "cvxcanon/util/Utils.hpp"

struct ExpressionAttributes {
  virtual ~ExpressionAttributes() {}
};

// Expression trees
class Expression {
 public:
  enum Type {
    // Linear functions
    ADD,
    DIAG_MAT,
    DIAG_VEC,
    HSTACK,
    INDEX,
    KRON,
    MUL,
    NEG,
    RESHAPE,
    SUM_ENTRIES,
    TRACE,
    UPPER_TRI,
    VSTACK,

    // Elementwise functions
    ABS,
    ENTR,
    EXP,
    HUBER,
    KL_DIV,
    LOG,
    LOG1P,
    LOGISTIC,
    MAX_ELEMWISE,
    POWER,

    // General nonlinear functions
    GEO_MEAN,
    LAMBDA_MAX,
    LOG_DET,
    LOG_SUM_EXP,
    MATRIX_FRAC,
    MAX_ENTRIES,
    NORM_NUC,
    P_NORM,
    QUAD_OVER_LIN,
    SIGMA_MAX,
    SUM_LARGEST,

    // Constraints
    EQ,
    LEQ,
    SOC,

    // Leaf nodes
    CONST,
    PARAM,
    VAR,
  };

  Type type() const { return data_->type; }
  const std::vector<Expression>& args() const { return data_->args; }
  const Expression& arg(int i) const { return data_->args[i]; }

  template<typename T>
  const T& attr() const {
    //printf("attr, pointer: %p->%p\n", data_.get(), data_->attributes.get());
    assert(data_->attributes.get() != nullptr);
    return static_cast<const T&>(*data_->attributes);
  }
  std::shared_ptr<const ExpressionAttributes> attr_ptr() const {
    return data_->attributes;
  }

  Expression() {}
  Expression(
      Type type,
      std::vector<Expression> args,
      std::shared_ptr<const ExpressionAttributes> attributes)
      : data_(new Data{type, args, attributes}) {}

  // Convenient constructor for Python SWIG support, takes ownership of
  // attributes.
  Expression(
      Type type,
      std::vector<Expression> args,
      const ExpressionAttributes* attributes = nullptr)
      : data_(new Data{
          type, args, std::shared_ptr<const ExpressionAttributes>(attributes)}) {}

 private:
  struct Data {
    Type type;
    std::vector<Expression> args;
    std::shared_ptr<const ExpressionAttributes> attributes;
  };

  std::shared_ptr<Data> data_;
};

class Size {
 public:
  std::vector<int> dims;
};

class Problem {
 public:
  enum Sense {
    MAXIMIZE,
    MINIMIZE,
  };

  Problem() {}
  Problem(
      Sense sense,
      Expression objective,
      std::vector<Expression> constraints)
      : sense(sense),
        objective(objective),
        constraints(constraints) {}

  Sense sense;
  Expression objective;
  std::vector<Expression> constraints;
};

class ConstAttributes : public ExpressionAttributes {
 public:
  void set_dense_data(double* matrix, int rows, int cols);
  Size size() const;
  DenseMatrix dense_data;
};

struct VarAttributes : public ExpressionAttributes {
  int id;
  Size size;
};

struct PNormAttributes : public ExpressionAttributes {
  double p;
};

struct ReshapeAttributes : public ExpressionAttributes {
  Size size;
};

struct Slice {
  int start, stop, step;
};

struct IndexAttributes : public ExpressionAttributes {
  std::vector<Slice> keys;
};

#endif  // EXPRESSION_H
