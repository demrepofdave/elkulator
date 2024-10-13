#ifndef _CALLBACK_HANDLERS_H
#define _CALLBACK_HANDLERS_H

#include <stdint.h>

typedef void (*callback_filename_t)(const char * filename);
typedef void (*callback_void_t)(void);


typedef struct 
{
    // File menu callback handlers
    callback_filename_t handler_load_state;
    callback_filename_t handler_save_state;

    // Tape menu callback handlers
    callback_filename_t handler_load_tape;
    callback_void_t rewind_tape;
    callback_void_t eject_tape;

} callback_handlers_t;

extern callback_handlers_t callback_handlers;

#endif // _CALLBACK_HANDLERS_H