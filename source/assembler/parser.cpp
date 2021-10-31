#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "parser_list.h"
#include "../../debug_tools/debug_tools.h"

static int ITERATION = 0;

Parser_errstruct* parser_errstruct()
{
    static Parser_errstruct errstruct = {};

    return &errstruct;
}

#define PARSER_ASSERT(CONDITION, ERR)                               \
    do                                                              \
    {                                                               \
        if(!(CONDITION))                                            \
        {                                                           \
            Parser_errstruct* ptr = parser_errstruct();             \
            ptr->errnum = (ERR);                                    \
            return (ERR);                                           \
        }                                                           \
    } while(0)                                                      \

#define PARSER_ASSERT_SETPOS(CONDITION, ERR, POS)                   \
    do                                                              \
    {                                                               \
        if(!(CONDITION))                                            \
        {                                                           \
            Parser_errstruct* ptr = parser_errstruct();             \
            ptr->pos = (POS);                                       \
            ptr->errnum = (ERR);                                    \
            return (ERR);                                           \
        }                                                           \
    } while(0)                                                      \

#define PARSER_LOG_ERR(ERR, LINE, TXT, INFILE)                      \
                                                                    \
    {                                                               \
        Parser_errstruct* ptr = parser_errstruct();                 \
        ptr->line   = (LINE);                                       \
        ptr->errnum = (ERR);                                        \
        strcpy(ptr->txt, TXT);                                      \
        strcpy(ptr->infile, INFILE);                                \
        log_err();                                                  \
        continue;                                                   \
    }                                                               \

/////////////////////////////////////////////////////////////////////////////

static Labels_array* labels_array()
{
    static Labels_array lbl_arr = {};

    return &lbl_arr;
}

static parser_err label(Lexems_array* lexs, Label* lbl, size_t ip)
{
    assert(lexs && lbl);

    if(lexs->lexems[0].type != LEX_WORD)
        return PARSER_NOTLABEL;

    if(lexs->lexems[1].type != LEX_COLON)
        return PARSER_NOTLABEL;
        
    if(lexs->lexems_sz != 2)
        return PARSER_NOTLABEL;
    
    lbl->hash = lexs->lexems[0].value.hash;
    lbl->ip   = ip;

    return PARSER_NOERR;
}

static parser_err get_label(Label* lbl)
{
    assert(lbl->hash != 0);

    Labels_array* lbl_arr = labels_array();

    for(size_t iter = 0; iter < lbl_arr->labels_sz; iter++)
    {
        if(lbl_arr->labels[iter].hash == lbl->hash)
        {
            lbl->ip = lbl_arr->labels[iter].ip;
            return PARSER_NOERR;
        }
    }

    return PARSER_LABEL_NODECL;
}

static parser_err add_label(Label* lbl)
{
    Labels_array* lbl_arr = labels_array();

    if(get_label(lbl) == PARSER_LABEL_NODECL)
    {
        lbl_arr->labels[lbl_arr->labels_sz].ip   = lbl->ip;
        lbl_arr->labels[lbl_arr->labels_sz].hash = lbl->hash;
        lbl_arr->labels_sz++;

        return PARSER_NOERR;
    }

    return PARSER_LABEL_REDECL;
}

/////////////////////////////////////////////////////////////////////////////

/*  Grammar:
    Statement
        Command Argument
        Statement "," Argument
    Argument
        LEX_WORD
        "[" LEX_WORD "]"
        Expression
        "[" Expression "]"
    Expression
        Term
        Term "+" Expression
        Term "-" Expression
    Term
        Primary
        Term "*" Primary
    Primary
        "(" Expression ")"
        LEX_REGISTER
        LEX_IMMCONST
        "-"
        "+"
*/

static parser_err expression(Lexems_array*, Argument*);

