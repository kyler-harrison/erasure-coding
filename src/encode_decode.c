#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "encode_decode.h"
#include "encode_decode_matrix.h"

int main(int argc, char **argv) {
	int op_type;  // 0 means encode, 1 means decode
	char file_path[MAX_PATH_LEN];  // input/output file path, either the file to shard or where to write reconstruction
	char shard_base_path[MAX_PATH_LEN];  // path without shard index to read/write
	char arg_status_str[MAX_STATUS_LEN];  // will be filled if input args have error

	// check args and set variables
	int arg_status = handle_args(argc, argv, &op_type, file_path, shard_base_path, arg_status_str);

	if (arg_status != OK) {
		fprintf(stderr, "%s\n", arg_status_str);
		return arg_status;
	}

	if (op_type == 0) {
		encode();
	} else if (op_type == 1) {
		//decode();
	} else {
		fprintf(stderr, "how did that happen?\n");
		return BAD_ARGS;
	}

	return 0;
}

/*
 *  Checks for valid arguments and sets variables for whether to encode/decode.
 *  op_type is 0 for encode, 1 for decode. file_path is input/output path of file to shard/reconstruct.
 *  shard_base_path is the file path to read or write to without the shard index. 
 *  After encoding with a shard_base_path of "shard_path", each shard will be a file named 
 *  "shard_path0", "shard_path1", ..., "shard_pathN" If decoding, will read in 
 *  files using same naming convention.
 */

int handle_args(int argc, char **argv, int *op_type, char file_path[MAX_PATH_LEN], char shard_base_path[MAX_PATH_LEN], char arg_status_str[MAX_STATUS_LEN]) {
	if (argc != 4) {
		strncpy(arg_status_str, "Need 3 arguments corresponding to encode/decode integer, file path to write/read to/from, and base shard file path.\nExample: ./encode_decode 0 input_file file_shard", MAX_STATUS_LEN);
		arg_status_str[sizeof("Need 3 arguments corresponding to encode/decode integer, file path to write/read to/from, and base shard file path.\nExample: ./encode_decode 0 input_file file_shard") - 1] = '\0';
		return BAD_ARGS;
	}

	// check op_type
	int inp_op_type = atoi(argv[1]);

	switch (inp_op_type) {
		case 0:
			*op_type = inp_op_type;
			break;

		case 1:
			*op_type = inp_op_type;
			break;

		default:
			strncpy(arg_status_str, "The first argument (operation type) should be 0 to encode or 1 to decode.", MAX_STATUS_LEN);
			arg_status_str[sizeof("The first argument (operation type) should be 0 to encode or 1 to decode.") - 1] = '\0';
			return BAD_ARGS;
	}

	// copy paths (NOTE if length of inputted string > MAX_PATH_LEN, only the chars up to MAX_PATH_LEN - 1 are copied and no err outputted)

	strncpy(file_path, argv[2], MAX_PATH_LEN);
	file_path[sizeof(argv[2]) - 1] = '\0';

	strncpy(shard_base_path, argv[3], MAX_PATH_LEN);
	shard_base_path[sizeof(argv[3]) - 1] = '\0';

	return OK;
}

/*
 *  Shards the input_path file using the pre-computed encoder matrix (see gen_matrices.c).
 *  Each output shard is written to a file named output_base_path with its shard idx at end.
 *  Returns OK if successful, error else.
 */

int encode(char input_path[MAX_PATH_LEN], char output_base_path[MAX_PATH_LEN]) {
	/* PSEUDO
		 get input file size, compute how many bytes_per_row in output matrix

		 iterate through rows of ENCODER, iterate through bytes_per_row, iterate through ENCODER_COLS
		 while computing offset in file corresponding to output matrix offset

		 fseek to whatever offset (could this be simpler?), update resulting byte by xor between 
		 ENCODER[ENCODER_ROW] and the data byte at fseeked file position

		 once done looping through ENCODER_COLS, write final resulting byte
	*/

	return OK;
}
