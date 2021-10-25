#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "preprocessor_list.h"

static FILE* PREPROCESSOR_STREAM = nullptr;
static FILE* BINLINE_STREAM      = nullptr;

const static char MISSING_SIGN[]        = "sign before literal is missing";
const static char SIGN_DUPLICATE[]      = "sign of literal is duplicated";
const static char MISSING_PARENTHESIS[] = "closing parenthesis is missing";
const static char TRAILING_SYMBOLS[]    = "trailing symbols in argument";
const static char TRAILING_COMMA[]      = "trailing comma";
const static char INVALID_ARG[]         = "argument is invalid";
const static char UNKNWN_ARG[]          = "unknown argument";
const static char INVALID_ARGS_NUM[]    = "invalid amount of args entered";
const static char LITS_FOR_SYS[]        = "system command can get only one-literal argument";
const static char SIGN_FOR_SYS[]        = "system command cannot have signed argument";
const static char IMMCONST_FOR_POP[]    = "`pop` command cannot access number";
const static char UNKNWN_CMD[]          = "unknown command";

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

        BINLINE_STREAM      = fopen(BINLINE_LISTFILE,      "w");
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

////////////////////////    Error logging   ////////////////////////
void log_err(const char infile_name[])
{
    FILE* stream = stderr;

    Preprocessor_errstruct* ptr = preprocessor_errstruct();
    PRINT("\n" "%s:%llu:%llu: ", infile_name, ptr->line + 1, ptr->pos);
    PRINT("error: ");
    
    switch(ptr->errnum)
    {
        case PREPROCESSOR_MISSING_SIGN:
            PRINT("%s\n", MISSING_SIGN);
            break;
        case PREPROCESSOR_SIGN_DUPLICATE:
            PRINT("%s\n", SIGN_DUPLICATE);
            break;
        case PREPROCESSOR_MISSING_PARENTHESIS:
            PRINT("%s\n", MISSING_PARENTHESIS);
            break;
        case PREPROCESSOR_TRAILING_SYMBOLS:
            PRINT("%s\n", TRAILING_SYMBOLS);
            break;
        case PREPROCESSOR_TRAILING_COMMA:
            PRINT("%s\n", TRAILING_COMMA);
            break;
        case PREPROCESSOR_INVALID_ARG:
            PRINT("%s\n", INVALID_ARG);
            break;
        case PREPROCESSOR_UNKNWN_ARG:
            PRINT("%s\n", UNKNWN_ARG);
            break;
        case PREPROCESSOR_INVALID_ARGS_NUM:
            PRINT("%s\n", INVALID_ARGS_NUM);
            break;
        case PREPROCESSOR_LITS_FOR_SYS:
            PRINT("%s\n", LITS_FOR_SYS);
            break;
        case PREPROCESSOR_SIGN_FOR_SYS:
            PRINT("%s\n", SIGN_FOR_SYS);
            break;
        case PREPROCESSOR_UNKNWN_CMD:
            PRINT("%s\n", UNKNWN_CMD);
            break;
        case PREPROCESSOR_IMMCONST_FOR_POP:
            PRINT("%s\n", IMMCONST_FOR_POP);
            break;
        case PREPROCESSOR_NOERR:
            /* fallthrough */
        default:
            PRINT("Error type was not set\n");
    }

    PRINT("    %s\n\n", ptr->txt);
}

////////////////////////    Line listing    ////////////////////////
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
    for(size_t lit_iter = 0; lit_iter < arg->literals_sz; lit_iter++)
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
    for(size_t arg_iter = 0; arg_iter < expr->args_sz; arg_iter++)
    {
        PRINT("    Argument #%llu\n", arg_iter);
        list_arg(&expr->args[arg_iter], stream);
    }
    PRINT("  }\n\n");

    fflush(stream);
}
