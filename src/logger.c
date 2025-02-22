#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "logger.h"
#include "host_abstraction_layer/native_time.h"

log_level_t current_log_level = LOG_ERROR;

const char * log_level_string[MAX_LOG_ENTRIES] = 
{
    "LOG-ERROR",
    "LOG-WARNING",
    "LOG-INFO",
    "LOG-DEBUG"
};

void log_set_level(log_level_t log_level)
{
    current_log_level = log_level;
}

void log_msg(log_level_t log_level, const char * function, char *format, ...)
{
    if(log_level <= current_log_level && log_level < MAX_LOG_ENTRIES)
    {
        char buf[1024];
        char time_buf[128];
        struct timeval tv;
        struct timezone tz;
        size_t offset = 0;

        gettimeofday(&tv,&tz);
        time_t time_val = tv.tv_sec;
        struct tm *local = localtime(&time_val);
        offset = strftime(time_buf, 128, "%Y-%b-%d %H:%M:%S", local);
        if(offset > 0)
        {
            // Time was ok, append msecs.
            snprintf(time_buf+offset, 128-offset, ".%03ld", tv.tv_usec/1000);
        }
        va_list ap;
        va_start(ap, format);
        vsprintf(buf, format, ap);
        va_end(ap);
        fprintf(stdout, "%s :%-11s: %-16s:%s\n", time_buf, log_level_string[log_level], function, buf);
        fflush(stdout);
    }
}
