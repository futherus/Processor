#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "processing.h"
#include "stack/include/Stack.h"
#include "../binary/Binary.h"
#include "../args/args.h"
#include "cpu_dump.h"

#define A$(condition, err)                      \
    if(!(condition))                            \
    {                                           \
        fprintf(stderr, "Error: %s", #err);     \
        return (err);                           \
    }                                           \

static int file_sz(const char filename[], ssize_t* sz)
{
    struct stat buff = {};
    if(stat(filename, &buff) == -1)
        return -1;
    
    *sz = buff.st_size;
    
    return 0;
}

int main(int argc, char* argv[])
{
    char binfile_name[FILENAME_MAX] = "";

    args_msg msg = process_args(argc, argv, binfile_name);
    if(msg)
    {
        response_args(msg);
        return msg;
    }

    cpu_dump_init();
    
    ssize_t binfile_sz = 0;
    A$(file_sz(binfile_name, &binfile_sz) == 0, CPU_READ_FAIL);

    FILE* binstream = fopen(binfile_name, "rb");
    A$(binstream, CPU_READ_FAIL);

    CPU cpu = {};
    stack_init(&cpu.stk, 0);

    Binary bin = {};

    A$(binary_init(&bin, binfile_sz) == 0, CPU_BIN_FAIL);

    A$(binary_fread(&bin, binstream, binfile_sz) == 0, CPU_READ_FAIL);

    A$(fclose(binstream) == 0, CPU_READ_FAIL);

    FILE* istream = stdin;
    FILE* ostream = stdout;

    A$(processing(&bin, &cpu, istream, ostream) == 0, CPU_PROCESSING_FAIL);

    stack_dstr(&cpu.stk);
    A$(binary_dstr(&bin) == 0, CPU_BIN_FAIL);

    return 0;
}