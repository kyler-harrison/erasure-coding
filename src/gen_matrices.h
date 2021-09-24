// largest table (multiplication and division) is 64 x 64
#define MAX_TABLE (64)

// length of file name string 
#define MAX_PATH_LEN (50)

// status codes
#define OK (0)
#define INVALID_FIELD (-1)
#define FILE_READ_ERR (-2)

int load_table(int, int, int [MAX_TABLE][MAX_TABLE]);
