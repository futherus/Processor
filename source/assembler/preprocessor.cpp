#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "../hash.h"
#include "../log/log.h"
#include "preprocessor.h"
#include "preprocessor_list.h"

Preprocessor_errstruct* preprocessor_errstruct()
{
    static Preprocessor_errstruct errstruct = {};

    return &errstruct;
}

#define PREPROCESSOR_ASSERT(CONDITION, ERR)                         \
    do                                                              \
    {                                                               \
        if(!(CONDITION))                                            \
        {                                                           \
            Preprocessor_errstruct* ptr = preprocessor_errstruct(); \
            ptr->errnum = (ERR);                                    \
            return (ERR);                                           \
        }                                                           \
    } while(0)                                                      \

#define PREPROCESSOR_ASSERT_SETLINE(CONDITION, ERR, LINE)           \
    do                                                              \
    {                                                               \
        if(!(CONDITION))                                            \
        {                                                           \
            Preprocessor_errstruct* ptr = preprocessor_errstruct(); \
            ptr->line = (LINE);                                     \
            ptr->errnum = (ERR);                                    \
            return (ERR);                                           \
        }                                                           \
    } while(0)                                                      \

#define PREPROCESSOR_ASSERT_SETPOS(CONDITION, ERR, POS)             \
    do                                                              \
    {                                                               \
        if(!(CONDITION))                                            \
        {                                                           \
            Preprocessor_errstruct* ptr = preprocessor_errstruct(); \
            ptr->pos = (POS);                                       \
            ptr->errnum = (ERR);                                    \
            return (ERR);                                           \
        }                                                           \
    } while(0)                                                      \


static void clean_whitespaces(char* txt, size_t* pos)
{
    assert(txt && pos);

    while(isspace(txt[*pos]))
        (*pos)++;
}

static void wordlen(char* txt, size_t* pos, size_t* n_read)
{
    assert(txt && pos && n_read);

    while(isalpha(txt[*pos + *n_read]))
        (*n_read)++;
}

static preprocessor_err get_cmd(Expression* expr, char* txt, size_t* pos)
{
    assert(expr && txt && pos);

    size_t n_read = 0;
    wordlen(txt, pos, &n_read);
    expr->cmd.hash = qhashfnv1_64(txt + *pos, n_read);

    switch(expr->cmd.hash)
    {
        case HASH_push:                   
            expr->cmd.value_8b = EMBCMD_push; 
            expr->cmd.n_args   = 1;
            break;
        case HASH_out:
            expr->cmd.value_8b = CMD_out;
            expr->cmd.n_args   = 0;
            break;
        //.
        //.
        //.
        default:
            PREPROCESSOR_ASSERT_SETPOS(0, PREPROCESSOR_UNKNWN_CMD, *pos);
    }
    
    *pos += n_read;

    return PREPROCESSOR_NOERR;
}

#define CUR_LITER_ (arg->literals[arg->literals_sz])
static preprocessor_err get_arg(Argument* arg, char* txt, size_t* pos)
{
    assert(arg && txt && pos);

    preprocessor_err err = PREPROCESSOR_NOERR;

    clean_whitespaces(txt, pos);

    if(txt[*pos] == '+' || txt[*pos] == '-')
    {
        if(CUR_LITER_.sign != 0) 
            PREPROCESSOR_ASSERT_SETPOS(0, PREPROCESSOR_SIGN_DUPLICATE, *pos);
        
        CUR_LITER_.sign = (txt[*pos] == '+') ? 1 : -1;
        (*pos)++;
        
        err = get_arg(arg, txt, pos);
        PREPROCESSOR_ASSERT(!err, err);

        return PREPROCESSOR_NOERR;
    }
    
    size_t n_read = 0;

    if(sscanf(txt + *pos, "%lg%n", &CUR_LITER_.value_64b, &n_read) > 0)
    {
        CUR_LITER_.type = ARG_IMMCONST;
        arg->literals_sz++;
    }
    else
    {
        wordlen(txt, pos, &n_read);
        if(n_read == 0)
            return PREPROCESSOR_NOERR; // <---POINT OF EXIT
        
        uint64_t hash = qhashfnv1_64(txt + *pos, n_read);

        switch(hash)
        {
            case HASH_rax:
                CUR_LITER_.type     = ARG_REGISTER;
                CUR_LITER_.value_8b = 1;
                arg->literals_sz++;
                break;

            case HASH_rbx:
                CUR_LITER_.type     = ARG_REGISTER;
                CUR_LITER_.value_8b = 2;
                arg->literals_sz++;
                break;

            case HASH_rcx:
                CUR_LITER_.type     = ARG_REGISTER;
                CUR_LITER_.value_8b = 3;
                arg->literals_sz++;
                break;

            default:
                PREPROCESSOR_ASSERT_SETPOS(0, PREPROCESSOR_UNKNWN_ARG, *pos);
        }
    }

    *pos += n_read;
    
    err = get_arg(arg, txt, pos);

    return err;
}
#undef CUR_LITER