static parser_err primary(Lexems_array* lexs, Argument* arg)
{
    assert(lexs && arg);

    int err = 0;

    Lexem lex = {};

    PARSER_ASSERT_SETPOS(consume(&lex, lexs) == 0, PARSER_NO_ARG, lex.pos);
    
    switch(lex.type)
    {
        case LEX_ADD:           // unary plus, no need to do anything
            primary(lexs, arg);

            break;
        case LEX_SUB:           // unary minus, converting '-x' -> '0 - x'
            lex.type = LEX_IMMCONST;
            lex.value.num = 0;
            arg->lexs[arg->lexs_sz++] = lex;

            primary(lexs, arg);

            lex.type = LEX_SUB;
            arg->lexs[arg->lexs_sz++] = lex;

            break;
        case LEX_LRPAR:
            expression(lexs, arg);

            err = consume(&lex, lexs);
            PARSER_ASSERT_SETPOS(!err && lex.type == LEX_RRPAR, PARSER_MISSING_PARENTHESES, lex.pos);
            
            break;
        case LEX_IMMCONST: case LEX_REGISTER :
            arg->lexs[arg->lexs_sz++] = lex;

            break;
        default:
            PARSER_ASSERT_SETPOS(0, PARSER_UNKNWN_ARG, lex.pos);
            //if(getlabel)
    }

    return PARSER_NOERR;
}

static parser_err term(Lexems_array* lexs, Argument* arg)
{
    assert(lexs && arg);

    parser_err err = PARSER_NOERR;
    
    err = primary(lexs, arg);
    if(err)
        return err;

    Lexem lex = {};

    while(peek(&lex, lexs) == 0)
    {
        switch(lex.type)
        {
            case LEX_MUL:
                consume(&lex, lexs);

                err = primary(lexs, arg);
                if(err)
                    return err;

                arg->lexs[arg->lexs_sz++] = lex;

                break;
            default:
                return PARSER_NOERR;
        }
    }

    return PARSER_NOERR;
}

static parser_err expression(Lexems_array* lexs, Argument* arg)
{
    assert(lexs && arg);

    parser_err err = PARSER_NOERR;

    err = term(lexs, arg);
    if(err)
        return err;

    Lexem lex = {};

    while(peek(&lex, lexs) == 0)
        switch(lex.type)
        {
            case LEX_ADD : case LEX_SUB :
                consume(&lex, lexs);

                err = term(lexs, arg);
                if(err)
                    return err;

                arg->lexs[arg->lexs_sz++] = lex;

                break;
            default:
                return PARSER_NOERR;
        }

    return PARSER_NOERR;
}

static parser_err label_arg(Lexems_array* lexs, Argument* arg)
{
    parser_err err = PARSER_NOERR;

    Lexem lex = {};
    Label lbl = {};

    peek(&lex, lexs);
    if(lex.type != LEX_WORD)
        return PARSER_NOTLABEL;

    consume(&lex, lexs);

    lbl.hash = lex.value.hash;
    err = get_label(&lbl);
    if(err)
    {
        if(ITERATION == 2)
            PARSER_ASSERT_SETPOS(0, err, lex.pos);
        else
            lbl.ip = SIZE_MAX;
    }

    lex.type = LEX_INSTRPTR;
    lex.value.ip = lbl.ip;
    
    arg->lexs[0] = lex;

    return PARSER_NOERR;
}

static parser_err argument(Lexems_array* lexs, Argument* arg)
{
    parser_err err = PARSER_NOERR;

    Lexem lex = {};

    peek(&lex, lexs);
    if(lex.type == LEX_LQPAR)
    {
        consume(&lex, lexs);
        arg->memory = MEM_RAM;
    }
    
    if((err = label_arg(lexs, arg)) == PARSER_NOERR)
        ;
    else if((err = expression(lexs, arg)) == PARSER_NOERR)
        ;
    else
        PARSER_ASSERT_SETPOS(0, err, lex.pos);

    if(arg->memory == MEM_RAM)
    {
        err = (parser_err) consume(&lex, lexs);
        PARSER_ASSERT_SETPOS(!err && lex.type == LEX_RQPAR, PARSER_MISSING_PARENTHESES, lex.pos);
    }
    
    return PARSER_NOERR;
}

