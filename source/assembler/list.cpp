#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

static FILE* list_stream_();

static void close_liststream_()
{
    if(fclose(list_stream_()) != 0)
        fprintf(stderr, "File for listing cannot be closed %s", strerror(errno));
}

static FILE* list_stream_()
{
    static int first_call = 1;
    static FILE* stream = nullptr;

    if(first_call)
    {
        first_call = 0;

        stream = fopen(LISTFILE, "w");

        if(stream)
            atexit(&close_liststream_);
        else
            fprintf(stderr, "Cannot open file for listing %s", strerror(errno));
    }

    return stream;
}

#define PRINT(format, ...) fprintf(stream, (format), ##__VA_ARGS__) 

void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t line)
{
    static int first_call = 1;
    FILE* stream = list_stream_();
    if(!stream)
        return;

    if(first_call)
    {
        PRINT("|Line|Command|Binary\n");
        first_call = 0;
    }

    PRINT("|%4llu|", line);
    PRINT("%-7s|", txt_line);
    for(size_t iter = 0; iter < bin_line_sz; iter++)
    {
        PRINT("%02hx ", bin_line[iter]);
    }
    PRINT("\n");
}
