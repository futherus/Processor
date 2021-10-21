#ifndef DEBUG_H
#define DEBUG_H

#include "../binary/Binary.h"

const char ASM_LISTFILE[] = "asm_list.txt";

void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t line);

#endif // DEBUG_H