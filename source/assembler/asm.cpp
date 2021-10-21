#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text/Text.h"
#include "../args/args.h"
#include "parser.h"
#include "../log/log.h"
#include "../binary/Binary.h"

const char LOGFILE[] = "asm_log.txt";

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

    //text_clean(txt, ';'); // function cleans all after delimiter ///How to add function to Text in order not to change Text.cpp/Text.h

    Binary bin = {};
    L$(ASSERT(binary_init(&bin, BIN_LINE_CAP * txt.index_arr_size) == 0, ASM_BIN_FAIL);)

    L$(ASSERT(parser(&bin, &txt) == 0, ASM_PARSE_FAIL);)

    FILE* ostream = fopen(outfile_name, "wb");
    L$(ASSERT(ostream, ASM_WRITE_FAIL);)
    
    L$(ASSERT(binary_fwrite(ostream, &bin, bin.sz) == 0, ASM_WRITE_FAIL);)

    L$(ASSERT(fclose(ostream) == 0, ASM_WRITE_FAIL);)
    
    L$(text_destroy(&txt);)
    L$(ASSERT(binary_dstr(&bin) == 0, ASM_BIN_FAIL);)

    return ASM_NOERR;
}