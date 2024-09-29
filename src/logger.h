#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>

// Quick and dirty implementation of logger messages to the screen.

#define log_fatal(f_, ...) fprintf(stderr,(f_), ##__VA_ARGS__)
#define log_debug(f_, ...) fprintf(stdout,(f_), ##__VA_ARGS__)

#endif // _LOGGER_H