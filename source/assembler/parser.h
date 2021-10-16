#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdint.h>
#include "../binary/Binary.h"
#include "text/Text.h"

const char IN[]  = "in";
const char OUT[] = "out";
const char ADD[] = "add";
const char MUL[] = "mul";
const char HAL[] = "hal";
const char PUSH[] = "push";

/// Capacity of text line buffer 
const size_t TXT_CMD_CAP = 0x200;

/// Capacity of binary line buffer
const size_t BIN_LINE_CAP = 0x10;

enum parser_err 
{
    PARSER_NOERR             = 0,
    PARSER_UNKNWN_CMD        = 1,
    PARSER_ARGS_OVRFLW       = 2,
    PARSER_UNEXPCTD_ARGS     = 3,
    PARSER_BINLINE_OVRFLW    = 4,
    PARSER_LINE_FAIL         = 5,
    PARSER_CMD_FAIL          = 6,
    PARSER_PUT_FAIL          = 7,
};

parser_err parser(Binary* bin, const Text* txt);

#endif // TRANSLATOR_H