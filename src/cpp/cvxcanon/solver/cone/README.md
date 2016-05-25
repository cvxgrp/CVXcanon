# Cone solver framework

The `ConeSolver` interface is inspired by [conic problems in
MathProgBase](http://mathprogbasejl.readthedocs.io/en/latest/conic.html). In
particular, solvers implementing this interface accept problems of the form

$$ \begin{split} \mathrm{minimize} \;\; & c^Tx  \\
\mathrm{subject\;to} \;\; & b - Ax \in \mathcal{K}
\end{split}
$$

where $A$ is a sparse matrix, $b$ and $c$ are dense vectors and $\mathcal{K}$ is
the cross product containing any of the following cones

- zero cone (linear equality constraints): $\{x \mid x = 0\}$
- nonnegative cone (linear inequality): $\{x \mid x \ge 0\}$
- second order cone: $\{(x,y) \mid \|x\|_2 \le y\}$
- exponential cone:  $\{(x,y,z) \mid y > 0, ye^{x/y} \le z\} \cup \{(x,y,z) \mid x \le 0, y = 0, z \ge 0\}$

See `ConeSolver.hpp` for details. In essence, a solver must implement the
following method:

```
ConeSolution solve(const ConeProblem& problem)
```

TODO(mwytock): this inferface should likely be specified in pure C (as opposed
to C++) to mesh better with existing cone solvers. In addition, we likely want a
plugin architecture such that third party libraries can implement the cone
solver interface without requiring any modification to CVXcanon.