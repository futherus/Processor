#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

const size_t LEXEMS_ARRAY_CAP = 50;

enum Lexem_types
{
    LEX_NOTYPE = 0,
    LEX_CMD,
    LEX_REGISTER,
    LEX_IMMCONST,
    LEX_INSTRPTR,
    LEX_WORD,

    LEX_ADD,
    LEX_SUB,
    LEX_MUL,
    LEX_LRPAR,
    LEX_RRPAR,
    LEX_LQPAR,
    LEX_RQPAR,
    LEX_COMMA,
    LEX_COLON,
};

struct Lexem 
{
    unsigned char type = LEX_NOTYPE;
    size_t pos = 0;

    union Value
    {
        double         num;  //< for immensive constants
        unsigned char code;  //< for reserved names
        uint64_t      hash;  //< for string   names
        size_t          ip;  //< for instruction pointers
    } value;
};

struct Lexems_array
{
    size_t lexems_sz = 0;
    size_t lexems_ptr = 0;

    Lexem lexems[LEXEMS_ARRAY_CAP];
};

int lexer(Lexems_array* lexs, char* txt);

int consume(Lexem*, Lexems_array*);

int peek(Lexem*, Lexems_array*);

#endif // LEXER_H
