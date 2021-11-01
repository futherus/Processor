#ifndef DUMP_SYSTEM_H
#define DUMP_SYSTEM_H

#include <stdio.h>
#include <string.h>
#include <time.h>

const int DUMPSYSTEM_DEFAULT_STREAM = 1;

#define CHECK$(CONDITION__, ERROR__, ACTION__)                                              \
    if(CONDITION__)                                                                         \
    {                                                                                       \
        fprintf(stderr, "ERROR: %s", #ERROR__);                                             \
                                                                                            \
        FILE* stream = dumpsystem_get_stream_(DUMPSYSTEM_DEFAULT_STREAM);                   \
        if(stream != nullptr)                                                               \
        {                                                                                   \
            fprintf(stream, "ERROR: %s\n"                                                   \
                    "at %s:%d:%s\n", #ERROR__, __FILE__, __LINE__, __PRETTY_FUNCTION__);    \
            fflush(stream);                                                                 \
        }                                                                                   \
                                                                                            \
        ACTION__                                                                            \
    }                                                                                       \

#define PASS$(CONDITION__, ACTION__)                                                        \
    if(CONDITION__)                                                                         \
    {                                                                                       \
        FILE* stream = dumpsystem_get_stream_(DUMPSYSTEM_DEFAULT_STREAM);                   \
        if(stream != nullptr)                                                               \
        {                                                                                   \
            fprintf(stream, "at %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);      \
            fflush(stream);                                                                 \
        }                                                                                   \
                                                                                            \
        ACTION__                                                                            \
    }                                                                                       \

#define LOG$(MESSAGE__, ...)                                                                \
    {                                                                                       \
        FILE* stream = dumpsystem_get_stream_(DUMPSYSTEM_DEFAULT_STREAM);                   \
        if(stream != nullptr)                                                               \
        {                                                                                   \
            fprintf(stream, "LOG: \"" MESSAGE__ "\"\n"                                      \
                   "at %s:%d:%s\n", __VA_ARGS__, __FILE__, __LINE__, __PRETTY_FUNCTION__);  \
            fflush(stream);                                                                 \
        }                                                                                   \
    }                                                                                       \

#define SET_FILE(DESCRNAME, filename)   \
    DESCR_##DESCRNAME,                  \

enum dumpsystem_descriptors_
{
    DESCR_stderr = 0,

    #include "dumpsystem_files.inc"

    DESCR_END
};
#undef SET_FILE

#define dumpsystem_get_stream(DESCRNAME) dumpsystem_get_stream_(DESCR_##DESCRNAME)

FILE* dumpsystem_get_stream_(int descriptor);

#endif // DUMP_SYSTEM_H
