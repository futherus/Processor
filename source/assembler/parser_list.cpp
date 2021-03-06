#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser_list.h"
#include "../common/dumpsystem.h"

static FILE* PARSER_STREAM  = nullptr;
static FILE* BINLINE_STREAM = nullptr;

const static char MISSING_PARENTHESES[] = "closing parenthesis is missing";
const static char TRAILING_SYMBOLS[]    = "trailing symbols in argument";
const static char TRAILING_COMMA[]      = "trailing comma";
const static char NO_ARG[]              = "argument is missing";
const static char NO_CMD[]              = "command was expected";
const static char NOTLABEL[]            = "label was expected";
const static char UNKNWN_ARG[]          = "unknown argument";
const static char INVALID_ARGS_NUM[]    = "invalid amount of args entered";
const static char UNKNWN_CMD[]          = "unknown command";
const static char LABEL_REDECL[]        = "label redeclaration";
const static char LABEL_NODECL[]        = "no label declaration";

void parser_dump_init()
{
    BINLINE_STREAM = dumpsystem_get_stream(binline_list);
    PARSER_STREAM  = dumpsystem_get_stream(statement_list);
}

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__)

////////////////////////   Error logging   ////////////////////////
void log_err()
{
    FILE* stream = stderr;

    Parser_errstruct* ptr = parser_errstruct();
    PRINT("\n" "%s:%lu:%lu: ", ptr->infile, ptr->line + 1, ptr->pos);
    PRINT("error: ");
    
    switch(ptr->errnum)
    {
        case PARSER_MISSING_PARENTHESES:
            PRINT("%s\n", MISSING_PARENTHESES);
            break;
        case PARSER_TRAILING_SYMBOLS:
            PRINT("%s\n", TRAILING_SYMBOLS);
            break;
        case PARSER_TRAILING_COMMA:
            PRINT("%s\n", TRAILING_COMMA);
            break;
        case PARSER_UNKNWN_ARG:
            PRINT("%s\n", UNKNWN_ARG);
            break;
        case PARSER_INVALID_ARGS_NUM:
            PRINT("%s\n", INVALID_ARGS_NUM);
            break;
        case PARSER_UNKNWN_CMD:
            PRINT("%s\n", UNKNWN_CMD);
            break;
        case PARSER_NO_ARG:
           PRINT("%s\n", NO_ARG);
           break;
        case PARSER_NO_CMD:
           PRINT("%s\n", NO_CMD);
           break;
        case PARSER_NOTLABEL:
           PRINT("%s\n", NOTLABEL);
           break;
        case PARSER_LABEL_REDECL:
            PRINT("%s\n", LABEL_REDECL);
            break;
        case PARSER_LABEL_NODECL:
            PRINT("%s\n", LABEL_NODECL);
            break;
        case PARSER_NOERR:
            /* fallthrough */
        default:
            PRINT("Error type was not set\n");
    }

    PRINT("    %s\n\n", ptr->txt);
}

////////////////////////    Line listing    ////////////////////////
void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t line, size_t next_ip)
{
    static bool first_call = 1;
    FILE* stream = BINLINE_STREAM;
    if(!stream)
        return;

    static size_t ip = 0;

    if(first_call)
    {
        PRINT("|Line||Command----------------||-IP-|");
        for(int addr = 0; addr < 64; addr++)
        {
            if(addr % 8 == 0)
                PRINT("|");
            
            PRINT("%2d ", addr);
        }
        
        PRINT("\n");
        first_call = 0;
    }

    PRINT("|%4lu|", line);
    PRINT("|%-23s|", txt_line);
    PRINT("|%4lu|", ip);

    for(size_t iter = 0; iter < bin_line_sz; iter++)
    {
        if(iter % 8 == 0)
            PRINT("|");

        PRINT("%02hx ", bin_line[iter]);
    }
    PRINT("\n");

    ip = next_ip;

    fflush(stream);
}

//////////////////////// Expression listing ////////////////////////
static void list_cmd(Command* cmd, FILE* stream)
{
    PRINT("    value:  %u\n",   cmd->code);
    PRINT("    is_sys  %d\n",   cmd->is_sys);
    PRINT("    hash:   %lu\n", cmd->hash);
    PRINT("    n_args: %lu\n", cmd->n_args);
}

static void list_lex(Lexem* lex, FILE* stream)
{
    PRINT("          type:           %u\n",   lex->type);
    PRINT("          value (double): %lg\n",  lex->value.num);
    PRINT("          pos:            %lu\n", lex->pos);
}

static void list_arg(Argument* arg, FILE* stream)
{
    PRINT("      isRAM:  %d\n",   arg->memory);
    PRINT("      lit_sz: %lu\n", arg->lexs_sz);

    PRINT("      Lexems:\n");
    PRINT("      {\n");
    for(size_t lit_iter = 0; lit_iter < arg->lexs_sz; lit_iter++)
    {
        PRINT("        Lexem #%lu\n", lit_iter);
        list_lex(&arg->lexs[lit_iter], stream);
    }
    PRINT("      }\n");
}

void list_statement(Statement* stment, char* txt)
{
    FILE* stream = PARSER_STREAM;
    if(!stream)
        return;

    PRINT("Statement\n");
    
    PRINT("  Txt code: `%s`\n", txt);
    PRINT("  Command:\n");
    list_cmd(&stment->cmd, stream);

    PRINT("  args_sz: %lu\n", stment->args_sz);

    PRINT("  Arguments:\n");
    PRINT("  {\n");
    for(size_t arg_iter = 0; arg_iter < stment->args_sz; arg_iter++)
    {
        PRINT("    Argument #%lu\n", arg_iter);
        list_arg(&stment->args[arg_iter], stream);
    }
    PRINT("  }\n\n");

    fflush(stream);
}

////////////////////////  Labels listing  ////////////////////////
void list_labels(Label_array* lbl_arr)
{
    FILE* stream = PARSER_STREAM;
    if(!stream)
        return;
    
    PRINT("Labels:\n");
    PRINT("{\n");
    for(size_t lbl_iter = 0; lbl_iter < lbl_arr->labels_sz; lbl_iter++)
    {
        PRINT("  Label:\n");
        PRINT("    ip:   %lu\n", lbl_arr->labels[lbl_iter].ip);
        PRINT("    hash: %lu\n", lbl_arr->labels[lbl_iter].hash);
    }
    PRINT("}\n");

    fflush(stream);
}
