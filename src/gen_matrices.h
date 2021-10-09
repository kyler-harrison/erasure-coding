// largest table (multiplication and division) is 64 x 64
#define MAX_TABLE (64)
#define FIELD_3_DIMS (8)
#define FIELD_4_DIMS (16)
#define FIELD_5_DIMS (32)
#define FIELD_6_DIMS (64)

#define MAX_PATH_LEN (50)  // length of file name string 
#define MAX_STATUS_LEN (200)  // length of status message to output

// status codes
#define OK (0)
#define INVALID_FIELD (-1)
#define FILE_READ_ERR (-2)
#define BAD_ARGS (-3)

// function prototypes
int load_table(int field, char path[MAX_PATH_LEN], int table[MAX_TABLE][MAX_TABLE], int rows, int cols);
int handle_args(int argc, char **argv, char arg_status[MAX_STATUS_LEN], int *k, int *n, int *overwrite, int *field);
void gen_x_y(int k, int n, int *x, int *y);
void gen_cauchy(int **cauchy, int *x, int *y, int k, int n, int div_table[MAX_TABLE][MAX_TABLE]);
void expand_matrix(int **in_matrix, int **out_matrix, int field, int k, int n, int mul_table[MAX_TABLE][MAX_TABLE]);
void create_sq_matrix(int **in_matrix, int **out_matrix, int *drop_rows, int n);
void invert_cauchy(int **sq_cauchy, int **inv_cauchy, int row_col_dim, int *x_rm, int *y, int mul_table[MAX_TABLE][MAX_TABLE], int div_table[MAX_TABLE][MAX_TABLE]);
