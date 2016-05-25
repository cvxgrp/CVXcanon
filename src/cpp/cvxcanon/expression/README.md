
# Expression trees

This directory holds the representation for expression trees which are the
building block of an optimization problem. The `Expression` class represents a
single node in the abstract syntax tree (AST) and includes a type, pointers to
children and an optional pointer to additional attributes. Note that the
`Expression` class can also represent DAGs as it is possible for a  node to have
multiple parents.

Given ASTs for expressions we define an optimization problem (the `Problem`
class) with an expression for the objective, expressions for zero or more
constraints and a problem sense (either `MINIMIZE` or `MAXIMIZE`).

In addition to expression tree representation, the following functionality is
provided:

- `ExpressionShape` computes the dimensions of functions represented by an expression
- `ExpressionUtil` provides syntatic sugar and other utilities for constructing
- and manipulating expression trees
- `LinearExpression` transforms expression trees representing linear
functions to sparse matrices
- `TextFormat` formats expression trees concisely for printing
