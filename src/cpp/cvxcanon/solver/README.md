# Solver framework

Here we define the basic `Solver` framework for a general optimization problem
represented in AST form. A `Solver` subclass provides the method

```
Solution solve(const Problem& problem);
```

which produces a solution for a given problem, see `Solver.hpp` for details.

It is expected that a given a solver is specialized to a certain subset of
problems. For example, `SymbolicConeSolver` assumes that its input is
representing in a canonical cone form (linear objective with cone
constraints, see the `cone` subdirectory for details.). Given a problem in this
form, `SymbolicConeSolver` instantiates the matrices representing the objective
and constraints and calls a cone solver.
