#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Text.h"
#include "parser.h"
#include "../common/jumps.h"
#include "../common/args.h"
#include "../common/Binary.h"
#include "../common/dumpsystem.h"

enum asm_error
{
    ASM_NOERR       = 0,
    ASM_ARGS_ERR    = 1,
    ASM_SYNTAX_ERR  = 2,
    ASM_SYS_FAIL    = 3,
    ASM_READ_FAIL   = 4,
    ASM_WRITE_FAIL  = 5,
};

int main(int argc, char* argv[])
{
    char infile_name [FILENAME_MAX] = "";
    char outfile_name[FILENAME_MAX] = "";

    args_msg msg = process_args(argc, argv, infile_name, outfile_name);
    if(msg)
    {
        response_args(msg);
        return ASM_ARGS_ERR;
    }

    Text txt = {};
    Binary bin = {};
    FILE* ostream = nullptr;

TRY__    
    CHECK$(text_create(&txt, infile_name), ASM_SYS_FAIL, FAIL__)

    PASS$(binary_init(&bin, BIN_LINE_CAP * txt.index_arr_size), FAIL__)

    if(parser(&bin, &txt, infile_name))
    {
        ERROR__ = ASM_SYNTAX_ERR;
        RETURN__;
    }

    ostream = fopen(outfile_name, "wb");
    CHECK$(ostream == nullptr, ASM_SYS_FAIL, FAIL__)
    
    PASS$(binary_fwrite(ostream, &bin, bin.sz), FAIL__)

CATCH__
    ERROR__ = ASM_SYS_FAIL;

FINALLY__
    fclose(ostream);
    text_destroy(&txt);
    binary_dstr(&bin);

    return ERROR__;

ENDTRY__
}
