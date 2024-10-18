#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>

// Quick and dirty implementation of logger messages to the screen.

#define log_fatal(f_, ...) fprintf(stderr,(f_), ##__VA_ARGS__)      // TODO: stderr? to file?
#define log_debug(f_, ...) log_msg(__FUNCTION__,(f_), ##__VA_ARGS__) // TODO: stdout? to file?

void log_msg(const char * function, char *format, ...);

#endif // _LOGGER_H