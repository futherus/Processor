#ifndef LOG_H
#define LOG_H

extern const char LOGFILE[];

#define L$(str) log_(#str, __FILE__, __func__, __LINE__);                   \
                str                                                         \

#define DI$(uint64)                                                         \
    do                                                                      \
    {                                                                       \
        char temp[1000] = #uint64;                                          \
        sprintf(temp + sizeof(#uint64) - 1, ": %llu", uint64);              \
        log_(temp, __FILE__, __func__, __LINE__);                           \
    } while(0)                                                              \

#define ASSERT(condition, err)                                              \
    do                                                                      \
    {                                                                       \
        if(!(condition))                                                    \
        {                                                                   \
            log_("ERROR: " #err , __FILE__, __func__, __LINE__);            \
            return (err);                                                   \
        }                                                                   \
    } while(0)                                                              \

void log_(const char str[], const char file[], const char func[], int line);

#endif // LOG_H