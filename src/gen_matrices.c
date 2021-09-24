#include <stdio.h>
#include <string.h>

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
	int mul_table[MAX_TABLE][MAX_TABLE];
	int field = 4;  // TODO set with cl args
	int type = 0;  // TODO define as multiplication table constant type somewhere

	// file path determined by field number below
	char path[MAX_PATH_LEN] = "";

	// these will be set to a constant in the header file based on the field number below
	int num_rows;
	int num_cols;

	// determine path based on field
	switch (field) {
		case 6:
			if (type == MUL_TYPE) {
				strncpy(path, "../tables/multiplication/2to6_1000011.txt", MAX_PATH_LEN);
			} else if (type == DIV_TYPE) {
				strncpy(path, "../tables/division/2to6_1000011.txt", MAX_PATH_LEN);
			} else {
				return INVALID_TYPE;
			}

			num_rows = FIELD_6_DIMS;
			num_cols = FIELD_6_DIMS;

			break;

		case 5:
			if (type == MUL_TYPE) {
				strncpy(path, "../tables/multiplication/2to5_100101.txt", MAX_PATH_LEN);
			} else if (type == DIV_TYPE) {
				strncpy(path, "../tables/division/2to5_100101.txt", MAX_PATH_LEN);
			} else {
				return INVALID_TYPE;
			}

			num_rows = FIELD_5_DIMS;
			num_cols = FIELD_5_DIMS;

			break;

		case 4:
			if (type == MUL_TYPE) {
				strncpy(path, "../tables/multiplication/2to4_10011.txt", MAX_PATH_LEN);
			} else if (type == DIV_TYPE) {
				strncpy(path, "../tables/division/2to4_10011.txt", MAX_PATH_LEN);
			} else {
				return INVALID_TYPE;
			}

			num_rows = FIELD_4_DIMS;
			num_cols = FIELD_4_DIMS;

			break;

		case 3:
			if (type == MUL_TYPE) {
				strncpy(path, "../tables/multiplication/2to3_1011.txt", MAX_PATH_LEN);
			} else if (type == DIV_TYPE) {
				strncpy(path, "../tables/division/2to3_1011.txt", MAX_PATH_LEN);
			} else {
				return INVALID_TYPE;
			}

			num_rows = FIELD_3_DIMS;
			num_cols = FIELD_3_DIMS;

			break;

		default:
			return INVALID_FIELD;
	}

	// get multiplication/division table
	load_table(field, path, mul_table, num_rows, num_cols);

	return 0;
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

