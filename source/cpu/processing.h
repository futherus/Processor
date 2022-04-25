#ifndef CPU_H
#define CPU_H

#include "../processor_config.h"
#include "../common/Binary.h"
#include "stack/Stack.h"

struct CPU 
{
    val64_t* ram = {0};

    Stack stk = {};

    val64_t regs[REGS_CAP]  = {0};

    val64_t*  reg_ptr      = 0; ///< register with pointer to argument (one-literal)
    val8_t    reg_sys_8b   = 0; ///< register with 8  bits argument (one-literal, register)
    val64_t   reg_sys_64b  = 0; ///< register with 64 bits argument (one-literal, immconst)
    size_t    reg_sz       = 0; ///< register with size of stack (relative to last 'call')

    long long reg_int      = 0;

};

enum processing_err
{
    PROCESSING_NOERR = 0,
    PROCESSING_READ_FAIL = 1,
    PROCESSING_BIN_FAIL = 2,
    PROCESSING_UNKNWN_CMD = 3,
    PROCESSING_READCMD_FAIL = 4,
    PROCESSING_READARG_FAIL = 5,
    PROCESSING_PROCESSING_FAIL = 6,
    PROCESSING_RAM_OVERFLOW = 7,
};

processing_err processing(Binary* bin, CPU* cpu, FILE* istream, FILE* ostream);

#endif // CPU_H
