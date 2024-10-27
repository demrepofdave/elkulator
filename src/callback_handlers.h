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

    // Disc menu callback handlers
    callback_filename_t handler_load_disc0_2; // TODO: will be one handler not two in future.
    callback_filename_t handler_load_disc1_3; // TODO: will be one handler not two in future.

    // Tape menu callback handlers
    callback_filename_t handler_load_tape;
    callback_void_t rewind_tape;
    callback_void_t eject_tape;

    // ROM menu callback handlers
    callback_filename_t handler_load_cart1;
    callback_filename_t handler_load_cart2;
    callback_void_t     handle_unload_carts;

    // Misc menu callback handlers
    callback_filename_t handle_screenshot;

} callback_handlers_t;

extern callback_handlers_t callback_handlers;

#endif // _CALLBACK_HANDLERS_H