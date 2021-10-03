#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gen_matrices.h"

/*  TODO: 
 *  [x] process cl args, do checks on k, n, num row drops, and whatever else
 *  [x] load mul/div tables into arrays (probs need to #define possible dimensions 
 *     beforehand and init based on cl args unless there's a better way, i guess 
 *	   could just allocate max table size and then smaller ones only use what they need)
 *  [x] generate x, y sets for cauchy
 *  [x] generate cauchy matrix
 *  [x] expand matrix (used to create final encoder/decoder)
 *  [x] how 2 "drop" rows?
 *  [] invert cauchy matrix
 *  [] function to write encode_decode.c containing arrays of encoder/decoder
 *  [] function to write encode_decode.h
 */

int main(int argc, char **argv) {
	char arg_status_str[MAX_STATUS_LEN] = "";
	int k;
	int n;
	int overwrite;
	int field;
	int arg_status = handle_args(argc, argv, arg_status_str, &k, &n, &overwrite, &field);

	if (arg_status != OK) {
		fprintf(stderr, "%s\n", arg_status_str);
		return arg_status;
	}
	printf("k = %d, n = %d, overwrite = %d, arg_status = %d, field = %d\n", k, n, overwrite, arg_status, field);

	// TODO should consider dynamic mem allocation for tables and path strings

	// 2d arrays of finite arithmetic tables
	int mul_table[MAX_TABLE][MAX_TABLE];
	int div_table[MAX_TABLE][MAX_TABLE];

	// table file paths determined by field number below
	char mul_path[MAX_PATH_LEN] = "";
	char div_path[MAX_PATH_LEN] = "";

	// these will be set to a constant in the header file based on the field number below
	int num_rows;
	int num_cols;

	// determine paths based on field 
	switch (field) {
		case 6:
			strncpy(mul_path, "../tables/multiplication/2to6_1000011.txt", MAX_PATH_LEN);
			mul_path[sizeof("../tables/multiplication/2to6_1000011.txt") - 1] = '\0';
			strncpy(div_path, "../tables/division/2to6_1000011.txt", MAX_PATH_LEN);
			div_path[sizeof("../tables/division/2to6_1000011.txt") - 1] = '\0';

			num_rows = FIELD_6_DIMS;
			num_cols = FIELD_6_DIMS;
			break;

		case 5:
			strncpy(mul_path, "../tables/multiplication/2to5_100101.txt", MAX_PATH_LEN);
			mul_path[sizeof("../tables/multiplication/2to5_100101.txt") - 1] = '\0';
			strncpy(div_path, "../tables/division/2to5_100101.txt", MAX_PATH_LEN);
			div_path[sizeof("../tables/division/2to5_100101.txt") - 1] = '\0';

			num_rows = FIELD_5_DIMS;
			num_cols = FIELD_5_DIMS;
			break;

		case 4:
			strncpy(mul_path, "../tables/multiplication/2to4_10011.txt", MAX_PATH_LEN);
			mul_path[sizeof("../tables/multiplication/2to4_10011.txt") - 1] = '\0';
			strncpy(div_path, "../tables/division/2to4_10011.txt", MAX_PATH_LEN);
			div_path[sizeof("../tables/division/2to4_10011.txt") - 1] = '\0';

			num_rows = FIELD_4_DIMS;
			num_cols = FIELD_4_DIMS;
			break;

		case 3:
			strncpy(mul_path, "../tables/multiplication/2to3_1011.txt", MAX_PATH_LEN);
			mul_path[sizeof("../tables/multiplication/2to3_1011.txt") - 1] = '\0';
			strncpy(div_path, "../tables/division/2to3_1011.txt", MAX_PATH_LEN);
			div_path[sizeof("../tables/division/2to3_1011.txt") - 1] = '\0';

			num_rows = FIELD_3_DIMS;
			num_cols = FIELD_3_DIMS;
			break;

		default:
			return INVALID_FIELD;
	}

	// get multiplication/division table
	load_table(field, mul_path, mul_table, num_rows, num_cols);
	load_table(field, div_path, div_table, num_rows, num_cols);

	// allocate mem for x and y disjoint sets (used to make a Cauchy matrix
	// of (k + n) x n dims
	int *x = (int *) malloc(sizeof(int) * (k + n));
	int *y = (int *) malloc(sizeof(int) * n);

	// generate x and y sets
	gen_x_y(k, n, x, y);

	// allocate mem for 2d cauchy matrix
	// cauchy is (k + n) by n (or length(x) by length(y)), how many times have I commented this?
	int *cauchy[k + n];
	for (int i = 0; i < (k + n); i++) {
		cauchy[i] = (int *) malloc(n * sizeof(int));
	}

	// generate cauchy matrix
	gen_cauchy(cauchy, x, y, k, n, div_table);

	// allocate mem for 2d expanded cauchy matrix
	// expanded_cauchy has dims: field * (k + n) x field * n
	int *expanded_cauchy[field * (k + n)];
	for (int i = 0; i < (field * (k + n)); i++) {
		expanded_cauchy[i] = (int *) malloc(field * n * sizeof(int));
	}

	// generate expanded cauchy matrix
	expand_matrix(cauchy, expanded_cauchy, field, k, n, mul_table);

	// TODO this should either be a cl arg or should be determined somewhere based on
	// the shards that were not returned, for now just dropping the first 
	// create array of row indexes to drop from cauchy (need to drop k to make square)
	int row_drops[k];
	for (int i = 0; i < k; i++) {
		row_drops[i] = i;
	}

	// allocate mem for the square cauchy (with rows dropped)
	int *sq_cauchy[n];
	for (int i = 0; i < n; i++) {
		sq_cauchy[i] = (int *) malloc(n * sizeof(int));
	}

	// create the square cauchy with rows removed
	// TODO check output more thoroughly
	create_sq_matrix(cauchy, sq_cauchy, row_drops, n);

	// FREEDOM
	free(x);
	free(y);
	for (int i = 0; i < (k + n); i++) {
		free(cauchy[i]);
	}
	for (int i = 0; i < (field * (k + n)); i++) {
		free(expanded_cauchy[i]);
	}
	for (int i = 0; i < n; i++) {
		free(sq_cauchy[i]);
	}

	return 0;
}

