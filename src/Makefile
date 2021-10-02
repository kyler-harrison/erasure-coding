.PHONY: all
all: gen_matrices
	@echo Make complete.

gen_matrices: gen_matrices.c ../bin/
	gcc -Wall -Werror -std=c99 gen_matrices.c -o ../bin/gen_matrices

