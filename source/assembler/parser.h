#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "../binary/Binary.h"
#include "text/Text.h"
#include "lexer.h"

const size_t ARGS_CAP = 0x6;
const size_t LEXS_CAP = 0x6;
const size_t LBLS_CAP = 0x20;

struct Command
{
    val8_t   code   = 0;

    spec_t   is_sys = 0;
    uint64_t hash   = 0;
    size_t   n_args = 0;
};

struct Argument
{
    Lexem lexs[LEXS_CAP] = {};
    size_t lexs_sz = 0;

    spec_t memory = MEM_NOT_RAM;
};

struct Statement
{
    Command cmd = {};
    
    Argument args[ARGS_CAP] = {0};
    size_t args_sz = 0;
};

struct Label
{
    size_t   ip   = 0;
    uint64_t hash = 0;
};

struct Labels_array
{
    Label  labels[LBLS_CAP] = {};

    size_t labels_sz = 0;
};

/// Capacity of text line buffer 
const size_t TXT_CMD_CAP = 0x200;

enum parser_err 
{
    PARSER_NOERR               =  0,
    PARSER_MISSING_SIGN        =  1,
    PARSER_SIGN_DUPLICATE      =  2,
    PARSER_MISSING_PARENTHESES =  3,
    PARSER_TRAILING_SYMBOLS    =  4,
    PARSER_TRAILING_COMMA      =  5,
    PARSER_INVALID_ARG         =  6,
    PARSER_UNKNWN_ARG          =  7,
    PARSER_NO_ARG              =  8,
    PARSER_INVALID_ARGS_NUM    =  9,
    PARSER_LITS_FOR_SYS        = 10,
    PARSER_SIGN_FOR_SYS        = 11,
    PARSER_IMMCONST_FOR_POP    = 12,
    PARSER_UNKNWN_CMD          = 13,
    PARSER_NO_CMD              = 14,
    PARSER_NOTLABEL            = 15,
    PARSER_LABEL_REDECL        = 16,
    PARSER_LABEL_NODECL        = 17,
};

struct Parser_errstruct
{
    size_t line = 0;
    size_t pos = 0;
    char txt[TXT_CMD_CAP] = "";
    char infile[FILENAME_MAX];

    parser_err errnum = PARSER_NOERR;
};

Parser_errstruct* parser_errstruct();

parser_err parser(Binary* bin, const Text* txt, const char infilename[]);

#endif // PARSER_H