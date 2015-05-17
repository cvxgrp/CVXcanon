#include "LinOp.hpp"
#include "LinOpOperations.hpp"
#include "Utils.hpp"
#include <cassert>
#include <map>
#include <iostream>

/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

std::vector<Matrix> build_vector(Matrix &mat);
std::vector<Matrix> get_sum_coefficients(LinOp &lin);
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
	printf("LinOp switch statement.\n");
	printf("Type: %d\n", lin.type);
	std::vector<Matrix> coeffs;
	switch(lin.type){
		case PROMOTE:
			coeffs = get_promote_mat(lin);
			break;
		case MUL:
			coeffs = get_mul_mat(lin);
			break;
		case RMUL:
			coeffs = get_rmul_mat(lin);
			break;
		case MUL_ELEM:
			coeffs = get_mul_elemwise_mat(lin);
			break;
		case DIV:
			coeffs = get_div_mat(lin);
			break;
		case SUM:
			coeffs = get_sum_coefficients(lin);
			break;
		case NEG:
			coeffs = get_neg_mat(lin);
			break;
		case INDEX:
			coeffs = get_index_mat(lin);
			break;
		case TRANSPOSE:
			coeffs = get_transpose_mat(lin);
			break;
		case SUM_ENTRIES:
			coeffs = get_sum_entries_mat(lin);
			break;
		case TRACE:
			coeffs = get_trace_mat(lin);
			break;
		case RESHAPE:
			coeffs = get_reshape_mat(lin);
			break;
		case DIAG_VEC:
			coeffs = get_diag_vec_mat(lin);
			break;
		case DIAG_MAT:
			coeffs = get_diag_matrix_mat(lin);
			break;
		case UPPER_TRI:
			coeffs = get_upper_tri_mat(lin);
			break;
		case CONV:
			coeffs = get_conv_mat(lin);
			break;
		case HSTACK:
			coeffs = get_hstack_mat(lin);
			break;
		case VSTACK:
			coeffs = get_vstack_mat(lin);
			break;
		default:
			std::cout << "INVALID LINOP!" << std::endl;
			exit(-1);
	}
	printf("Returning from LinOp Switch\n");
	return coeffs;
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

	// makes a coefficient for each argument,
	// essentially an identity with an offset.
	int offset = 0;
	int num_args = lin.args.size();
	for(int idx = 0; idx < num_args; idx++){
		LinOp arg = *lin.args[idx];

		// if we are using vstack, the arguments have columns that are interleaved.
		// in hstack, the arguments are laid out in order. 
		int column_offset;
		int offset_increment;
		if(vertical){
			column_offset = lin.size[0];
			offset_increment = arg.size[0];
		} else {
			column_offset = arg.size[0];
			offset_increment = arg.size[0] * arg.size[1];
		}

		std::vector<Triplet> tripletList;
		tripletList.reserve(arg.size[0] * arg.size[1]);
		for(int i = 0; i < arg.size[0]; i++){
			for(int j = 0; j < arg.size[1]; j++){
				int row_idx = i + (j * column_offset) + offset;
				int col_idx = i + (j * arg.size[0]);
				tripletList.push_back(Triplet(row_idx, col_idx, 1));
			}
		}

		Matrix coeff(lin.size[0] * lin.size[1], arg.size[0] * arg.size[1]);
		coeff.setFromTriplets(tripletList.begin(), tripletList.end());
		coeffs_mats.push_back(coeff);
		offset += offset_increment;
	}
	return coeffs_mats;
} 

Matrix get_constant_data_as_column(LinOp &lin){
	int rows = lin.data.size();
	int cols = lin.data[0].size();
	Matrix coeffs (rows * cols, 1);

	std::vector<Triplet> tripletList;
	tripletList.reserve(rows * cols);
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			tripletList.push_back(Triplet(cols * i +  j, 0, lin.data[i][j]));
		}
	}
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return coeffs;
}

// TODO: profile and see if it would be more efficient to not treat
// everything as a sparse matrix.
Matrix get_constant_data(LinOp &lin){
	int rows = lin.data.size();
	int cols = lin.data[0].size();
	Matrix coeffs (rows, cols);

	std::vector<Triplet> tripletList;
	tripletList.reserve(rows * cols);
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			tripletList.push_back(Triplet(i, j, lin.data[i][j]));
		}
	}
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return coeffs;
}

