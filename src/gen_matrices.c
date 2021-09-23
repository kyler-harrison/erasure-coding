#include <stdio.h>
#include <string.h>

#include "gen_matrices.h"

/*  TODO: 
 *  process cl args, do checks on k, n, num row drops, and whatever else
 *  load mul/div tables into arrays (probs need to #define possible dimensions 
    beforehand and init based on cl args unless there's a better way, i guess 
		could just allocate max table size and then smaller ones only use what they need)
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

	load_table(field, mul_table);

	return 0;
}

/*
 *  Loads in a finite field arithmetic table from one of the text files in 
 *  the tables directory.
 */

int load_table(int field, int table[MAX_TABLE][MAX_TABLE]) {
	char path[MAX_PATH_LEN] = "";

	switch (field) {
		case 6:
			strncpy(path, "tables/2to6_1000011.txt", MAX_PATH_LEN);
			break;
		case 5:
			strncpy(path, "tables/2to5_100101.txt", MAX_PATH_LEN);
			break;
		case 4:
			strncpy(path, "tables/2to4_10011.txt", MAX_PATH_LEN);
			break;
		case 3:
			strncpy(path, "tables/2to3_1011.txt", MAX_PATH_LEN);
			break;
		default:
			return INVALID_FIELD;
	}

	printf("%s\n", path);

	return OK;
}

