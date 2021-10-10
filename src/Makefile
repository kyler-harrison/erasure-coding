.PHONY: all
all: gen_matrices encode_decode
	@echo Make complete.

gen_matrices: gen_matrices.c ../bin/
	gcc -Wall -Werror -std=c99 gen_matrices.c -o ../bin/gen_matrices -lm

encode_decode: encode_decode.c ../bin/
	gcc -Wall -Werror -std=c99 encode_decode.c -o ../bin/encode_decode


