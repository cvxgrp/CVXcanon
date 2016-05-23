// Builds matrices for the canonical cone form and passes to ConeSolver

#ifndef SYMBOLIC_CONE_SOLVER_H
#define SYMBOLIC_CONE_SOLVER_H

#include "cvxcanon/solver/ConeSolver.hpp"
#include "cvxcanon/solver/Solver.hpp"

class VariableOffsetMap {
 public:
  VariableOffsetMap() : n_(0) {}

  // returns offset, inserting if necssary
  int insert(int var_id, int size) {
    auto ret = map_.insert(std::make_pair(var_id, std::make_pair(n_, size)));
    if (ret.second)
      n_ += size;
    return ret.first->second.first;
  }

  int n() const { return n_; }
  const std::map<int, std::pair<int, int>>& map() const { return map_; }

 private:
  int n_;

  // var_id -> (offset, size)
  std::map<int, std::pair<int, int>> map_;
};

class SymbolicConeSolver : public Solver {
 public:
  SymbolicConeSolver(std::unique_ptr<ConeSolver> solver);
  Solution solve(const Problem& problem) override;

 private:
  std::unique_ptr<ConeSolver> cone_solver_;
};

#endif  // SYMBOLIC_CONE_SOLVER_H
