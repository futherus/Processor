#ifndef CPU_H
#define CPU_H

#include "../processor_config.h"
#include "../binary/Binary.h"
#include "stack/include/Stack.h"

struct CPU 
{
    val64_t ram[MEM_CAP] = {0};

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
    PROCESSING_READ_FAIL,
    PROCESSING_BIN_FAIL,
    PROCESSING_UNKNWN_CMD,
    PROCESSING_READCMD_FAIL,
    PROCESSING_READARG_FAIL,
    PROCESSING_PROCESSING_FAIL,
};

processing_err processing(Binary* bin, CPU* cpu, FILE* istream, FILE* ostream);

#endif // CPU_H
