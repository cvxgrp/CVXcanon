// Solves a convex optimization problem in linear cone form.

#ifndef CVXCANON_SOLVER_SYMBOLIC_CONE_SOLVER_H
#define CVXCANON_SOLVER_SYMBOLIC_CONE_SOLVER_H

#include <map>
#include <utility>

#include "cvxcanon/solver/cone/ConeSolver.hpp"
#include "cvxcanon/solver/Solver.hpp"

// Maps variables into a single vector
// TODO(mwytock): This should be refactored to go elsewhere.
class VariableOffsetMap {
 public:
  VariableOffsetMap() : n_(0) {}

  // Inserts a new variable id (if necessary) and returns its offset into the
  // vector.
  int insert(int var_id, int size) {
    auto ret = map_.insert(std::make_pair(var_id, std::make_pair(n_, size)));
    if (ret.second)
      n_ += size;
    return ret.first->second.first;
  }

  // Accessors
  int n() const { return n_; }
  const std::map<int, std::pair<int, int>>& map() const { return map_; }

 private:
  // total size of all variables
  int n_;

  // var_id -> (offset, size)
  std::map<int, std::pair<int, int>> map_;
};

// Solves a linear cone problem. The main function of this class is in
// transforming the linear cone problem from AST form (i.e. Problem) to matrix
// representing (A, b, c) along with a set of cones. After performing this
// transformation, SymbolicConeSolver simply calls the specified ConeSolver on
// the problem and returns the solution.
class SymbolicConeSolver : public Solver {
 public:
  // ConeSolver will be used to
  explicit SymbolicConeSolver(std::unique_ptr<ConeSolver> cone_solver);

  // Precondition: problem must be in linear cone form.
  Solution solve(const Problem& problem) override;

 private:
  std::unique_ptr<ConeSolver> cone_solver_;
};

#endif  // CVXCANON_SOLVER_SYMBOLIC_CONE_SOLVER_H
