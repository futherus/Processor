#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "parser.h"
#include "../log/log.h"
#include "asm_list.h"

struct Expression
{
    char txt_cmd[TXT_CMD_CAP] = "";
    unsigned int bin_cmd = 0;
    arg_t args[ARGS_CAP] = {0}; 
    size_t n_args = 0;
};
 
static parser_err get_args(Expression* expr, char* txt_line, size_t* pos)
{
    for(size_t iter = 0; iter < expr->n_args; iter++)
    {
        size_t n_read = 0;

        sscanf(txt_line + *pos, "%lg%n", &(expr->args[iter]), &n_read);
        
        *pos += n_read;
    }

    return PARSER_NOERR;
}

#define DEF_CMD(TEXT, N_ARGS, code)                                             \
    if (strcmp(expr->txt_cmd, #TEXT) == 0)                                      \
    {                                                                           \
        expr->bin_cmd = (CMD_##TEXT);                                           \
        expr->n_args  = (N_ARGS);                                               \
    }                                                                           \
    else                                                                        \

static parser_err get_expr(Expression* expr, char* txt_line)
{
    assert(expr && txt_line);

    size_t pos = 0;
    sscanf(txt_line, "%s%n", expr->txt_cmd, &pos);

    #include "../def_cmd.inc"
    /* else */ ASSERT(0, PARSER_UNKNWN_CMD);

    get_args(expr, txt_line, &pos);

    char probe = 0;
    ASSERT(sscanf(txt_line + pos, " %c", &probe) == EOF, PARSER_UNEXPCTD_ARGS);

    return PARSER_NOERR;
}

#undef DEF_CMD

static parser_err put_expr(bin_t* dst_line, size_t* dst_sz, Expression* expr)
{
    assert(dst_line && dst_sz && expr);

    size_t pos = 0;

    cmd_t temp = {};
    temp.bits.cmd = expr->bin_cmd;
    dst_line[pos] = temp.byte;
    pos += sizeof(cmd_t);

    for(size_t arg_iter = 0; arg_iter < expr->n_args; arg_iter++)
    {
        *((arg_t*) (dst_line + pos)) = expr->args[arg_iter];
        pos += sizeof(arg_t);
    }

    *dst_sz = pos;

    return PARSER_NOERR;
}

static parser_err parse_line(bin_t* bin_line, size_t* bin_line_sz, char* txt_line)
{
    assert(bin_line && bin_line_sz && txt_line);

    Expression expr = {};

    parser_err err = PARSER_NOERR;

    err = get_expr(&expr, txt_line);
    ASSERT(!err, PARSER_READ_FAIL);

    err = put_expr(bin_line, bin_line_sz, &expr);
    ASSERT(!err, PARSER_WRITE_FAIL);

    return err;
}

parser_err parser(Binary* bin, const Text* txt)
{
    parser_err err = PARSER_NOERR;

    bin->sz = 0;
    size_t n_line = txt->index_arr_size;

    for(size_t line = 0; line < n_line; line++)
    {
        DI$(line);
        size_t bin_line_sz = 0;
        bin_t bin_line[BIN_LINE_CAP] = {0};

        char* txt_line = txt->index_arr[line].begin;

        err = parse_line(bin_line, &bin_line_sz, txt_line);
        ASSERT(!err, PARSER_LINE_FAIL);

        list_line(bin_line, bin_line_sz, txt_line, line);

        binary_sread(bin, bin_line, bin_line_sz);
    }

    return PARSER_NOERR;
}