static preprocessor_err verify_arg(Argument* arg)
{
    assert(arg);

    if(arg->literals_sz == 0)
        PREPROCESSOR_ASSERT(arg->literals != 0, PREPROCESSOR_INVALID_ARG);

    for(size_t lit_iter = 1; lit_iter < arg->literals_sz; lit_iter++)
        PREPROCESSOR_ASSERT(arg->literals[lit_iter].sign != 0, PREPROCESSOR_MISSING_SIGN);
    
    return PREPROCESSOR_NOERR;
}

static preprocessor_err verify_expr(Expression* expr)
{
    assert(expr);

    preprocessor_err err = PREPROCESSOR_NOERR;

    PREPROCESSOR_ASSERT(expr->cmd.n_args == expr->args_sz, PREPROCESSOR_INVALID_ARGS_NUM);
    
    if(expr->cmd.value_8b == EMBCMD_pop)
        PREPROCESSOR_ASSERT(expr->args[0].literals_sz == 1, PREPROCESSOR_LITS_FOR_POP); // EMBCMD_pop cannot evaluate args
    
    for(size_t arg_iter = 0; arg_iter < expr->args_sz; arg_iter++)
    {
        err = verify_arg(&expr->args[arg_iter]);
        PREPROCESSOR_ASSERT(!err, err);
    }

    return PREPROCESSOR_NOERR;
}

static preprocessor_err get_expr(Expression* expr, char* txt)
{
    assert(expr && txt);

    preprocessor_err err = PREPROCESSOR_NOERR;
    size_t pos = 0;

    err = get_cmd(expr, txt, &pos);
    if(err)
        return err;

    clean_whitespaces(txt, &pos);

    while(true)
    {
        if(txt[pos] == '\0')
            break;

        PREPROCESSOR_ASSERT_SETPOS(txt[pos] != ',', PREPROCESSOR_TRAILING_COMMA, pos);

        if(txt[pos] == '[')
        {
            expr->args[expr->args_sz].memory = MEMORY_RAM;
            pos++;
        }

        err = get_arg(&expr->args[expr->args_sz], txt, &pos);
        if(err)
            return err;

        clean_whitespaces(txt, &pos);

        if(expr->args[expr->args_sz].memory == MEMORY_RAM)
        {
            PREPROCESSOR_ASSERT_SETPOS(txt[pos] == ']', PREPROCESSOR_MISSING_PARENTHESIS, pos);
            
            pos++;
        }


        expr->args_sz++;
        
        clean_whitespaces(txt, &pos);

        PREPROCESSOR_ASSERT_SETPOS(txt[pos] == ',' || txt[pos] == '\0', PREPROCESSOR_TRAILING_SYMBOLS, pos);

        if(txt[pos] == '\0')
            break;
        
        pos++;
        clean_whitespaces(txt, &pos);

        PREPROCESSOR_ASSERT_SETPOS(txt[pos] != '\0', PREPROCESSOR_TRAILING_COMMA, pos);
    }
    while(txt[pos++] == ',');

    err = verify_expr(expr);
    PREPROCESSOR_ASSERT_SETPOS(!err, err, 0);

    return PREPROCESSOR_NOERR;
}


#define PUT_CMD(code)                                                       \
    do                                                                      \
    {                                                                       \
        cmd_t temp = {};                                                    \
        temp.bits.cmd = (code);                                             \
                                                                            \
        dst_line[*pos] = temp.byte;                                         \
        *pos += sizeof(cmd_t);                                              \
    } while(0)                                                              \

