#ifndef DEBUG_H
#define DEBUG_H

#include "../binary/Binary.h"

void mem_dump(void* src_ptr, const char msg[]);

const char LISTFILE[] = "asm_list.txt";
const char LOGFILE[] = "asm_log.txt";

#define L$(str) log_(#str, __FILE__, __func__, __LINE__);                   \
                str                                                         \

#define ASSERT(condition, err)                                              \
    do                                                                      \
    {                                                                       \
        if(!(condition))                                                    \
        {                                                                   \
            log_("ERROR: " #err , __FILE__, __func__, __LINE__);            \
            return (err);                                                   \
        }                                                                   \
    } while(0)                                                              \

void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t txt_line_sz, size_t line);

void log_(const char str[], const char file[], const char func[], int line);

#endif // DEBUG_H