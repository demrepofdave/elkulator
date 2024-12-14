#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>

// Quick and dirty implementation of logger messages to the screen.

#define log_fatal(f_, ...) fprintf(stderr,(f_), ##__VA_ARGS__)      // TODO: stderr? to file?
#define log_debug(f_, ...) log_msg(__FUNCTION__,(f_), ##__VA_ARGS__) // TODO: stdout? to file?

// The following tyedefs are interchangeable,
// however have two types allow clearer
// intent in variable naming.

typedef long native_timestamp_t;
typedef long native_timediff_t;

void native_timediff_sprintf(char * diff_string, size_t diff_string_length, native_timediff_t timediff);
native_timestamp_t log_get_timestamp();
void log_msg(const char * function, char *format, ...);
void log_timer_begin();
void log_time_mark(const char *msg);
void log_time_display();
#endif // _LOGGER_H