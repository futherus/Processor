#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text/Text.h"
#include "../args/args.h"
#include "parser.h"
#include "../binary/Binary.h"
#include "../jumps.h"

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
    char infile_name [FILENAME_MAX] = "";
    char outfile_name[FILENAME_MAX] = "";

    args_msg msg = process_args(argc, argv, infile_name, outfile_name);
    if(msg)
    {
        response_args(msg);
        return ASM_ARGS_FAIL;
    }

    Text txt = {};
    Binary bin = {};
    FILE* ostream = nullptr;

TRY__    
    CHECK__(text_create(&txt, infile_name), ASM_READ_FAIL);

    //text_clean(txt, ';'); // function cleans all after delimiter

    CHECK__(binary_init(&bin, BIN_LINE_CAP * txt.index_arr_size), ASM_BIN_FAIL);

    parser(&bin, &txt, infile_name);
    CHECK__(parser_errstruct()->errnum, ASM_PARSE_FAIL);

    ostream = fopen(outfile_name, "wb");
    CHECK__(ostream == nullptr, ASM_WRITE_FAIL);
    
    CHECK__(binary_fwrite(ostream, &bin, bin.sz), ASM_WRITE_FAIL);

CATCH__
    printf("Error in asm.cpp");
FINALLY__
    printf("No error in asm.cpp");
    fclose(ostream);
    text_destroy(&txt);
    binary_dstr(&bin);

    return ERROR__;

ENDTRY__
}