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
 *  [] invert cauchy matrix
 *  [] expand matrix (used to create final encoder/decoder)
 *  [] write encode_decode.c containing arrays of encoder/decoder
 *  [] write encode_decode.h
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

	// FREEDOM
	free(x);
	free(y);
	for (int i = 0; i < (k + n); i++) {
		free(cauchy[i]);
	}

	return 0;
}

/*
 *  Processes command line arguments. Does checks, decides on field. 
 *  For now, args are all required, should be:
 *  ./gen_matrices [k] [n] [overwrite_bool]
 *  TODO should include optional decision for drop rows?
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
		strncpy(arg_status, "The inputted values of k and n require a finite field larger than 2**64, try smaller values.", MAX_STATUS_LEN);
		arg_status[sizeof("The inputted values of k and n require a finite field larger than 2**64, try smaller values.") - 1] = '\0';
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

