#ifndef BINARY_H
#define BINARY_H

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>

#include <stdint.h>
#include "../hash.h"

//////////////////////////////////////////////
#define DEF_SYSCMD(TEXT, hash, n_args, code) \
    SYSCMD_##TEXT,                           \

#define DEF_CMD(TEXT, hash, n_args, code)    \
    CMD_##TEXT,                              \

enum cmd_bin_code
{
    #include "../def_syscmd.inc"
    
    #include "../def_cmd.inc"
};

#undef DEF_CMD
#undef DEF_SYSCMD
/////////////////////////////////////////////
#define DEF_REG(TXT, hash)                  \
    REG_##TXT,                              \

enum reg_bin_code
{
    #include "../def_reg.inc"
};

#undef DEF_REG
/////////////////////////////////////////////

/// bin types
typedef double        val64_t;
typedef unsigned char val8_t;

/// specifier type
typedef int           spec_t;

/// Binary type
typedef unsigned char bin_t;

/// command type
union cmd_t 
{
    struct
    {
        unsigned char cmd: 6;
        unsigned char reg: 1;
        unsigned char mem: 1;
    } bits;
    
    unsigned char byte;
};

const spec_t ARG_ERRTYPE  = -1;
const spec_t ARG_IMMCONST =  0;
const spec_t ARG_REGISTER =  1;

const spec_t MEM_NOT_RAM =  0;
const spec_t MEM_RAM     =  1;

/// Capacity of binary line buffer
const size_t BIN_LINE_CAP = 0x200;

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