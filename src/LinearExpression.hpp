
typedef std::unordered_map<int, int> VariableMap;

class VariableOffsetMap {
 public:

};

// Coefficients of an affine function Ax + b
class AffineCoefficients {
 public:
  Eigen::SparseMatrix<double> A;
  Eigen::VectorXd b;
};


VariableOffsetMap build_variable_offset_map(const Problem& problem);
AffineCoefficients build_coefficients(
    const Expression& expr, const VarOffsetMap& var_offsets);
