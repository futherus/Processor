#include "dumpsystem.h"
#include <stdio.h>
#include <stdlib.h>

static void dumpsystem_close_streams_();

#define SET_FILE(DESCRNAME, FILENAME)                               \
    case DESCR_##DESCRNAME:                                         \
    {                                                               \
        static FILE* STREAM_##DESCRNAME = nullptr;                  \
                                                                    \
        if(STREAM_##DESCRNAME == nullptr)                           \
        {                                                           \
            STREAM_##DESCRNAME = fopen(FILENAME, "w");              \
                                                                    \
            if(!STREAM_##DESCRNAME)                                 \
                perror("Cannot open `" #FILENAME "` for dump");     \
        }                                                           \
                                                                    \
        return STREAM_##DESCRNAME;                                  \
    }                                                               \

FILE* dumpsystem_get_stream_(int descriptor)
{
    switch(descriptor)
    {
        case DESCR_stderr:
            return stderr;

        #include "dumpsystem_files.inc"

        default:
            return stderr;
    }
    
    atexit(&dumpsystem_close_streams_);
}
#undef SET_FILE

static void dumpsystem_close_streams_()
{
    for(int iter = DESCR_stderr + 1; iter < DESCR_END; iter++)
    {
        FILE* temp = dumpsystem_get_stream_(iter);
        if(temp)
            if(fclose(temp) != 0)
                perror("One of dumpfiles closed unsuccesfully");
    }
}
