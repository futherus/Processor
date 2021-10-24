#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "preprocessor_list.h"

static FILE* PREPROCESSOR_STREAM = nullptr;
static FILE* BINLINE_STREAM      = nullptr;

static void close_liststream_()
{
    if(fclose(PREPROCESSOR_STREAM) != 0 || fclose(BINLINE_STREAM) != 0)
        fprintf(stderr, "Files for listing cannot be closed %s", strerror(errno));
}

static int list_stream_()
{
    static int first_call = 1;

    if(first_call)
    {
        first_call = 0;

        BINLINE_STREAM = fopen(BINLINE_LISTFILE, "w");
        PREPROCESSOR_STREAM = fopen(PREPROCESSOR_LISTFILE, "w");

        if(BINLINE_STREAM && PREPROCESSOR_STREAM)
            atexit(&close_liststream_);
        else
        {
            fprintf(stderr, "Cannot open file for listing %s", strerror(errno));
            return -1;
        }
    }

    return 0;
}

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__) 

//////////////////////// Line listing ////////////////////////
void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t line)
{
    list_stream_();
    static int first_call = 1;
    FILE* stream = BINLINE_STREAM;
    if(!stream)
        return;

    if(first_call)
    {
        PRINT("|Line||Command----------------|");
        for(int addr = 0; addr < 50; addr++)
        {
            if(addr % 8 == 0)
                PRINT("|");
            
            PRINT("%2d ", addr);
        }
        
        PRINT("\n");
        first_call = 0;
    }

    PRINT("|%4llu|", line);
    PRINT("|%-23s|", txt_line);
    for(size_t iter = 0; iter < bin_line_sz; iter++)
    {
        if(iter % 8 == 0)
            PRINT("|");

        PRINT("%02hx ", bin_line[iter]);
    }
    PRINT("\n");

    fflush(stream);
}

//////////////////////// Expression listing ////////////////////////
static void list_cmd(Command* cmd, FILE* stream)
{
    PRINT("    Cmd hash:   %llu\n", cmd->hash);
    PRINT("    Cmd value:  %u\n",   cmd->value_8b);
    PRINT("    Cmd n_args: %llu\n", cmd->n_args);
}

static void list_lit(Arg_literal* lit, FILE* stream)
{
    PRINT("          value_64b: %lg\n", lit->value_64b);
    PRINT("          value_8b:  %u\n",  lit->value_8b);
    PRINT("          sign:      %d\n",  lit->sign);
    PRINT("          type:      %d\n",  lit->type);
}

static void list_arg(Argument* arg, FILE* stream)
{
    PRINT("      isRAM:  %d\n",   arg->memory);
    PRINT("      lit_sz: %llu\n", arg->literals_sz);

    PRINT("      Literals:\n");
    PRINT("      {\n");
    for(size_t lit_iter = 0; lit_iter < LITS_CAP; lit_iter++)
    {
        PRINT("        Literal #%llu\n", lit_iter);
        list_lit(&arg->literals[lit_iter], stream);
    }
    PRINT("      }\n");
}

void list_expr(Expression* expr, char* txt)
{
    list_stream_();
    FILE* stream = PREPROCESSOR_STREAM;
    if(!stream)
        return;

    PRINT("Expression\n");
    
    PRINT("  Txt code: `%s`\n", txt);
    PRINT("  Command:\n");
    list_cmd(&expr->cmd, stream);

    PRINT("  args_sz: %llu\n", expr->args_sz);

    PRINT("  Arguments:\n");
    PRINT("  {\n");
    for(size_t arg_iter = 0; arg_iter < ARGS_CAP; arg_iter++)
    {
        PRINT("    Argument #%llu\n", arg_iter);
        list_arg(&expr->args[arg_iter], stream);
    }
    PRINT("  }\n\n");

    fflush(stream);
}
