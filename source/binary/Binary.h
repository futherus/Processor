#ifndef BINARY_H
#define BINARY_H

#include <stdint.h>
#include <stdio.h>

/// Binary code type
typedef double bin_t;

/// Max amount of args
static const size_t ARGS_CAP = 0x8;

enum asm_cmd
{
    NOCMD = 0,
    in    = 1,
    out   = 2, 
    add   = 3,
    mul   = 4,
    push  = 5,
    hal      ,
    BAD_CMD,
};

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

Binary_err binary_sread(Binary* bin, bin_t* src, size_t count);

Binary_err binary_fread(Binary* bin, FILE* istream, size_t count);

Binary_err binary_swrite(bin_t* dst, Binary* bin, size_t count);

Binary_err binary_fwrite(FILE* ostream, Binary* bin, size_t count);

Binary_err binary_dstr(Binary* bin);

#endif // BINARY_H