/*
 *  Processes command line arguments. Does checks, decides on field. 
 *  For now, args are all required, should be:
 *  ./gen_matrices [k] [n] [overwrite_bool]
 *  TODO should include option for drop rows (i.e. the erasures, the row indexes that are missing) 
 */

int handle_args(int argc, char **argv, char arg_status[MAX_STATUS_LEN], int *k, int *n, int *overwrite, int *field) {
	// all args required
	if (argc != 4) {
		strncpy(arg_status, "Need 3 arguments corresponding to k, n, and overwrite boolean.\nExample: ./gen_matrices 4 2 1", MAX_STATUS_LEN);
		arg_status[sizeof("Need 3 arguments corresponding to k, n, and overwrite boolean.\nExample: ./gen_matrices 4 2 1") - 1] = '\0';
		return BAD_ARGS;
	}

	// get args corresponding to k, n, overwrite bool as ints
	*k = atoi(argv[1]);
	*n = atoi(argv[2]);
	*overwrite = atoi(argv[3]);

	if ((*k <= 0) || (*n < 2)) {
		strncpy(arg_status, "Ensure that k is greater than 0 and that n is greater than 2.", MAX_STATUS_LEN);
		arg_status[sizeof("Ensure that k is greater than 0 and that n is greater than 2.") - 1] = '\0';
		return BAD_ARGS;
	} 

	if ((*overwrite != 0) && (*overwrite != 1)) {
		strncpy(arg_status, "The overwrite boolean must be either 0 or 1.", MAX_STATUS_LEN);
		arg_status[sizeof("The overwrite boolean must be either 0 or 1.") - 1] = '\0';
		return BAD_ARGS;	
	}

	// pretty sure k < n is possible, but assuming k > n for now
	if (*k < *n) {
		strncpy(arg_status, "k must be greater than n.", MAX_STATUS_LEN);
		arg_status[sizeof("k must be greater than n.") - 1] = '\0';
		return BAD_ARGS;
	}

	// need a cauchy matrix dims (k + n) x n
	// which means need a set (x) of length (k + n) disjoint of another set (y) of length n
	// so total number of unique numbers is k + n + n 
	int unique_nums = *k + (2 * *n);

	// determine the finite field power based on the number of unique numbers required for k and n
	if (unique_nums > pow(2, 6)) {
		strncpy(arg_status, "The inputted values of k and n require a finite field larger than 2**6, try smaller values.", MAX_STATUS_LEN);
		arg_status[sizeof("The inputted values of k and n require a finite field larger than 2**6, try smaller values.") - 1] = '\0';
		return BAD_ARGS;
	} else if ((pow(2, 5) < unique_nums) && (unique_nums <= pow(2, 6))) {
		*field = 6;
	} else if ((pow(2, 4) < unique_nums) && (unique_nums <= pow(2, 5))) {
		*field = 5;
	} else if ((pow(2, 3) < unique_nums) && (unique_nums <= pow(2, 4))) {
		*field = 4;
	} else {
		*field = 3;
	}
	
	return OK;
}

