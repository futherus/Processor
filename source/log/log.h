#ifndef LOG_H
#define LOG_H

const char PROCESSOR_LOGFILE[] = "log.txt";

void mem_dump(void* src_ptr, const char msg[]);

#define L$(str) log_(#str, __FILE__, __func__, __LINE__);                   \
                str                                                         \

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