
#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stack/include/Stack.h"
#include "processing.h"
#include "cpu_dump.h"
#include "../dumpsystem/dumpsystem.h"

static FILE* CPU_STREAM = nullptr;

void cpu_dump_print_elem(FILE* ostream, const val64_t* elem)
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
    PRINT("INSTRUCTION POINTER: %llu\n", *ip);

    if(cpu->reg_ptr != nullptr)
        PRINT("  pointer  register: %lg [%p]\n", *cpu->reg_ptr, cpu->reg_ptr);
    else
        PRINT("  pointer  register: --||-- [%p]\n", cpu->reg_ptr);

    PRINT("  temp_8b  register: %hhu\n", cpu->reg_temp_8b);
    PRINT("  temp_64b register: %lg\n", cpu->reg_temp_64b);

    stack_dump(&cpu->stk, "Stack\n");

    PRINT("  RAM\n");
    PRINT("  {");
    for(size_t iter = 0; iter < RAM_CAP; iter++)
    {
        if(iter % 8 == 0)
            PRINT("\n");
        PRINT("    %4llu: %10lg ", iter, cpu->ram[iter]);
    }
    PRINT("\n  }\n");

    PRINT("  {");
    for(size_t iter = RAM_CAP; iter < MEM_CAP; iter++)
    {
        if(iter % 8 == 0)
            PRINT("\n");
        PRINT("    %4llu: %16llx ", (iter - RAM_CAP) * sizeof(val64_t), cpu->ram[iter]);
    }
    PRINT("\n  }\n");

    PRINT("  Registers\n");
    PRINT("  {");
    for(size_t iter = 0; iter < REGS_CAP; iter++)
    {
        if(iter % 4 == 0)
            PRINT("\n");
        PRINT("    %4llu: %10lg ", iter, cpu->regs[iter]);
    }
    PRINT("\n  }\n");
    PRINT("----------------------------------------------------------------------------------------\n\n");

    fflush(stream);
}
