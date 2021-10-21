#include <stdio.h>
#include <stdlib.h>
#include "processing.h"
#include "stack/include/Stack.h"
#include "../log/log.h"

static cpu_err get_cmd(cmd_t* dst_cmd, Binary* bin)
{
    cmd_t cmd = {};
    ASSERT(binary_swrite(&cmd, bin, sizeof(cmd_t)) == 0, CPU_READCMD_FAIL);

    *dst_cmd = cmd;
    
    return CPU_NOERR;
}

static cpu_err get_arg(arg_t* dst_arg, Binary* bin)
{
    arg_t arg = 0;

    ASSERT(binary_swrite(&arg, bin, sizeof(arg_t)) == 0, CPU_READARG_FAIL);

    *dst_arg = arg;
    
    return CPU_NOERR;
}

#define DEF_CMD(TEXT, n_args, CODE)    \
    case CMD_##TEXT:                   \
        CODE                           \

cpu_err processing(Binary* bin, FILE* istream, FILE* ostream)
{
    Stack stk = {};
    stack_init(&stk, 0);

    while(bin->ip != bin->sz)
    {
        cmd_t cmd = {};
        get_cmd(&cmd, bin);

        switch(cmd.bits.cmd)
        {
            #include "../def_cmd.inc"
            
            default:
            {
                ASSERT(0, CPU_UNKNWN_CMD);
            }
        }
    }
    
    stack_dstr(&stk);

    return CPU_NOERR;
}  

#undef DEF_CMD
