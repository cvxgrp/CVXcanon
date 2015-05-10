#include <vector>
#include <cassert>
#include <map>
#include "Utils.hpp"

enum operatortype {
	VARIABLE,
	PROMOTE,
	MUL,
	RMUL,
	MUL_ELEM,
	DIV,
	SUM,
	NEG,
	INDEX,
	TRANSPOSE,
	SUM_ENTRIES,
	TRACE,
	RESHAPE,
	DIAG_VEC,
	DIAG_MAT,
	UPPER_TRI,
	CONV,
	HSTACK,
	VSTACK,
	SCALAR_CONST,
	DENSE_CONST,
	SPARSE_CONST,
	NO_OP,
	CONSTANT_ID
};

typedef operatortype OperatorType;

class LinOp{	
	public:
		OperatorType type;
		void* data;
		int size[2];
		std::vector< LinOp* > args;
		
		bool hasConstantType(){
			return  type == SCALAR_CONST || 
			type == DENSE_CONST || type == SPARSE_CONST;
		}
};

/***********************
 * FUNCTION PROTOTYPES *
 ***********************/
std::vector<Matrix> build_vector(Matrix &mat);
std::vector<Matrix> get_sum_coefficients(LinOp &lin);
std::map<int,Matrix> get_variable_coeffs(LinOp &lin);
std::map<int,Matrix> get_const_coeffs(LinOp &lin);
std::vector<Matrix> get_sum_entries_mat(LinOp &lin);
std::vector<Matrix> get_trace_mat(LinOp &lin);
std::vector<Matrix> get_neg_mat(LinOp &lin);
std::vector<Matrix> get_div_mat(LinOp &lin);
std::vector<Matrix> get_promote_mat(LinOp &lin);
std::vector<Matrix> get_mul_mat(LinOp &lin);
std::vector<Matrix> get_mul_elemwise_mat(LinOp &lin);
std::vector<Matrix> get_rmul_mat(LinOp &lin);
std::vector<Matrix> get_index_mat(LinOp &lin);
std::vector<Matrix> get_transpose_mat(LinOp &lin);
std::vector<Matrix> get_reshape_mat(LinOp &lin);
std::vector<Matrix> get_diag_vec_mat(LinOp &lin);
std::vector<Matrix> get_diag_matrix_mat(LinOp &lin);
std::vector<Matrix> get_upper_tri_mat(LinOp &lin);
std::vector<Matrix> get_conv_mat(LinOp &lin);
std::vector<Matrix> get_hstack_mat(LinOp &lin);
std::vector<Matrix> get_vstack_mat(LinOp &lin);

std::vector<Matrix> get_func_coeffs(LinOp& lin){
	switch(lin.type){
		case PROMOTE:
			return get_promote_mat(lin);
		case MUL:
			return get_mul_mat(lin);
		case RMUL:
			return get_rmul_mat(lin);
		case MUL_ELEM:
			return get_mul_elemwise_mat(lin);
		case DIV:
			return get_div_mat(lin);
		case SUM:
			return get_sum_coefficients(lin);
		case NEG:
			return get_neg_mat(lin);
		case INDEX:
			return get_index_mat(lin);
		case TRANSPOSE:
			return get_transpose_mat(lin);
		case SUM_ENTRIES:
			return get_sum_entries_mat(lin);
		case TRACE:
			return get_trace_mat(lin);
		case RESHAPE:
			return get_reshape_mat(lin);
		case DIAG_VEC:
			return get_diag_vec_mat(lin);
		case DIAG_MAT:
			return get_diag_matrix_mat(lin);
		case UPPER_TRI:
			return get_upper_tri_mat(lin);
		case CONV:
			return get_conv_mat(lin);
		case HSTACK:
			return get_hstack_mat(lin);
		case VSTACK:
			return get_vstack_mat(lin);
		case NO_OP:
			break;
	}
	std::vector<Matrix> tmp;
	return tmp;
}

/*******************
 * HELPER FUNCTIONS
 *******************/

std::vector<Matrix> build_vector(Matrix &mat){
	std::vector<Matrix> vec;
 	vec.push_back(mat);
 	return vec;
}

// returns N x N sparse identity
Matrix sparse_eye (int n){
	Matrix eye_n(n, n);
	eye_n.setIdentity();
	return eye_n;
}

// Todo: is this efficient? 
Matrix sparse_ones(int rows, int cols)
{	
 	Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(rows, cols);
 	Matrix mat = ones.sparseView();
 	return mat;
}

std::vector<Matrix> stack_matrices(LinOp &lin, bool vertical){
	std::vector<Matrix> coeffs_mats;

	//TODO:: IMPLEMENT ME!

	return coeffs_mats;
} 

/*****************************
 * LinOP -> Matrix FUNCTIONS
 *****************************/

std::vector<Matrix> get_vstack_mat(LinOp &lin){
	assert(lin.type == VSTACK);
	return stack_matrices(lin, true);
}

std::vector<Matrix> get_hstack_mat(LinOp &lin){
	assert(lin.type == HSTACK);
	return stack_matrices(lin, false);
}

