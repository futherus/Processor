#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text/Text.h"
#include "../args/args.h"
#include "preprocessor.h"
#include "preprocessor_list.h"
#include "../log/log.h"
#include "../binary/Binary.h"

const char LOGFILE[] = "asm_logs/asm_log.txt";

enum asm_err
{
    ASM_NOERR      = 0,
    ASM_ARGS_FAIL  = 1,
    ASM_READ_FAIL  = 2,
    ASM_PARSE_FAIL = 3,
    ASM_WRITE_FAIL = 4,
    ASM_BIN_FAIL   = 5,
};

int main(int argc, char* argv[])
{
    char infile_name [MAX_FILENAME_SIZE] = "";
    char outfile_name[MAX_FILENAME_SIZE] = "";

    args_msg msg = process_args(argc, argv, infile_name, outfile_name);
    if(msg)
    {
        response_args(msg);
        return ASM_ARGS_FAIL;
    }

    Text txt = {};
    L$(ASSERT(text_create(&txt, infile_name) == 0, ASM_READ_FAIL);)

    //text_clean(txt, ';'); // function cleans all after delimiter

    Binary bin = {};
    ASSERT(binary_init(&bin, BIN_LINE_CAP * txt.index_arr_size) == 0, ASM_BIN_FAIL);

    if(preprocessor(&bin, &txt) != 0)
        log_err(infile_name);

    FILE* ostream = fopen(outfile_name, "wb");
    ASSERT(ostream, ASM_WRITE_FAIL);
    
    ASSERT(binary_fwrite(ostream, &bin, bin.sz) == 0, ASM_WRITE_FAIL);

    ASSERT(fclose(ostream) == 0, ASM_WRITE_FAIL);
    
    text_destroy(&txt);
    L$(ASSERT(binary_dstr(&bin) == 0, ASM_BIN_FAIL);)

    return ASM_NOERR;
}