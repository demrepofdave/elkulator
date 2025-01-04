/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * native_time.h
 * 
 * A number of time functions useful for human readable timestamps
 * and also for debugging code performance issues.
 * 
 */


/******************************************************************************
* Include files
*******************************************************************************/
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "host_abstraction_layer/native_time.h"


/******************************************************************************
* Preprocessor Macros
*******************************************************************************/


/******************************************************************************
* Typedefs
*******************************************************************************/

typedef struct
{
    native_timestamp_t timestamp;
    char msg[128];
} t_timeEntry;

static t_timeEntry log_timing_list[1024];

static int timeEntryIndex = 0;


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

native_timestamp_t native_timestamp_get()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return ((tv.tv_sec % 100000) * 1000000) + tv.tv_usec;
}

native_timediff_t native_cumulative_time_adjust(native_timediff_t offset, native_timediff_t cumulative_total, native_timediff_t current_timediff)
{
    native_timediff_t adjusted_timediff = current_timediff - offset;
    native_timediff_t new_cumulative_total = cumulative_total + adjusted_timediff;
    if(new_cumulative_total < 0)
    {
        new_cumulative_total = 0;
    }
    return(new_cumulative_total);
}

void native_timediff_sprintf(char * diff_string, size_t diff_string_length, native_timediff_t timediff)
{
    if(diff_string)
    {
        snprintf(diff_string, diff_string_length, "+%ld.%03ld ms", (timediff / 1000), (timediff %1000));
    }
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
        log_timing_list[timeEntryIndex].timestamp = native_timestamp_get();
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