/*
 *  Loads in a finite field arithmetic table from one of the text files in 
 *  the tables directory. Each line corresponds to a table row, numbers are
 *  seperated by spaces. mul_table[2][4] means 2 * 4 (in that finite field).
 */

int load_table(int field, char path[MAX_PATH_LEN], int table[MAX_TABLE][MAX_TABLE], int num_rows, int num_cols) {
	FILE *file_ptr = fopen(path, "r");

	if (file_ptr == NULL) {
		return FILE_READ_ERR;
	}

	// each line of file is row in arithmetic table, nums sep by space
	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			fscanf(file_ptr, "%d", &table[i][j]);
		}
	}

	fclose(file_ptr);

	return OK;
}

/*
 *  Generates two disjoint sets (x and y). x is length of (k + n), y is length
 *  of n. Sets are later used to generate Cauchy matrix of (k + n) x n. For 
 *  now, the numbers in these sets just increase linearly. Could change how
 *  and where numbers are assigned to create a different generator matrix (this
 *  may be desirable for security reasons? not entirely sure if important).
 */

void gen_x_y(int k, int n, int *x, int *y) {
	for (int i = 0; i < (k + n); i++) {
		x[i] = i;
	}

	// y needs to start at the last value of x + 1, but still needs idx from 0
	int y_idx = 0;

	for (int j = (k + n); j < (k + 2 * n); j++) {
		y[y_idx] = j;
		y_idx++;
	}
}

/*
 *  Generates a length(x) by length(y) Cauchy matrix. If x and y are generated
 *  correctly, this will be a (k + n) by n matrix.
 *  See: https://en.wikipedia.org/wiki/Cauchy_matrix
 */

void gen_cauchy(int **cauchy, int *x, int *y, int k, int n, int div_table[MAX_TABLE][MAX_TABLE]) {
	for (int i = 0; i < (k + n); i++) {
		for (int j = 0; j < n; j++) {
			// note that div_table[1][3] means 1 div 3 in the finite field, and xor 
			// is subtraction/addition 
			cauchy[i][j] = div_table[1][x[i] ^ y[j]];
		}
	}
}

/*
 *  Given in_matrix (either cauchy or inverted cauchy), expand each element 
 *  with basis vectors in field. For a confusing explanation that is slightly
 *  different from this implementation, see: https://arxiv.org/pdf/1611.09968.pdf
 *  out_matrix is updated (should have dims: field * (k + n) x field * n)
 */

void expand_matrix(int **in_matrix, int **out_matrix, int field, int k, int n, int mul_table[MAX_TABLE][MAX_TABLE]) {
	// create basis "vectors" for the given field (representing each as a num is easier)
	int basis_nums[field];
	for (int i = 0; i < field; i++) {
		basis_nums[i] = pow(2, i);
	}
	int count = 0;

	/*
	 *  Iterates over each row, each column of in_matrix. Then creates a field * field
	 *  matrix using basis_nums, this matrix represents the expanded version of the 
	 *  single number in the original in_matrix. This expanded matrix is put into
	 *  out_matrix for each element in in_matrix. This is very confusing.
	 */

	for (int row = 0; row < (k + n); row++) {

		for (int col = 0; col < n; col++) {

			for (int basis_idx = 0; basis_idx < field; basis_idx++) {
				// get the product between the number from in_matrix and the basis num
				int product = mul_table[in_matrix[row][col]][basis_nums[basis_idx]];

				for (int bit_idx = 0; bit_idx < field; bit_idx++) {
				  /* Sets the correct idx in out_matrix to the values in product starting from the
					 * lsb up to the gsb. This is done with a right bit shift and & 1. This is very
           * confusing, but it works.
					 */ 

					out_matrix[(field * row) + bit_idx][(field * col) + basis_idx] = (product >> bit_idx) & 1;
					count++;
				}
			}
		}
	}
	printf("count = %d, should be %d\n", count, (field * (k + n)) * (field * n));
}

/*
 *  Given in_matrix, drop the row indexes in row_drops to make a square matrix in 
 *  out_matrix. out_matrix should already be initialized as a square matrix.
 */

void create_sq_matrix(int **in_matrix, int **out_matrix, int *row_drops, int n) {
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			out_matrix[row][col] = in_matrix[row][col];
		}
	}
}

