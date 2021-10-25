#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdint.h>
#include "../binary/Binary.h"
#include "text/Text.h"

const size_t ARGS_CAP = 0x6;
const size_t LITS_CAP = 0x6;

const val8_t REG_dax = 128; 

struct Command
{
    val8_t value_8b = 0;  ///->value8_b ?

    spec_t   is_sys = 0;
    uint64_t hash   = 0;
    size_t   n_args = 0;
};

struct Arg_literal
{
    val64_t value_64b = 0;
    val8_t  value_8b  = 0;

    int    sign = 0;
    spec_t type = ARG_ERRTYPE;
};

struct Argument
{
    Arg_literal literals[LITS_CAP] = {};
    size_t literals_sz = 0;

    spec_t memory = MEM_NOT_RAM;
};

struct Expression 
{
    Command cmd = {};
    
    Argument args[ARGS_CAP] = {0};
    size_t args_sz = 0;
};

/// Capacity of text line buffer 
const size_t TXT_CMD_CAP = 0x200;

enum preprocessor_err 
{
    PREPROCESSOR_NOERR               =  0,
    PREPROCESSOR_MISSING_SIGN        =  1,
    PREPROCESSOR_SIGN_DUPLICATE      =  2,
    PREPROCESSOR_MISSING_PARENTHESIS =  3,
    PREPROCESSOR_TRAILING_SYMBOLS    =  4,
    PREPROCESSOR_TRAILING_COMMA      =  5,
    PREPROCESSOR_INVALID_ARG         =  6,
    PREPROCESSOR_UNKNWN_ARG          =  7,
    PREPROCESSOR_INVALID_ARGS_NUM    =  8,
    PREPROCESSOR_LITS_FOR_SYS        =  9,
    PREPROCESSOR_SIGN_FOR_SYS        = 10,
    PREPROCESSOR_IMMCONST_FOR_POP    = 11,
    PREPROCESSOR_UNKNWN_CMD          = 12,
};

struct Preprocessor_errstruct
{
    size_t line = 0;
    size_t pos = 0;
    char txt[TXT_CMD_CAP] = "";

    preprocessor_err errnum = PREPROCESSOR_NOERR;
};

Preprocessor_errstruct* preprocessor_errstruct();

preprocessor_err preprocessor(Binary* bin, const Text* txt);

#endif // PREPROCESSOR_H