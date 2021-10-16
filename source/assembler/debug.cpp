#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

static FILE* list_stream_();
static FILE* log_stream_();

static void close_listfile_()
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
            atexit(&close_listfile_);
        else
            fprintf(stderr, "Cannot open file for listing %s", strerror(errno));
    }

    return stream;
}

static void close_logfile_()
{
    if(fclose(log_stream_()) != 0)
        fprintf(stderr, "File for log cannot be closed %s", strerror(errno));
}

static FILE* log_stream_()
{
    static int first_call = 1;
    static FILE* stream = nullptr;

    if(first_call)
    {
        first_call = 0;

        stream = fopen(LOGFILE, "w");

        if(stream)
            atexit(&close_logfile_);
        else
            fprintf(stderr, "Cannot open file for log %s", strerror(errno));
    }

    return stream;
}

#define PRINT(format, ...) fprintf(stream, (format), ##__VA_ARGS__) 

void list_line(bin_t* bin_line, size_t bin_line_sz, char* txt_line, size_t txt_line_sz, size_t line)
{
    static first_call = 1;
    if(first_call)
    {
        
    }
    FILE* stream = list_stream_();
    if(!stream)
        return;

    PRINT("|Line|Command|Binary\n");
    PRINT("|%4llu|", line, txt_line);
    PRINT("%-7s|", txt_line);
    for(size_t iter = 0; iter < bin_line_sz; iter++)
    {
        PRINT("%llx ", bin_line[iter]);
        //PRINT("%llx")
    }
    PRINT("\n");
}

void log_(const char str[], const char file[], const char func[], int line)
{
    FILE* stream = log_stream_();
    if(!stream)
        return;

    PRINT("%-20s||%4d|| %-90s || %-70s || %s %s\n", func, line, str, file, __DATE__, __TIME__);
}

void mem_dump(void* src_ptr, const char msg[])
{
        uint32_t* ptr = (uint32_t*) src_ptr;

        fprintf(stderr, "%s", msg);
        fprintf(stderr, "-----------------------------------------------");
        for(long iter = -64; iter < 64; iter++)
        {
                if(iter == 0)
                        fprintf(stderr, "\n");
                if(iter % 4 == 0)
                        fprintf(stderr, "\n" "%p:", ptr + iter);

                fprintf(stderr, "|%08x|", *(ptr + iter));
        }
        fprintf(stderr, "\n-----------------------------------------------\n\n");
}