std::vector<Matrix> get_conv_mat(LinOp &lin){
	assert(lin.type == CONV);

	//TODO:: IMPLEMENT ME!

	Matrix m = NULL_MATRIX;
	return build_vector(m);
}

std::vector<Matrix> get_upper_tri_mat(LinOp &lin){
	assert(lin.type == UPPER_TRI);
	int rows = lin.args[0]->size[0];
	int cols = lin.args[0]->size[1];

	int entries = lin.size[0];
	Matrix coeffs(entries, rows*cols);

	std::vector<Triplet> tripletList;
	tripletList.reserve((rows * cols) / float(2));
	int count = 0;
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if(j > i){
				// index in the extracted vector
				int row_idx = count;
				count++;
				// index in the original matrix
				int col_idx = j*rows + i;
				tripletList.push_back(Triplet(row_idx, col_idx, 1.0));
			}
		}
	}

	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}

std::vector<Matrix> get_diag_matrix_mat(LinOp &lin){
	assert(lin.type == DIAG_MAT);
	int rows = lin.size[0];

	Matrix coeffs(rows, rows * rows);
	std::vector<Triplet> tripletList;
	tripletList.reserve(rows);
	for(int i = 0; i < rows; i++){
		// index in the extracted vector
		int row_idx = i;
		// index in the original matrix
		int col_idx = i * rows + i;
		tripletList.push_back(Triplet(row_idx, col_idx, 1.0));
	}

	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}

std::vector<Matrix> get_diag_vec_mat(LinOp &lin){
	assert(lin.type == DIAG_VEC);
	int rows = lin.size[0];

	Matrix coeffs(rows * rows, rows);
	std::vector<Triplet> tripletList;
	tripletList.reserve(rows);
	for(int i = 0; i < rows; i++){
		// index in the diagonal matrix
		int row_idx = i*rows + i;
		//index in the original vector
		int col_idx = i;
		tripletList.push_back(Triplet(row_idx, col_idx, 1.0));
	}

	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}

std::vector<Matrix> get_transpose_mat(LinOp &lin){
	assert(lin.type == TRANSPOSE);
	int rows = lin.size[0];
	int cols = lin.size[1];

	Matrix coeffs(rows * cols, rows * cols);

	std::vector<Triplet> tripletList;
	tripletList.reserve(rows * cols);
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			int row_idx = rows * j + i;
			int col_idx = i * cols + j;
			tripletList.push_back(Triplet(row_idx, col_idx, 1.0));
		}
	}
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}

std::vector<Matrix> get_index_mat(LinOp &lin){
	assert(lin.type == INDEX);
	int rows = lin.args[0]->size[0];
	int cols = lin.args[0]->size[1];

	// TODO!!!! FIX THIS ONCE WE SETTLE ON A REPRESENTATION
	int* row_slice = (int*) lin.data;
	int* col_slice = (int*)lin.data + 3;

	std::vector<Triplet> tripletList;
	// could reserve less if slice[2] > 1...
	tripletList.reserve((row_slice[1] - row_slice[0]) * (col_slice[1] - col_slice[0]));
	int counter = 1;
	for(int row = row_slice[0]; row < row_slice[1]; row += row_slice[2]){
		for(int col = col_slice[0]; col < col_slice[1]; col += col_slice[2]){
			int row_idx = counter;
			counter++;
			int col_idx = col * rows + row;
			tripletList.push_back(Triplet(row_idx, col_idx, 1.0));
		}
	}

	Matrix coeffs (lin.size[0] * lin.size[1], rows*cols);
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}

std::vector<Matrix> get_mul_elemwise_mat(LinOp &lin){
	assert(lin.type == MUL_ELEM);

	Matrix constant = *(Matrix *) lin.data;
	int rows = constant.rows();
	int cols = constant.cols();

	// turn constant into a column vector
	constant.conservativeResize(rows * cols, 1);

	// build a giant diagonal matrix
	std::vector<Triplet> tripletList;
	tripletList.reserve(rows * cols);
	for(int i = 0; i < rows * cols; i++){
		// TODO: coeffref is slow due to nature of sparse matrix elem access.
		// speed this up by iterating over elements of sparse matrix first,
		// or storing the point to a dense matrix instead of a sparse one.
		tripletList.push_back(Triplet(i, i, constant.coeffRef(i, 1)));
	}

	Matrix coeffs(rows * cols, rows * cols);
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}

std::vector<Matrix> get_rmul_mat(LinOp &lin){
	assert(lin.type == RMUL);

	Matrix constant = *(Matrix *) lin.data;
	
	Matrix ident = sparse_eye(lin.size[0]);

	// Matrix coeffs = Eigen::KroneckerProductSparse(constant.transpose(), ident);

	// TODO! (Figure out how to use this function to avoid more nasty for-loops.)

	return build_vector(ident);
}


