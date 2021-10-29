#ifndef DUMP_SYSTEM_H
#define DUMP_SYSTEM_H

#include <stdio.h>

#define SET_FILE(DESCRNAME, filename)   \
    DESCR_##DESCRNAME,                  \

enum dumpsystem_descriptors
{
    DESCR_stderr = 0,

    #include "dumpsystem_files.inc"

    DESCR_END
};
#undef SET_FILE

#define dumpsystem_get_stream(DESCRNAME) dumpsystem_get_stream_(DESCR_##DESCRNAME)

FILE* dumpsystem_get_stream_(int descriptor);

#endif // DUMP_SYSTEM_H