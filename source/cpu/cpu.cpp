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
#include "../log/log.h"

const char LOGFILE[] = "cpu_log.txt";

void processor_print_elem(FILE* ostream, const val64_t* elem)
{
    fprintf(ostream, "%lg", *elem);
}

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
    char binfile_name[MAX_FILENAME_SIZE] = "";
    char outfile_name[MAX_FILENAME_SIZE] = "";  /* not used */

    args_msg msg = process_args(argc, argv, binfile_name, outfile_name); 
    if(msg)
        response_args(msg);

    ssize_t binfile_sz = 0;
    ASSERT(file_sz(binfile_name, &binfile_sz) == 0, CPU_READ_FAIL);
    binfile_sz = binfile_sz / sizeof(bin_t);

    FILE* binstream = fopen(binfile_name, "rb");
    ASSERT(binstream, CPU_READ_FAIL);

    Binary bin = {};
    ASSERT(binary_init(&bin, binfile_sz) == 0, CPU_BIN_FAIL);

    ASSERT(binary_fread(&bin, binstream, binfile_sz) == 0, CPU_READ_FAIL);

    ASSERT(fclose(binstream) == 0, CPU_READ_FAIL);

    FILE* istream = stdin;
    FILE* ostream = stdout;

    stack_dump_set_print(&processor_print_elem);

    ASSERT(processing(&bin, istream, ostream) == 0, CPU_PROCESSING_FAIL);

    ASSERT(binary_dstr(&bin) == 0, CPU_BIN_FAIL);

    return 0;
}