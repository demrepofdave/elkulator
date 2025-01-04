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


#ifndef _NATIVE_TIME_H
#define _NATIVE_TIME_H

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdio.h>

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/


/******************************************************************************
* Typedefs
*******************************************************************************/

// The following tyedefs are interchangeable, however have two types allow 
// clearer intent in variable naming.

typedef long native_timestamp_t;
typedef long native_timediff_t;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

native_timediff_t native_cumulative_time_adjust(native_timediff_t offset, native_timediff_t cumulative_total, native_timediff_t current_timediff);
void native_timediff_sprintf(char * diff_string, size_t diff_string_length, native_timediff_t timediff);
native_timestamp_t native_timestamp_get();

void log_timer_begin();
void log_time_mark(const char *msg);
void log_time_display();


#endif // _NATIVE_TIME_H