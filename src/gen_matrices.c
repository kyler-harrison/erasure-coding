#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gen_matrices.h"

/*  TODO: 
 *  process cl args, do checks on k, n, num row drops, and whatever else
 *  load mul/div tables into arrays (probs need to #define possible dimensions 
 *  beforehand and init based on cl args unless there's a better way, i guess 
 *	could just allocate max table size and then smaller ones only use what they need)
 *  generate x, y sets for cauchy
 *  generate cauchy matrix
 *  invert cauchy matrix
 *  expand matrix (used to create final encoder/decoder)
 *  write encode_decode.c containing arrays of encoder/decoder
 *  write encode_decode.h
 */

int main(int argc, char **argv) {
	char arg_status_str[MAX_STATUS_LEN] = "";
	int field;
	int arg_status = handle_args(argc, argv, arg_status_str, &field);

	if (arg_status != OK) {
		fprintf(stderr, "%s\n", arg_status_str);  // TODO make sure this works
		return arg_status;
	}

	// 2d arrays of finite arithmetic tables
	int mul_table[MAX_TABLE][MAX_TABLE];
	int div_table[MAX_TABLE][MAX_TABLE];

	// file path determined by field number below
	char mul_path[MAX_PATH_LEN] = "";
	char div_path[MAX_PATH_LEN] = "";

	// these will be set to a constant in the header file based on the field number below
	int num_rows;
	int num_cols;

	// determine path based on field 
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

	// TODO rm - test print 
	/*
	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			printf("%d ", mul_table[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			printf("%d ", div_table[i][j]);
		}
		printf("\n");
	}
	*/

	return 0;
}

/*
 *  Processes command line arguments. Does checks, decides on field. 
 *  For now, args are all required, should be:
 *  ./gen_matrices [k] [n] [overwrite_bool]
 *  TODO should include optional decision for drop rows?
 */

int handle_args(int argc, char **argv, char arg_status[MAX_STATUS_LEN], int *field) {
	if (argc != 4) {
		strncpy(arg_status, "Need 3 arguments corresponding to k, n, and overwrite boolean.\nExample: ./gen_matrices 4 2 1", MAX_STATUS_LEN);
		arg_status[sizeof("Need 3 arguments corresponding to k, n, and overwrite boolean.\nExample: ./gen_matrices 4 2 1") - 1] = '\0';
		return BAD_ARGS;
	}

	int k = atoi(argv[1]);
	int n = atoi(argv[2]);
	int overwrite = atoi(argv[3]);

	if ((k < 0) || (n < 0)) {
		strncpy(arg_status, "Ensure that the k and n arguments are greater than 0.", MAX_STATUS_LEN);
		arg_status[sizeof("Ensure that the k and n arguments are greater than 0.") - 1] = '\0';
		return BAD_ARGS;
	}
	
	return OK;
}

/*
 *  Loads in a finite field arithmetic table from one of the text files in 
 *  the tables directory.
 */

int load_table(int field, char path[MAX_PATH_LEN], int table[MAX_TABLE][MAX_TABLE], int num_rows, int num_cols) {

	FILE *file_ptr = fopen(path, "r");

	if (file_ptr == NULL) {
		return FILE_READ_ERR;
	}

	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			fscanf(file_ptr, "%d", &table[i][j]);
		}
	}

	fclose(file_ptr);

	return OK;
}

