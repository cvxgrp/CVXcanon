
#include <unordered_set>

typedef std::unordered_map<int, int> VariableMap;

class VariableOffsetMap {
 public:
  std::unordered_set<int> variable_ids() const;
  int offset(int var_id) const;
  int size(int var_id) const;

  void insert(int var_id, int size);
};

// Coefficients of an affine function Ax + b
class AffineCoefficients {
 public:
  Eigen::SparseMatrix<double> A;
  Eigen::VectorXd b;
};

VariableOffsetMap build_variable_offset_map(const Problem& problem);
AffineCoefficients build_coefficients(
    const Expression& expr,
    const VariableOffsetMap& var_offsets);