std::vector<Matrix> get_mul_mat(LinOp &lin){
	assert(lin.type == MUL);
	// assumes lin.data points to a sparse eigen matrix.
	Matrix block = *(Matrix *) lin.data;
	int block_rows = block.rows();
	int block_cols = block.cols();
	int num_blocks = lin.size[1];

	Matrix coeffs (num_blocks * block_rows, num_blocks * block_cols);

	// There is probably a more efficient way to do this, but eigen sparse
	// matrix documentation is, well, sparse.
	std::vector<Triplet> tripletList;
	tripletList.reserve(num_blocks * block_rows * block_cols);
	for(int curr_block = 0; curr_block < num_blocks; curr_block++){
		int start_i = curr_block * block_rows;
		int start_j = curr_block * block_cols;
		for(int i = 0; i < block_rows; i++){
			for(int j = 0; j < block_cols; j++){
				// TODO: coeffref is slow due to nature of sparse matrix elem access.
				// speed this up by iterating over elements of sparse matrix first,
				// or storing the point to a dense matrix instead of a sparse one.
				tripletList.push_back(Triplet(start_i + i, start_j + j,
															block.coeffRef(i, j)));
			}
		}
	}
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}


std::vector<Matrix> get_promote_mat(LinOp &lin){
	assert(lin.type == PROMOTE);
	int num_entries = lin.size[0] * lin.size[1];
	Matrix ones = sparse_ones(num_entries, 1);
	return build_vector(ones);
}

std::vector<Matrix> get_reshape_mat(LinOp &lin){
	assert(lin.type == RESHAPE);
	Matrix one(1,1);
	one.insert(0, 0) = 1;
	return build_vector(one);
}


std::vector<Matrix> get_div_mat(LinOp &lin){
	assert(lin.type == DIV);
	// assume we are dividing by a scalar
	double divisor = *(double *) lin.data;
	int n = lin.size[0] * lin.size[1];
	Matrix coeffs = sparse_eye(n);
	coeffs /= divisor;
	return build_vector(coeffs);
}

/**
 * Todo...
 *
 */
std::vector<Matrix> get_neg_mat(LinOp &lin){
	assert(lin.type == NEG);
 	int n = lin.size[0] * lin.size[1];
	Matrix coeffs = sparse_eye(n);
	coeffs *= -1;
	return build_vector(coeffs);
}

/**
 * Todo...
 *
 */
 std::vector<Matrix> get_trace_mat(LinOp &lin){
 	assert(lin.type == TRACE);
 	int rows = lin.args[0]->size[0];
 	Matrix mat (1, rows*rows);
 	for(int i = 0; i < rows; i++){
 		mat.insert(0, i*rows + i) = 1;
 	}
 	return build_vector(mat);
 }

/**
 * Return the coefficient matrix for SUM_ENTRIES
 *
 * Parameters: LinOp with type SUM_ENTRIES
 * 
 * Returns: A vector of length N where each element is a 1x1 matrix
 */
 std::vector<Matrix> get_sum_entries_mat(LinOp &lin){
 	assert(lin.type == SUM_ENTRIES);
 	// assumes all args have the same size
 	int rows = lin.args[0]->size[0];
 	int cols = lin.args[0]->size[1];
 	Matrix mat = sparse_ones(1, rows * cols);
 	return build_vector(mat);
 }

/**
 * Return the coefficient matrix for SUM
 *
 * Parameters: LinOp with type SUM
 * 
 * Returns: A vector of length N where each element is a 1x1 matrix
 */
std::vector<Matrix> get_sum_coefficients(LinOp &lin){
	assert(lin.type == SUM);
	int n = lin.args.size();
	std::vector<Matrix> coeffs;
	Matrix scalar(1,1);
	scalar.insert(0,0) = 1;
	for(int i = 0; i < n; i++){
		coeffs.push_back(scalar);
	}
	return coeffs;
}

/**
 * Return the coefficients for a VARIABLE.
 *
 * Parameters:
 * 
 * Returns:
 *
 */
std::map<int,Matrix> get_variable_coeffs(LinOp &lin){
	assert(lin.type == VARIABLE);
	std::map<int, Matrix> id_to_coeffs;
	int id = *(int*) lin.data;

	// create a giant identity matrix
	int n = lin.size[0] * lin.size[1];
	Matrix coeffs = sparse_eye(n);
	id_to_coeffs[id] = coeffs;
	return id_to_coeffs;
}

/**
 * Returns the matrix for a constant type
 *
 * Parameters:
 *
 * Returns:
 */
std::map<int,Matrix> get_const_coeffs(LinOp &lin){
	assert(lin.hasConstantType());
	std::map<int, Matrix> id_to_coeffs;
	int id = CONSTANT_ID;

	// WARNING!!! This code assumes const_coeffs is pointing to a sparse Eigen 
	// matrix.... TODO: Verify this always holds or special case as needed
	Matrix coeffs = *(Matrix *) lin.data;

	// reshape coeffs into a column vector
	coeffs.conservativeResize(coeffs.rows()*coeffs.cols(), 1);

 	id_to_coeffs[id] = coeffs;
	return id_to_coeffs;
}
