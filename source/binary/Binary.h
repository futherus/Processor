#ifndef BINARY_H
#define BINARY_H

#include <stdint.h>
#include <stdio.h>

enum cmd_num
{
    CMD_IN   = 1,
    CMD_OUT  = 2,
    CMD_MUL  = 3,
    CMD_ADD  = 4,
    CMD_HAL  = 5,
    CMD_PUSH = 6,
};

/// Command type
union cmd_t 
{
    struct
    {
        unsigned char cmd:    5;
        unsigned char n_args: 3;
    } bits;
    
    unsigned char byte;
};

/// Arguments type
typedef double arg_t;

typedef unsigned char bin_t;

/// Max amount of args
static const size_t ARGS_CAP = 0x8;

/// Capacity of binary line buffer
const size_t BIN_LINE_CAP = 0x100;

enum Binary_err
{
    BIN_NOERR       = 0,
    BIN_SIZE_ERR    = 1,
    BIN_BAD_ALLOC   = 2,
    BIN_STREAM_FAIL = 3,
    BIN_NULL_BUF    = 4,
    BIN_NULLPTR     = 5,
};

struct Binary
{
    bin_t* buffer = nullptr;   ///< Pointer to beginning of allocated memory
    size_t ip     = 0;         ///< Instruction pointer      (position of unread element in buffer)
    size_t sz     = 0;         ///< Size of used memory      (in sizeof(bin_t*))
    size_t cap    = 0;         ///< Size of allocated memory (in sizeof(bin_t*))
};

Binary_err binary_init(Binary* bin, ssize_t cap);

Binary_err binary_sread(Binary* bin, void* src, size_t count);

Binary_err binary_fread(Binary* bin, FILE* istream, size_t count);

Binary_err binary_swrite(void* dst, Binary* bin, size_t count);

Binary_err binary_fwrite(FILE* ostream, Binary* bin, size_t count);

Binary_err binary_dstr(Binary* bin);

#endif // BINARY_H