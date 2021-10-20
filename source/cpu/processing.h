#ifndef CPU_H
#define CPU_H

#include "../binary/Binary.h"

enum cpu_err
{
    CPU_NOERR = 0,
    CPU_READ_FAIL,
    CPU_BIN_FAIL,
    CPU_UNKNWN_CMD,
    CPU_READCMD_FAIL,
    CPU_READARG_FAIL,
};

cpu_err processing(Binary* bin, FILE* istream, FILE* ostream);

#endif // CPU_H