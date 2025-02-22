#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>

// Quick and simple implementation of logger messages to the screen.

typedef enum
{
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    MAX_LOG_ENTRIES
} log_level_t;

#define log_fatal(f_, ...)   fprintf(stderr,(f_), ##__VA_ARGS__)      // TODO: stderr? to file?
#define log_error(f_, ...)   log_msg(LOG_ERROR, __FUNCTION__,(f_), ##__VA_ARGS__) // TODO: stdout? to file?
#define log_warning(f_, ...) log_msg(LOG_WARNING, __FUNCTION__,(f_), ##__VA_ARGS__) // TODO: stdout? to file?
#define log_info(f_, ...)    log_msg(LOG_INFO, __FUNCTION__,(f_), ##__VA_ARGS__) // TODO: stdout? to file?
#define log_debug(f_, ...)   log_msg(LOG_DEBUG, __FUNCTION__,(f_), ##__VA_ARGS__) // TODO: stdout? to file?

void log_set_level(log_level_t log_level);
void log_msg(log_level_t log_level, const char * function, char *format, ...);

#endif // _LOGGER_H