#define DEF_SYSCMD(TXT, HASH, N_ARGS, CODE) \
    case (SYSCMD_##TXT):                    \
        cmd->code     = SYSCMD_##TXT;       \
        cmd->n_args   = (N_ARGS);           \
        cmd->is_sys   = 1;                  \
        break;                              \

#define DEF_CMD(TXT, HASH, N_ARGS, CODE)    \
    case (CMD_##TXT):                       \
        cmd->code     = CMD_##TXT;          \
        cmd->n_args   = (N_ARGS);           \
        cmd->is_sys   = 0;                  \
        break;                              \

static parser_err command(Lexems_array* lexs, Command* cmd)
{
    Lexem lex = {};

    PARSER_ASSERT_SETPOS(consume(&lex, lexs) == 0, PARSER_NO_CMD, lex.pos);
    PARSER_ASSERT_SETPOS(lex.type == LEX_CMD, PARSER_UNKNWN_CMD, lex.pos);

    switch(lex.value.code)
    {
        #include "../def_syscmd.inc"

        #include "../def_cmd.inc"

        default:
            PARSER_ASSERT_SETPOS(0, PARSER_UNKNWN_CMD, lex.pos);
    }

    return PARSER_NOERR;
}

// #define ARGS_ (stment->args)
// #define CMD_ (stment->cmd)
// static parser_err verify_statement(Statement* stment)
// {
//     assert(stment);
// 
//     parser_err err = PARSER_NOERR;
// 
//     PARSER_ASSERT(CMD_.n_args == expr->args_sz, PARSER_INVALID_ARGS_NUM);
// 
//     if(CMD_.is_sys)
//     {
//         PARSER_ASSERT
//     }
// }

static parser_err statement(Lexems_array* lexs, Statement* stment)
{
    assert(lexs && stment);

    parser_err err = PARSER_NOERR;

    Lexem lex = {};

    err = command(lexs, &stment->cmd);
    if(err)
        return err;

    while(true)
    {
        if(peek(&lex, lexs) == EOF)
            break;

        err = argument(lexs, &stment->args[stment->args_sz]);
        if(err)
            return err;

        stment->args_sz++;

        if(consume(&lex, lexs) == EOF)
            break;

        PARSER_ASSERT_SETPOS(lex.type == LEX_COMMA, PARSER_TRAILING_SYMBOLS, lex.pos);

        PARSER_ASSERT_SETPOS(peek(&lex, lexs) != EOF, PARSER_TRAILING_COMMA, lex.pos);
    }

    //err = verify_statement(stment);
    //PARSER_ASSERT_SETPOS(!err, err);

    return PARSER_NOERR;
}

/////////////////////////////////////////////////////////////////////////////

#define PUT_CMD(CODE)                                                       \
    do                                                                      \
    {                                                                       \
        cmd_t temp = {};                                                    \
        temp.bits.cmd = (CODE);                                             \
                                                                            \
        dst_line[*pos] = temp.byte;                                         \
        *pos += sizeof(cmd_t);                                              \
    } while(0)                                                              \

#define PUT_CMD_WA(CODE, MEMORY, LEX_ARG)                                   \
    do                                                                      \
    {                                                                       \
        cmd_t temp = {};                                                    \
        temp.bits.cmd = (CODE);                                             \
        temp.bits.mem = (MEMORY);                                           \
        if((LEX_ARG).type == LEX_REGISTER)                                  \
            temp.bits.reg = ARG_REGISTER;                                   \
        else                                                                \
            temp.bits.reg = ARG_NOTREGISTER;                                \
                                                                            \
        dst_line[*pos] = temp.byte;                                         \
        *pos += sizeof(cmd_t);                                              \
                                                                            \
        if((LEX_ARG).type == LEX_REGISTER)                                  \
        {                                                                   \
            memcpy(dst_line + *pos, &(LEX_ARG).value.code, sizeof(val8_t)); \
            *pos += sizeof(val8_t);                                         \
        }                                                                   \
        else if((LEX_ARG).type == LEX_IMMCONST)                             \
        {                                                                   \
            memcpy(dst_line + *pos, &(LEX_ARG).value.num, sizeof(val64_t)); \
            *pos += sizeof(val64_t);                                        \
        }                                                                   \
        else if((LEX_ARG).type == LEX_INSTRPTR)                             \
        {                                                                   \
            memcpy(dst_line + *pos, &(LEX_ARG).value.ip, sizeof(size_t));   \
            *pos += sizeof(size_t);                                         \
        }                                                                   \
        else                                                                \
            assert(0);                                                      \
    } while(0)                                                              \

static void put_evaluated_arg(bin_t* dst_line, size_t* pos, Argument* arg, size_t* arg_iter)
{
    for(size_t lex_iter = 0; lex_iter < arg->lexs_sz; lex_iter++)
    {
        switch(arg->lexs[lex_iter].type)
        {
            case LEX_IMMCONST : case LEX_REGISTER :
                PUT_CMD_WA(SYSCMD_push, MEM_NOT_RAM, arg->lexs[lex_iter]);
                break;
            case LEX_ADD:
                PUT_CMD(SYSCMD_add);
                break;
            case LEX_SUB:
                PUT_CMD(SYSCMD_sub);
                break;
            case LEX_MUL:
                PUT_CMD(SYSCMD_mul);
                break;
            
        }
    }

    Lexem d$x_lex = {};
    d$x_lex.type  = LEX_REGISTER;
    d$x_lex.value.code = REG_dax + *arg_iter;

    PUT_CMD_WA(SYSCMD_pop, MEM_NOT_RAM, d$x_lex); // pop argument to d$x (dark register)

    if(arg->memory == MEM_RAM)  //if access to RAM
    {
        PUT_CMD_WA(SYSCMD_push, MEM_RAM, d$x_lex); // push RAM[d$x]

        PUT_CMD_WA(SYSCMD_pop, MEM_NOT_RAM, d$x_lex); // pop to d$x
    }
}

static parser_err put_statement(bin_t* dst_line, size_t* pos, Statement* stment)
{
    assert(dst_line && pos && stment); 
    
    if(stment->args_sz == 0) // cmd without args
    {
        PUT_CMD(stment->cmd.code);
    }

    else if(stment->cmd.is_sys) // Sys commands work with registers and memory directly, argument is one literal and can't be evaluated
    {
        PUT_CMD_WA(stment->cmd.code, stment->args[0].memory, stment->args[0].lexs[0]);
    }
    else
    {
        for(size_t arg_iter = 0; arg_iter < stment->args_sz; arg_iter++)
        {
            put_evaluated_arg(dst_line, pos, &stment->args[arg_iter], &arg_iter); // evaluates argument, pushes it to dax...dzx
        }

        PUT_CMD(stment->cmd.code); // implicitly gets argument from dax...dzx
    }

    return PARSER_NOERR;
}

/////////////////////////////////////////////////////////////////////////////

static parser_err parse_line(bin_t* bin_line, size_t* bin_line_sz, size_t* ip, char* txt)
{
    parser_err err = PARSER_NOERR;

    Lexems_array lexs = {};

    Statement stment = {};

    Label lbl = {};

    lexer(&lexs, txt);

    if((err = label(&lexs, &lbl, *ip)) == 0) //if line is label
    {
        if(ITERATION == 1)
            return add_label(&lbl);
        if(ITERATION == 2)
            return get_label(&lbl);
    }
    else if((err = statement(&lexs, &stment)) == 0) //if line is statement
    {
        err = put_statement(bin_line, bin_line_sz, &stment);
        
        *ip += *bin_line_sz;

        if(ITERATION == 2)
            list_statement(&stment, txt);

        return err;
    }
    else
    {
        return err;
    }
    
    return PARSER_NOERR;
}

parser_err parser(Binary* bin, const Text* txt, const char infilename[])
{
    parser_dump_init();
    parser_err err = PARSER_NOERR;
    parser_err save_err = PARSER_NOERR;

    size_t ip = 0;
    size_t n_line = txt->index_arr_size;

    for(ITERATION = 1; ITERATION <= 2; ITERATION++)
    {
        ip = 0;

        for(size_t line = 0; line < n_line; line++)
        {
            size_t bin_line_sz = 0;
            bin_t bin_line[BIN_LINE_CAP] = {};

            char* txt_line = txt->index_arr[line].begin;

            err = parse_line(bin_line, &bin_line_sz, &ip, txt_line);
            if(err && ITERATION == 2)
            {
                save_err = err;
                PARSER_LOG_ERR(err, line, txt_line, infilename);
            }

            if(ITERATION == 2)
            {
                list_line(bin_line, bin_line_sz, txt_line, line, ip);

                binary_sread(bin, bin_line, bin_line_sz);
            }
        }

        if(ITERATION == 1)
            list_labels(labels_array());
    }
    
    return save_err;
}