#define PUT_CMD_WA(code, memory, arg_lit)                                   \
    do                                                                      \
    {                                                                       \
        cmd_t temp = {};                                                    \
        temp.bits.cmd = (code);                                             \
        temp.bits.mem = (memory);                                           \
        temp.bits.reg = (arg_lit).type;                                     \
                                                                            \
        dst_line[*pos] = temp.byte;                                         \
        *pos += sizeof(cmd_t);                                              \
                                                                            \
        if((arg_lit).type == ARG_IMMCONST)                                  \
        {                                                                   \
            memcpy(dst_line + *pos, &(arg_lit).value_64b, sizeof(val64_t)); \
            *pos += sizeof(val64_t);                                        \
        }                                                                   \
        else                                                                \
        {                                                                   \
            memcpy(dst_line + *pos, &(arg_lit).value_8b,  sizeof(val8_t));  \
            *pos += sizeof(val8_t);                                         \
        }                                                                   \
    } while(0)                                                              \

static void put_evaluate_arg(bin_t* dst_line, size_t* pos, Argument* arg)
{
    Arg_literal zero_lit = {};
    zero_lit.type = ARG_IMMCONST;
    zero_lit.value_64b = 0.0;

    PUT_CMD_WA(EMBCMD_push, MEMORY_NOT_RAM, zero_lit); // Zero for arg calculation
    
    for(size_t lit_iter = 0; lit_iter < arg->literals_sz; lit_iter++)
    {
        PUT_CMD_WA(EMBCMD_push, MEMORY_NOT_RAM, arg->literals[lit_iter]); // push literal

        if(arg->literals[lit_iter].sign == -1)
            PUT_CMD(EMBCMD_sub); // - -> sub 
        else
            PUT_CMD(EMBCMD_add); // + -> add
    }

    Arg_literal dax_lit = {};
    dax_lit.type = ARG_REGISTER;
    dax_lit.value_8b = REG_dax;

    PUT_CMD_WA(EMBCMD_pop, MEMORY_NOT_RAM, dax_lit); // pop argument to dax (dark register)

    if(arg->memory == MEMORY_RAM)  //if access to RAM
    {
        PUT_CMD_WA(EMBCMD_push, MEMORY_RAM, dax_lit); // push RAM[dax]

        PUT_CMD_WA(EMBCMD_pop, MEMORY_NOT_RAM, dax_lit); // pop to dax
    }
}

static preprocessor_err put_expr(bin_t* dst_line, size_t* pos, Expression* expr)
{
    assert(dst_line && pos && expr); 
    
    if(expr->args_sz == 0) // cmd without args
    {
        PUT_CMD(expr->cmd.value_8b);
    }
    else if(expr->cmd.value_8b == EMBCMD_pop) // EMBCMD_pop works with registers and memory directly, argument is one literal and can't be evaluated
    {
        PUT_CMD_WA(EMBCMD_pop, expr->args[0].memory, expr->args[0].literals[0]);
    }
    else
    {
        for(size_t arg_iter = 0; arg_iter < expr->args_sz; arg_iter++)
        {
            put_evaluate_arg(dst_line, pos, &expr->args[arg_iter]); // evaluates argument, pushes it to dax
        }

        PUT_CMD(expr->cmd.value_8b); // implicitly gets argument from dax 
    }

    return PREPROCESSOR_NOERR;
}

static preprocessor_err preprocess_line(bin_t* bin_line, size_t* bin_line_sz, char* txt_line)
{
    assert(bin_line && bin_line_sz && txt_line);

    Expression expr = {};

    preprocessor_err err = PREPROCESSOR_NOERR;

    err = get_expr(&expr, txt_line);
    PREPROCESSOR_ASSERT(!err, err);

    list_expr(&expr, txt_line);

    put_expr(bin_line, bin_line_sz, &expr);

    return err;
}

preprocessor_err preprocessor(Binary* bin, const Text* txt)
{
    preprocessor_err err = PREPROCESSOR_NOERR;

    bin->sz = 0;
    size_t n_line = txt->index_arr_size;

    for(size_t line = 0; line < n_line; line++)
    {
        //DI$(line);
        size_t bin_line_sz = 0;
        bin_t bin_line[BIN_LINE_CAP] = {0};

        char* txt_line = txt->index_arr[line].begin;

        err = preprocess_line(bin_line, &bin_line_sz, txt_line);
        PREPROCESSOR_ASSERT_SETLINE(!err, err, line);



        list_line(bin_line, bin_line_sz, txt_line, line);

        binary_sread(bin, bin_line, bin_line_sz);
    }

    return PREPROCESSOR_NOERR;
}

/*
int test_function(char* txt)
{
    Expression expr = {};

    preprocessor_err err = get_expr(&expr, txt);

    list_expr(&expr, txt);

    bin_t bin_line[BIN_LINE_CAP] = {};

    size_t bin_line_sz = 0;

    put_expr(&bin_line, &bin_line_sz, &expr);

    return 0;
}

*/