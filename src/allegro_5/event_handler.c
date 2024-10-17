/* Main code for handling events */

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "common/event_handler.h"
#include "event_handler_internal.h"
#include "logger.h"
#include "common/video.h"
#include "menu_internal.h"
#include "keyboard_internal.h"


bool register_main_event_handlers();
elk_event_t handle_event_display_close(ALLEGRO_EVENT * event);
elk_event_t handle_event_timer_expiry(ALLEGRO_EVENT * event);
elk_event_t handle_window_resize_event(ALLEGRO_EVENT * event);
elk_event_t handle_null_event(ALLEGRO_EVENT * event);

#define MAX_CALLBACK_EVENT_HANDLERS      128


// Event variables
ALLEGRO_EVENT_QUEUE *queue;

typedef struct 
{
    int event_id;
    callback_event_handler_t handler_function;
} callback_event_handler_list_t;

callback_event_handler_list_t callback_event_handler_list[MAX_CALLBACK_EVENT_HANDLERS];
uint16_t registered_handlers;

// TODO: Remove externs eventually
extern void key_down_event(const ALLEGRO_EVENT *event);
extern void key_up_event(const ALLEGRO_EVENT *event);
extern void key_char_event(const ALLEGRO_EVENT *event);

// Public functions.

bool event_init_queue()
{
    // Initialise all handlers
    registered_handlers = 0;
    register_main_event_handlers();
    queue = al_create_event_queue();
    return(queue != NULL);
}

void event_register_event_source(ALLEGRO_EVENT_SOURCE * event_source)
{
    al_register_event_source(queue, event_source);
}

ALLEGRO_EVENT_QUEUE * event_get_event_queue()
{
    return queue;
}

bool register_event_handler(int id, callback_event_handler_t handler)
{
    bool registered = false;
    if( registered_handlers < MAX_CALLBACK_EVENT_HANDLERS )
    {
        callback_event_handler_list[registered_handlers].event_id = id;
        callback_event_handler_list[registered_handlers].handler_function = handler;
        registered_handlers++;
        registered = true;
    }
    return (registered);
}

bool register_main_event_handlers()
{
    bool result = true;

    result &= register_event_handler(ALLEGRO_EVENT_DISPLAY_CLOSE, handle_event_display_close);
    result &= register_event_handler(ALLEGRO_EVENT_TIMER,         handle_event_timer_expiry);
    result &= register_event_handler(ALLEGRO_EVENT_MENU_CLICK,    menu_handle_event);

    result &= register_event_handler(ALLEGRO_EVENT_KEY_DOWN, keyboard_handle_event);
    result &= register_event_handler(ALLEGRO_EVENT_KEY_CHAR, keyboard_handle_event);
    result &= register_event_handler(ALLEGRO_EVENT_KEY_UP,   keyboard_handle_event);

    // TODO: Events we acknowledge but are not yet handled by specific code.
    result &= register_event_handler(ALLEGRO_EVENT_MOUSE_AXES,          handle_null_event);
    result &= register_event_handler(ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY, handle_null_event);
    result &= register_event_handler(ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY, handle_null_event);
    result &= register_event_handler(ALLEGRO_EVENT_DISPLAY_RESIZE,      handle_window_resize_event);

    return result;
}

// Called when ALLEGRO_EVENT_DISPLAY_RESIZE event is recieved.
elk_event_t handle_window_resize_event(ALLEGRO_EVENT * event)
{
    // if not fullscreen!
    video_update_native_window_size(event->display.width, event->display.height);
    video_clearall();
    // endif
    return 0;
}

// Handler for events we want to acknowledge but are not yet implemented
elk_event_t handle_null_event(ALLEGRO_EVENT * event)
{
    return 0;
}

// Called when ALLEGRO_EVENT_DISPLAY_CLOSE event is recieved.
elk_event_t handle_event_display_close(ALLEGRO_EVENT * event)
{
    return(ELK_EVENT_EXIT);
}

// Called when ALLEGRO_EVENT_TIMER event is recieved.
elk_event_t handle_event_timer_expiry(ALLEGRO_EVENT * event)
{
    return(ELK_EVENT_TIMER_TRIGGERED);
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

// Main event handling Code
uint32_t event_await()
{
    ALLEGRO_EVENT event;
    bool quitting = false;
    bool timer_triggered = false;
    elk_event_t elkEvent = 0;
    while (!elkEvent) 
    {
        al_wait_for_event(queue, &event);

        // Handle main events.
        uint16_t count = 0;
        while(count < registered_handlers && !(elkEvent & ELK_EVENT_HANDLED))
        {
            if(event.type == callback_event_handler_list[count].event_id)
            {
                // Handler for the event found.
                elkEvent = callback_event_handler_list[count].handler_function(&event);
                elkEvent |= ELK_EVENT_HANDLED;
            }
            count++;
        }

        if(!(elkEvent & ELK_EVENT_HANDLED))
        {
            log_debug("event_await: event %d detected\n", event.type);
        }
    }

    if(elkEvent & ELK_EVENT_RESET)
    {
        log_debug("Elk Reset trigger\n");
    }
    if(elkEvent & ELK_EVENT_EXIT)
    {
        log_debug("quitting\n");
    }
    return elkEvent;
}