#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "logger.h"

void log_msg(const char * function, char *format, ...)
{
    char buf[1024];
    char time_buf[128];
    struct timeval tv;
    struct timezone tz;
    size_t offset = 0;

    gettimeofday(&tv,&tz);
    struct tm *local = localtime(&tv.tv_sec);
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
    fprintf(stdout, "%s %-16s:%s\n", time_buf, function, buf);
    //fputs(buf,rlog);
    fflush(stdout);
}