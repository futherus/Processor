#include <stdio.h>
#include <assert.h>

#include "lexer.h"
#include "../binary/Binary.h"
#include "../hash.h"

static void clean_whitespaces_(char* txt, size_t* pos)
{
    assert(txt && pos);

    while(isspace(txt[*pos]))
        (*pos)++;
}

static void wordlen_(char* ptr, int* n_read)
{
    assert(ptr && n_read);

    while(isalnum(ptr[*n_read]) || ptr[*n_read] == '_')
        (*n_read)++;
}


#define CUR_LEXEM_ (lexs->lexems[lexs->lexems_sz])

#define DEF_REG(TXT, HASH)                    \
    case (HASH):                              \
        CUR_LEXEM_.type       = LEX_REGISTER; \
        CUR_LEXEM_.value.code = REG_##TXT;    \
        lexs->lexems_sz++;                    \
        pos += n_read;                        \
        break;                                \

#define DEF_SYSCMD(TXT, HASH, N_ARGS, CODE)   \
    case (HASH):                              \
        CUR_LEXEM_.type       = LEX_CMD;      \
        CUR_LEXEM_.value.code = SYSCMD_##TXT; \
        lexs->lexems_sz++;                    \
        pos += n_read;                        \
        break;                                \

#define DEF_CMD(TXT, HASH, N_ARGS, CODE)      \
    case (HASH):                              \
        CUR_LEXEM_.type       = LEX_CMD;      \
        CUR_LEXEM_.value.code = CMD_##TXT;    \
        lexs->lexems_sz++;                    \
        pos += n_read;                        \
        break;                                \

#define DEF_SYMB(SYMB, TYPE)                  \
    case (SYMB):                              \
        CUR_LEXEM_.type = LEX_##TYPE;         \
        lexs->lexems_sz++;                    \
        pos += 1;                             \
        continue;                             \

    
int lexer(Lexems_array* lexs, char* txt)
{
    size_t pos = 0;

    while(txt[pos] != 0)
    {
        clean_whitespaces_(txt, &pos);

        CUR_LEXEM_.pos = pos;
        
        int n_read = 0;

        if(sscanf(txt + pos, "%lg%n", &CUR_LEXEM_.value.num, &n_read) > 0)
        {
            CUR_LEXEM_.type = LEX_IMMCONST;
            lexs->lexems_sz++;
            pos += n_read;
        }
        else 
        {
            switch(txt[pos])
            {
                DEF_SYMB('+', ADD)
                DEF_SYMB('-', SUB)
                DEF_SYMB('*', MUL)
                DEF_SYMB('(', LRPAR)
                DEF_SYMB(')', RRPAR)
                DEF_SYMB('[', LQPAR)
                DEF_SYMB(']', RQPAR)
                DEF_SYMB(',', COMMA)
                DEF_SYMB(':', COLON)

                default:
                { /* fallthrough */ }
            }

            wordlen_(txt + pos, &n_read);

            if(n_read == 0)
                return -1;     // UNKNOWN SYMBOLS
                
            uint64_t hash = qhashfnv1_64(txt + pos, n_read);

            switch(hash)
            {
                #include "../def_syscmd.inc"

                #include "../def_cmd.inc"

                #include "../def_reg.inc"

                default:
                    CUR_LEXEM_.type = LEX_WORD;
                    CUR_LEXEM_.value.hash = hash;
                    lexs->lexems_sz++;
                    pos += n_read;
                    break;
            }
        }
    }

    return 0;
}

int consume(Lexem* lex, Lexems_array* lexs)
{
    if(lexs->lexems_ptr == lexs->lexems_sz)
        return -1;

    *lex = lexs->lexems[lexs->lexems_ptr];
    lexs->lexems_ptr++;

    return 0;
}

int peek(Lexem* lex, Lexems_array* lexs)
{
    if(lexs->lexems_ptr == lexs->lexems_sz)
        return -1;

    *lex = lexs->lexems[lexs->lexems_ptr];

    return 0;
}
