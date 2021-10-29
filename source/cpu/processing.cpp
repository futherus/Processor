#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "processing.h"
#include "cpu_dump.h"
#include "stack/include/Stack.h"

static cpu_err get_cmd(cmd_t* dst_cmd, Binary* bin)
{
    cmd_t cmd = {};
    binary_swrite(&cmd, bin, sizeof(cmd_t));

    *dst_cmd = cmd;
    
    return CPU_NOERR;
}

// TODO: optimize to one switch
#define get_arg()                                                       \
    {                                                                   \
        switch(cmd.bits.reg)                                            \
        {                                                               \
            case ARG_REGISTER:                                          \
                binary_swrite(&cpu->reg_temp_8b, bin, sizeof(val8_t));  \
                cpu->reg_ptr = &cpu->regs[cpu->reg_temp_8b];            \
                break;                                                  \
                                                                        \
            case ARG_NOTREGISTER:                                       \
                binary_swrite(&cpu->reg_temp_64b, bin, sizeof(val64_t));\
                cpu->reg_ptr = &cpu->reg_temp_64b;                      \
                break;                                                  \
                                                                        \
            default:                                                    \
                assert(0);                                              \
        }                                                               \
        switch(cmd.bits.mem)                                            \
        {                                                               \
            case MEM_RAM:                                               \
                cpu->reg_ptr = &cpu->ram[(size_t) (*cpu->reg_ptr)];     \
                break;                                                  \
                                                                        \
            case MEM_NOT_RAM:                                           \
                break;                                                  \
                                                                        \
            default:                                                    \
                assert(0);                                              \
        }                                                               \
    }                                                                   \

#define DEF_SYSCMD(TEXT, hash, W_ARG, CODE)                     \
    case SYSCMD_##TEXT:                                         \
        if(W_ARG)                                               \
            get_arg();                                          \
        CODE                                                    \
        break;                                                  \

#define DEF_CMD(TEXT, hash, N_ARGS, CODE)                       \
    case CMD_##TEXT:                                            \
        CODE                                                    \
        break;                                                  \

#define dargs (cpu->regs + REG_dax)
#define dtemp (cpu->regs + REGS_CAP - 4)
#define wregs (cpu->regs)

cpu_err processing(Binary* bin, CPU* cpu, FILE* istream, FILE* ostream)
{
    assert(cpu && bin);

    while(bin->ip != bin->sz)
    {
        cmd_t cmd = {};
        get_cmd(&cmd, bin);

        switch(cmd.bits.cmd)
        {
            #include "../def_syscmd.inc"
            
            #include "../def_cmd.inc"
            
            default:
            {
                return CPU_UNKNWN_CMD;
            }
        }

        cpu_dump(cpu);
    }
        
    return CPU_NOERR;
}  

#undef DEF_CMD
