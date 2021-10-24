#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdint.h>
#include "../binary/Binary.h"
#include "text/Text.h"

const size_t ARGS_CAP = 0x8;
const size_t LITS_CAP = 8;

const spec_t ARG_ERRTYPE  = -1;
const spec_t ARG_IMMCONST =  0;
const spec_t ARG_REGISTER =  1;

const spec_t MEMORY_NOT_RAM =  0;
const spec_t MEMORY_RAM   =  1;

const int CMD_out = 2;  //

const uint64_t HASH_push = 12441716782781469529ULL; //
const uint64_t HASH_out  = 15613325803558276075ULL; //

const uint64_t HASH_rax  = 15610255967092878490ULL; //
const uint64_t HASH_rbx  = 15610254867581250391ULL; //
const uint64_t HASH_rcx  = 15610253768069622020ULL; //

const val8_t REG_dax = 255; //

struct Command
{
    val8_t value_8b = 0;  ///->value8_b ?

    uint64_t hash = 0;
    size_t  n_args = 0;
};

struct Arg_literal
{
    val64_t value_64b = 0;
    val8_t  value_8b = 0;

    int    sign = 0;
    spec_t type = ARG_ERRTYPE;
};

struct Argument
{
    Arg_literal literals[LITS_CAP] = {};
    size_t literals_sz   =  0;

    spec_t memory = MEMORY_NOT_RAM;
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
    PREPROCESSOR_LITS_FOR_POP        =  9,
    PREPROCESSOR_UNKNWN_CMD          = 10,
};

struct Preprocessor_errstruct
{
    size_t line = 0;
    size_t pos = 0;

    preprocessor_err errnum = PREPROCESSOR_NOERR;
};

Preprocessor_errstruct* preprocessor_errstruct();


preprocessor_err preprocessor(Binary* bin, const Text* txt);

#endif // PREPROCESSOR_H