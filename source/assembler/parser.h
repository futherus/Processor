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

enum parser_err 
{
    PARSER_NOERR             = 0,
    PARSER_UNKNWN_CMD        = 1,
    PARSER_ARGS_OVRFLW       = 2,
    PARSER_INVALID_ARGS      = 3,
    PARSER_BINLINE_OVRFLW    = 4,
    PARSER_LINE_FAIL         = 5,
    PARSER_READ_FAIL         = 6,
    PARSER_WRITE_FAIL        = 7,
    PARSER_UNEXPCTD_ARGS     = 8,
};

parser_err parser(Binary* bin, const Text* txt);

#endif // TRANSLATOR_H