std::vector<std::vector<int> > get_slice_data(LinOp &lin){
	assert(lin.type==INDEX);
	std::vector<double> row = lin.data[0];
	std::vector<double> col = lin.data[1];
	assert(row.size() == 3);
	assert(col.size() == 3);

	std::vector<int> row_slice;
	std::vector<int> col_slice;
	std::vector<std::vector<int> > slices;
	for(int i = 0; i < 3; i++){
		row_slice.push_back(int(row[i]));
		col_slice.push_back(int(col[i]));
	}
	slices.push_back(row_slice);
	slices.push_back(col_slice);

	return slices;
}

double get_divisor_data(LinOp &lin){
	assert(lin.type==DIV);
	return lin.data[0][0];
}

int get_id_data(LinOp &lin){
	assert(lin.type==VARIABLE);
	return int(lin.data[0][0]);
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
	Matrix constant = get_constant_data(lin);

	// Convolution is implemented by creating a toeplitz matrix with constant
	// as columns
	int rows = lin.size[0];
	int nonzeros = constant.rows();
	int cols = lin.args[0]->size[0];

	Matrix toeplitz(rows, cols);

	std::vector<Triplet> tripletList;
	tripletList.reserve(nonzeros * cols);
	for(int col = 0; col < cols; col++){
		int row_start = col;
		for(int i = 0; i < nonzeros; i++){
			int row_idx = row_start + i;

			// change this depending representation
			double val = constant.coeffRef(i, 0);
			tripletList.push_back(Triplet(row_idx, col, val));
		}
	}
	toeplitz.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(toeplitz);
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
	std::vector<std::vector<int> > slices = get_slice_data(lin);
	std::vector<int> row_slice = slices[0];
	std::vector<int> col_slice = slices[1];
	int rows = lin.args[0]->size[0];
	int cols = lin.args[0]->size[1];
	
	std::vector<Triplet> tripletList;
	// could reserve less if slice[2] > 1...
	tripletList.reserve((row_slice[1] - row_slice[0]) * (col_slice[1] - col_slice[0]));
	int counter = 0;
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
	Matrix constant = get_constant_data_as_column(lin);
	int n = constant.rows();

	// build a giant diagonal matrix
	std::vector<Triplet> tripletList;
	tripletList.reserve(n);
	for(int i = 0; i < n; i++){
		// TODO: coeffref is slow due to nature of sparse matrix elem access.
		// speed this up by iterating over elements of sparse matrix first,
		// or storing the point to a dense matrix instead of a sparse one.
		tripletList.push_back(Triplet(i, i, constant.coeffRef(i, 0)));
	}

	Matrix coeffs(n, n);
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}

std::vector<Matrix> get_rmul_mat(LinOp &lin){
	assert(lin.type == RMUL);
	Matrix constant = get_constant_data(lin);
	int rows = constant.rows();
	int cols = constant.cols();
	int n = lin.size[0];

	// coeffs is the kronecker product between the transpose of the constant
	// and a lin.size[0] identity matrix
	Matrix coeffs(cols * n, rows * n);
	std::vector<Triplet> tripletList;
	tripletList.reserve(rows * cols * n);
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			double val = constant.coeffRef(row, col);

			int row_start = col * n; 
			int col_start = row * n;
			for(int i = 0; i < n; i++){
				int row_idx = row_start + i;
				int col_idx = col_start + i;
				tripletList.push_back(Triplet(row_idx, col_idx, val));
			}
		}
	}
	coeffs.setFromTriplets(tripletList.begin(), tripletList.end());
	return build_vector(coeffs);
}


std::vector<Matrix> get_mul_mat(LinOp &lin){
	assert(lin.type == MUL);
	// assumes lin.data points to a sparse eigen matrix.
	Matrix block = get_constant_data(lin);

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
	double divisor = get_divisor_data(lin);
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

 	// std::cout << "Size of neg mat " << n << std::endl;
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

	// printf("In SUM: Size is %d\n", n);
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
	int id = get_id_data(lin);

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

	// get coeffs as a column vector
	Matrix coeffs = get_constant_data_as_column(lin);

 	id_to_coeffs[id] = coeffs;
	return id_to_coeffs;
}