#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "processing.h"
#include "cpu_dump.h"
#include "stack/include/Stack.h"

static int CMP(double var1, double var2)
{
    if(var1 - var2 > EPSILON)
        return 1;
    else if(var1 - var2 < -EPSILON)
        return -1;
    else
        return 0;
}

static processing_err get_cmd(cmd_t* dst_cmd, Binary* bin)
{
    cmd_t cmd = {};
    binary_swrite(&cmd, bin, sizeof(cmd_t));

    *dst_cmd = cmd;
    
    return PROCESSING_NOERR;
}

/////////////////////////////////////////////////////////////////////////
// TODO: optimize to one switch
#define get_arg()                                                       \
    {                                                                   \
        switch(cmd.bits.reg)                                            \
        {                                                               \
            case ARG_REGISTER:                                          \
                binary_swrite(&cpu->reg_sys_8b, bin, sizeof(val8_t));   \
                cpu->reg_ptr = &cpu->regs[cpu->reg_sys_8b];             \
                break;                                                  \
                                                                        \
            case ARG_NOTREGISTER:                                       \
                binary_swrite(&cpu->reg_sys_64b, bin, sizeof(val64_t)); \
                cpu->reg_ptr = &cpu->reg_sys_64b;                       \
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

/////////////////////////////////////////////////////////////////////////
#ifdef CPU_DUMP

    #define DEF_SYSCMD(TEXT, hash, W_ARG, CODE)                     \
        case SYSCMD_##TEXT:                                         \
            if(W_ARG)                                               \
                get_arg();                                          \
            cpu_dump_cmd(#TEXT, cpu->reg_ptr, 1);                   \
            CODE                                                    \
            break;                                                  \

    #define DEF_CMD(TEXT, hash, N_ARGS, CODE)                       \
        case CMD_##TEXT:                                            \
            cpu_dump_cmd(#TEXT, cpu->regs + REG_dax, N_ARGS);       \
            CODE                                                    \
            break;                                                  \

#else

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

#endif // CPU_DUMP //////////////////////////////////////////////////////

#define DARGS    (cpu->regs + REG_dax)
#define DTMPS    (cpu->regs + REG_tax)
#define REG_PTR  (cpu->reg_ptr)
#define REG_SZ   (cpu->reg_sz)
#define REG_INT  (cpu->reg_int)
#define IP       (bin->ip)

#define COPY(DST, SRC)     memcpy((DST), (SRC), sizeof(*(SRC)))
#define PRINT(format, ...) fprintf(ostream, (format), ##__VA_ARGS__)
#define SCAN(format, ...)  fscanf(istream,  (format), ##__VA_ARGS__)

#ifdef CPU_VERBOSE
    #define VERBOSE(format, ...) fprintf(ostream, (format), __VA_ARGS__)
#else 
    #define VERBOSE(format, ...) {void(0);}
#endif // CPU_VERBOSE

#define RAM  (cpu->ram)
#define VRAM ((unsigned char*) (cpu->ram + RAM_CAP))

#define PUSH(ARG) stack_push(&cpu->stk, (ARG))
#define POP(ARG)  stack_pop(&cpu->stk,  (ARG))

processing_err processing(Binary* bin, CPU* cpu, FILE* istream, FILE* ostream)
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
                return PROCESSING_UNKNWN_CMD;
            }
        }

#ifdef CPU_DUMP
        cpu_dump(cpu, &bin->ip);
#endif
    }
        
    return PROCESSING_NOERR;
}  

#undef DEF_CMD
