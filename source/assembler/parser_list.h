#ifndef PARSER_LIST_H
#define PARSER_LIST_H

#include "parser.h"
#include "../binary/Binary.h"

void parser_dump_init();

void log_err();

void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t line, size_t ip);

void list_statement(Statement* stment, char* txt);

void list_labels(Label_array* lbl_arr);

#endif // PARSER_LIST_H
