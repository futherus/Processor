#ifndef PREPROCESSOR_LIST_H
#define PREPROCESSOR_LIST_H

#include "preprocessor.h"
#include "../binary/Binary.h"

const char BINLINE_LISTFILE[]      = "asm_logs/binline_list.txt";
const char PREPROCESSOR_LISTFILE[] = "asm_logs/expression_list.txt";

void log_err(const char infile_name[]);

void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t line);

void list_expr(Expression* expr, char* txt);

#endif // PREPROCESSOR_LIST_H