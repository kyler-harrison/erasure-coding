#include <stdio.h>
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

	return 0;
}

void load_tables(int field, int mul_table[MAX_TABLE][MAX_TABLE], int div_table[MAX_TABLE][MAX_TABLE]) {
}

