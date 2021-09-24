// largest table (multiplication and division) is 64 x 64
#define MAX_TABLE (64)
#define FIELD_3_DIMS (8)
#define FIELD_4_DIMS (16)
#define FIELD_5_DIMS (32)
#define FIELD_6_DIMS (64)

// determine which table to load in based on these
#define MUL_TYPE (0)
#define DIV_TYPE (1)

// length of file name string 
#define MAX_PATH_LEN (50)

// status codes
#define OK (0)
#define INVALID_FIELD (-1)
#define FILE_READ_ERR (-2)
#define INVALID_TYPE (-3)

int load_table(int field, char path[MAX_PATH_LEN], int table[MAX_TABLE][MAX_TABLE], int rows, int cols);
