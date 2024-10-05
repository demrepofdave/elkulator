#ifndef _CALLBACK_HANDLERS_H
#define _CALLBACK_HANDLERS_H

#include <stdint.h>

typedef struct 
{
    //void (*handler_load_state)(const char * filename);
    //void (*handler_save_state)(const char * filename);
    void (*handler_load_tape)(const char * filename);
} CallbackHandlers;

extern CallbackHandlers callback_handlers;

#endif // _CALLBACK_HANDLERS_H