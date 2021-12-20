#include <processthreadsapi.h>

inline double get_cpu_time()
{
    FILETIME creation, exit, kernel, user;
    if (GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user) != 0)
    {
        return (double)(user.dwLowDateTime |
               ((unsigned long long)user.dwHighDateTime << 32)) * 0.0000001;
    }
    else
    {
        return 0;
    }
}
