#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "parser.h"
#include "debug.h"
 
#define DEF_CMD(text, binary, num_args)                                         \
    (strcmp(expr->txt_cmd, (text)) == 0)                                        \
    {                                                                           \
        expr->bin_cmd = (double) (binary);                                      \
        expr->n_args  = (num_args);                                             \
                                                                                \
        for(size_t iter = 0; iter < (num_args); iter++)                         \
        {                                                                       \
            size_t n_read = 0;                                                  \
            sscanf(txt_line + pos, "%lg%n", &(expr->args[iter]), &n_read);      \
            pos += n_read;                                                      \
        }                                                                       \
    }                                                                           \

struct Expression
{
    char txt_cmd[TXT_CMD_CAP] = "";
    bin_t bin_cmd = 0;
    bin_t args[ARGS_CAP] = {0}; 
    size_t n_args = 0;
};

static parser_err get_cmd(Expression* expr, char* txt_line, size_t txt_line_sz)
{
    assert(expr && txt_line);

    size_t pos = 0;
    sscanf(txt_line, "%s%n", expr->txt_cmd, &pos);

    if      DEF_CMD(IN,   in,   0)
    else if DEF_CMD(OUT,  out,  0)
    else if DEF_CMD(MUL,  mul,  0)
    else if DEF_CMD(ADD,  add,  0)
    else if DEF_CMD(HAL,  hal,  0)
    else if DEF_CMD(PUSH, push, 1)
    else
    {
        expr->bin_cmd = (double) BAD_CMD;
        ASSERT(0, PARSER_UNKNWN_CMD);
    }

    char probe = 0;
    ASSERT(sscanf(txt_line, " %c", &probe) != EOF, PARSER_UNEXPCTD_ARGS);

    return PARSER_NOERR;
}

static parser_err put_expr(bin_t* dst_line, size_t* dst_sz, Expression* expr)
{
    size_t pos = 0;
    dst_line[pos++] = expr->bin_cmd;
    
    size_t arg_iter = 0;
    while(arg_iter < expr->n_args)
    {
        dst_line[pos + arg_iter] = expr->args[arg_iter];
        arg_iter++;
    }
    pos += arg_iter;

    *dst_sz = pos;

    return PARSER_NOERR;
}

static parser_err parse_line(bin_t* bin_line, size_t* bin_line_sz, char* txt_line, size_t txt_line_sz)
{
    Expression expr = {};

    parser_err err = PARSER_NOERR;

    err = get_cmd(&expr, txt_line, txt_line_sz);
    ASSERT(!err, PARSER_CMD_FAIL);

    err = put_expr(bin_line, bin_line_sz, &expr);
    ASSERT(!err, PARSER_PUT_FAIL);

    return err;
}

parser_err parser(Binary* bin, const Text* txt)
{
    parser_err err = PARSER_NOERR;

    bin->sz = 0;
    size_t n_line = txt->index_arr_size;

    for(size_t line = 0; line < n_line; line++)
    {
        size_t bin_line_sz = 0;
        bin_t bin_line[BIN_LINE_CAP] = {0};

        char*  txt_line    = txt->index_arr[line].begin;
        size_t txt_line_sz = txt->index_arr[line].end - txt_line;

        L$(err = parse_line(bin_line, &bin_line_sz, txt_line, txt_line_sz);)
        ASSERT(!err, PARSER_LINE_FAIL);

        list_line(bin_line, bin_line_sz, txt_line, txt_line_sz, line);

        binary_sread(bin, bin_line, bin_line_sz);
    }

    return PARSER_NOERR;
}