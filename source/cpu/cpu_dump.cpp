
#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack/Stack.h"
#include "processing.h"
#include "cpu_dump.h"
#include "../common/dumpsystem.h"

static FILE* CPU_STREAM = nullptr;

static void cpu_dump_print_elem(FILE* ostream, const val64_t* elem)
{
    fprintf(ostream, "%lg", *elem);
}

void cpu_dump_init()
{
    CPU_STREAM = dumpsystem_get_stream(cpu_dump);

    stack_dump_init(CPU_STREAM, &cpu_dump_print_elem);
}

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__) 

void cpu_dump(CPU* cpu, size_t* ip)
{
    FILE* stream = CPU_STREAM;
    if(!stream)
        return;
        
    PRINT("\n----------------------------------------------------------------------------------------\n");
    PRINT("CPU dump\n");
    PRINT("INSTRUCTION POINTER: %lu\n", *ip);

    if(cpu->reg_ptr != nullptr)
        PRINT("  pointer  register: %lg [%p]\n", *cpu->reg_ptr, cpu->reg_ptr);
    else
        PRINT("  pointer  register: --//-- [%p]\n", cpu->reg_ptr);

    PRINT("  sys_8b  register: %hhu\n", cpu->reg_sys_8b);
    PRINT("  sys_64b register: %lg\n",  cpu->reg_sys_64b);

    stack_dump(&cpu->stk, "Stack\n");

    PRINT("  RAM\n");
    PRINT("  {");
    for(size_t iter = 0; iter < RAM_CAP; iter++)
    {
        if(iter % 8 == 0)
            PRINT("\n");
        PRINT("    %4lu: %10lg ", iter, cpu->ram[iter]);
    }
    PRINT("\n  }\n");

    PRINT("  VRAM\n");
    PRINT("  {");
    unsigned char* vram_ptr = (unsigned char*) (cpu->ram + RAM_CAP);

    for(size_t iter = 0; iter < VRAM_CAP; iter += sizeof(uint64_t))
    {
        if(iter % (8 * sizeof(uint64_t)) == 0)
            PRINT("\n");
        PRINT("    %4lu: %16llx ", iter, vram_ptr[iter]);
    }
    PRINT("\n  }\n");

    PRINT("  Registers\n");
    PRINT("  {");
    for(size_t iter = 0; iter < REGS_CAP; iter++)
    {
        if(iter % 4 == 0)
            PRINT("\n");
        PRINT("    %4lu: %10lg ", iter, cpu->regs[iter]);
    }
    PRINT("\n  }\n");
    PRINT("----------------------------------------------------------------------------------------\n\n");

    fflush(stream);
}

void cpu_dump_cmd(const char* cmd_txt, val64_t* args, unsigned char args_sz)
{
    FILE* stream = CPU_STREAM;
    if(!stream)
        return;

    PRINT("Command: %s (", cmd_txt);
    for(unsigned char arg_iter = 0; arg_iter < args_sz; arg_iter++)
    {
        PRINT("%lg", args[arg_iter]);
    }
    PRINT(")\n");

    fflush(stream);
}
