// Expressions representing the abstract syntax tree for an optimization problem

#ifndef CVXCANON_EXPRESSION_EXPRESSION_H
#define CVXCANON_EXPRESSION_EXPRESSION_H

#include <vector>
#include <memory>

#include "cvxcanon/util/Utils.hpp"

// Additional attributes of an Expression
struct ExpressionAttributes {
  virtual ~ExpressionAttributes() {}
};

const int kNoAxis = -1;

// A single node in the abstract syntax tree which includes a type, children and
// an optional pointer to additional attributes.
//
// As they are lightweight (containing only a type and pointers), expressions
// are intended to be immutable and should simply be recreated rather than
// modified.
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
    TRANSPOSE,
    UPPER_TRI,
    VSTACK,

    // Elementwise nonlinear functions
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
    EXP_CONE,

    // Leaf nodes
    CONST,
    PARAM,
    VAR,
  };

  // The type of this expression
  Type type() const { return data_->type; }

  // Accessors for the arguments (children) of this expression
  const std::vector<Expression>& args() const { return data_->args; }
  const Expression& arg(int i) const { return data_->args[i]; }

  // Accessors for the additional attributes
  // Example usage:
  //   const int p = expression.attr<PNormAttributes>().p;
  template<typename T>
  const T& attr() const {
    CHECK(data_->attributes.get() != nullptr);
    return static_cast<const T&>(*data_->attributes);
  }
  std::shared_ptr<const ExpressionAttributes> attr_ptr() const {
    return data_->attributes;
  }

  Expression(
      Type type,
      std::vector<Expression> args,
      std::shared_ptr<const ExpressionAttributes> attributes)
      : data_(new Data{type, args, attributes}) {}

  // The following two constructors are convenient for SWIG support and are not
  // intended to be used as well
  Expression() {}

  // Takes ownership of the naked
  Expression(
      Type type,
      std::vector<Expression> args,
      const ExpressionAttributes* attributes = nullptr)
      : data_(new Data{
          type, args,
              std::shared_ptr<const ExpressionAttributes>(attributes)}) {}

 private:
  // The actual implementation
  struct Data {
    Type type;
    std::vector<Expression> args;
    std::shared_ptr<const ExpressionAttributes> attributes;
  };

  std::shared_ptr<Data> data_;
};

// An optimization problem
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


// Represents the size of n-dimensional array
class Size {
 public:
  std::vector<int> dims;
};

// Expression attributes: in the remainder of the file we define type-specific
// attributes for each Expression that requires them. Note that there is a 1:1
// mapping between an expression type (e.g. CONST) and a subclass of
// ExpressionAttributes (e.g. ConstAttributes).

class ConstAttributes : public ExpressionAttributes {
 public:
  void set_dense_data(double* matrix, int rows, int cols);
  void set_sparse_data(double* data, int data_len,
                       double* row_idxs, int rows_len,
                       double* col_idxs, int cols_len,
                       int rows, int cols);
  Size size() const;

  bool sparse;
  DenseMatrix dense_data;
  SparseMatrix sparse_data;
};


struct VarAttributes : public ExpressionAttributes {
  enum Type {
    UNKNOWN,
    SEMIDEF_UPPER_TRI,
  };
  int id;
  Size size;
  Type variable_type;
};

struct PNormAttributes : public ExpressionAttributes {
  double p;
  int axis;
};

struct PowerAttributes : public ExpressionAttributes {
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

struct HuberAttributes : public ExpressionAttributes {
  Expression M;
};

struct SumEntriesAttributes : public ExpressionAttributes {
  int axis;
};

#endif  // CVXCANON_EXPRESISON_EXPRESSION_H
