#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "log.h"

static FILE* log_stream_();

static void close_logstream_()
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

        stream = fopen(PROCESSOR_LOGFILE, "w");

        if(stream)
            atexit(&close_logstream_);
        else
            fprintf(stderr, "Cannot open file for log %s", strerror(errno));
    }

    return stream;
}

#define PRINT(format, ...) fprintf(stream, (format), ##__VA_ARGS__) 

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
