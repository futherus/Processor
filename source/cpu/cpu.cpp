#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#include "processing.h"
#include "stack/include/Stack.h"
#include "../binary/Binary.h"
#include "../args/args.h"
#include "cpu_dump.h"
#include "../cpu_time.h"
#include "../dumpsystem/dumpsystem.h"
#include "../jumps.h"

enum cpu_error
{
    CPU_NOERR          = 0,
    CPU_ARGS_ERR       = 1,
    CPU_PROCESSING_ERR = 2,
    CPU_SYS_FAIL       = 3,
    CPU_READ_FAIL      = 4,
    CPU_STACK_FAIL     = 5,
    CPU_BAD_ALLOC      = 6,
};

static int file_sz(const char filename[], ssize_t* sz)
{
    assert(filename && sz);

    struct stat buff = {};
    if(stat(filename, &buff) == -1)
        return -1;
    
    *sz = buff.st_size;
    
    return 0;
}

static cpu_error cpu_init(CPU* cpu)
{
    assert(cpu);

    cpu->ram = (val64_t*) calloc(MEM_CAP, sizeof(val64_t));
    CHECK$(!cpu->ram, CPU_BAD_ALLOC, return CPU_BAD_ALLOC; )

    return CPU_NOERR;
}

int main(int argc, char* argv[])
{
    char binfile_name[FILENAME_MAX] = "";

    args_msg msg = process_args(argc, argv, binfile_name);
    if(msg)
    {
        response_args(msg);
        return CPU_ARGS_ERR;
    }

    cpu_dump_init();

    Binary bin = {};
    ssize_t binfile_sz = 0;

    CPU cpu = {};

    FILE* binstream = nullptr;
    FILE* istream   = stdin;
    FILE* ostream   = stdout;
    
    double start_time  = 0;
    double finish_time = 0;

TRY__
    CHECK$(file_sz(binfile_name, &binfile_sz), CPU_READ_FAIL, FAIL__)

    binstream = fopen(binfile_name, "rb");
    CHECK$(binstream == nullptr,         CPU_READ_FAIL,  FAIL__)

    CHECK$(stack_init(&cpu.stk, 0),      CPU_STACK_FAIL, FAIL__)

    PASS$(binary_init(&bin, binfile_sz),                 FAIL__)

    PASS$(binary_fread(&bin, binstream, binfile_sz),     FAIL__)

    CHECK$(fclose(binstream),            CPU_READ_FAIL,  FAIL__)

    CHECK$(cpu_init(&cpu),               CPU_BAD_ALLOC,  FAIL__)

    start_time = get_cpu_time();

    CHECK$(processing(&bin, &cpu, istream, ostream), CPU_PROCESSING_ERR, FAIL__)

    finish_time = get_cpu_time();

    LOG$("Execution time: %.4lg s", finish_time - start_time)

CATCH__
    ERROR__ = CPU_SYS_FAIL;

FINALLY__
    stack_dstr(&cpu.stk);
    binary_dstr(&bin);

    return ERROR__;

ENDTRY__
}
