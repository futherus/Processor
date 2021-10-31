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

    val64_t* reg_ptr      = 0; ///< register with pointer to argument (one-literal)
    val8_t   reg_temp_8b  = 0; ///< register with 8  bits argument (one-literal, register)
    val64_t  reg_temp_64b = 0; ///< register with 64 bits argument (one-literal, immconst)
    size_t   reg_sz       = 0; ///< register with size of stack (relative to last 'call')
};

enum cpu_err
{
    CPU_NOERR = 0,
    CPU_READ_FAIL,
    CPU_BIN_FAIL,
    CPU_UNKNWN_CMD,
    CPU_READCMD_FAIL,
    CPU_READARG_FAIL,
    CPU_PROCESSING_FAIL,
};

cpu_err processing(Binary* bin, CPU* cpu, FILE* istream, FILE* ostream);

#endif // CPU_H
