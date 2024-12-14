#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "logger.h"

typedef struct
{
    native_timestamp_t timestamp;
    char msg[128];
} t_timeEntry;

static t_timeEntry log_timing_list[1024];

static int timeEntryIndex = 0;

void native_timediff_sprintf(char * diff_string, size_t diff_string_length, native_timediff_t timediff)
{
    if(diff_string)
    {
        snprintf(diff_string, diff_string_length, "+%ld.%03ld ms", (timediff / 1000), (timediff %1000));
    }
}

void log_msg(const char * function, char *format, ...)
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
    fprintf(stdout, "%s %-16s:%s\n", time_buf, function, buf);
    fflush(stdout);
}

native_timestamp_t log_get_timestamp()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return ((tv.tv_sec % 100000) * 1000000) + tv.tv_usec;
}

void log_timer_begin(const char *msg)
{
    timeEntryIndex = 0;
    log_time_mark(msg);
}

void log_time_mark(const char *msg)
{
    struct timeval tv;
    struct timezone tz;
    if(timeEntryIndex < 1024)
    {
        gettimeofday(&tv, &tz);
        log_timing_list[timeEntryIndex].timestamp = log_get_timestamp();
        strncpy(log_timing_list[timeEntryIndex].msg, msg, 127);
        timeEntryIndex++;
    }
}

void log_time_display()
{
    native_timestamp_t previous_timestamp;
    native_timestamp_t start_timestamp;
    native_timestamp_t diff_begin;
    native_timediff_t  diff_last;
    if(timeEntryIndex > 0)
    {
        // Print first item.
        printf("Timestamp : Diff begin : Diff Last  : Note\n");
        printf("%ld :    :      :%s\n", log_timing_list[0].timestamp, log_timing_list[0].msg);
        previous_timestamp = log_timing_list[0].timestamp;
        start_timestamp    = log_timing_list[0].timestamp;
    }

    for(int count = 1; count < timeEntryIndex; count++)
    {
        diff_begin = log_timing_list[count].timestamp - start_timestamp;
        diff_last = log_timing_list[count].timestamp - previous_timestamp;
        printf("%ld.%03ld : +%ld.%03ld ms : (+%ld.%03ld ms) : %s\n", (log_timing_list[count].timestamp / 1000), (log_timing_list[count].timestamp % 1000),
                                                               (diff_begin / 1000), (diff_begin %1000), 
                                                               (diff_last / 1000), (diff_last %1000), 
                                                               log_timing_list[count].msg);
        previous_timestamp = log_timing_list[count].timestamp;
    }
}