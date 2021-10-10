#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "encode_decode.h"
#include "encode_decode_matrix.h"

int main(int argc, char **argv) {
	int op_type;  // 0 means encode, 1 means decode
	char base_path[MAX_PATH_LEN];  // path without shard index to read/write
	char arg_status_str[MAX_STATUS_LEN];  // will be filled if input args have error

	// check args and set variables
	int arg_status = handle_args(argc, argv, &op_type, base_path, arg_status_str);

	if (arg_status != OK) {
		fprintf(stderr, "%s\n", arg_status_str);
		return arg_status;
	}

	if (op_type == 0) {
		//encode();
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
 *  op_type is 0 for encode, 1 for decode. base_path is the file path to read or
 *  write to without the shard index. After encoding with a base_path of "shard_path", 
 *  each shard will be a file named "shard_path0", "shard_path1", ..., "shard_pathN"
 *  If decoding, will read in files using same naming convention.
 */

int handle_args(int argc, char **argv, int *op_type, char base_path[MAX_PATH_LEN], char arg_status_str[MAX_STATUS_LEN]) {
	if (argc != 3) {
		strncpy(arg_status_str, "Need 2 arguments corresponding to encode/decode and base file name.\nExample: ./encode_decode 0 file_shard", MAX_STATUS_LEN);
		arg_status_str[sizeof("Need 2 arguments corresponding to encode/decode and base file name.\nExample: ./encode_decode 0 file_shard") - 1] = '\0';
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

	// copy inputted base path (NOTE if length of base_path > MAX_PATH_LEN, only the chars up to MAX_PATH_LEN - 1 are copied)
	strncpy(base_path, argv[2], MAX_PATH_LEN);
	base_path[sizeof(argv[2]) - 1] = '\0';

	return OK;
}
