/* Main code for handling events */

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "common/event_handler.h"
#include "logger.h"
#include "common/video.h"
#include "menu_internal.h"
#include "keyboard_internal.h"


// Event variables
ALLEGRO_EVENT_QUEUE *queue;

// TODO: Remove externs eventually
extern void key_down_event(const ALLEGRO_EVENT *event);
extern void key_up_event(const ALLEGRO_EVENT *event);
extern void key_char_event(const ALLEGRO_EVENT *event);

// Public functions.

bool event_init_queue()
{
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

// Main event handling Code
uint32_t event_await()
{
    ALLEGRO_EVENT event;
    bool quitting = false;
    bool timer_triggered = false;
    uint32_t elkEvent = 0;
    while (!elkEvent) 
    {
        al_wait_for_event(queue, &event);
        switch(event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                log_debug("event_await: event display close - quitting\n");
                elkEvent = ELK_EVENT_EXIT;
                break;

            case ALLEGRO_EVENT_TIMER:
                //log_debug("event_await: event timer triggered\n");
                elkEvent = ELK_EVENT_TIMER_TRIGGERED;
                break;

            case ALLEGRO_EVENT_MENU_CLICK:
                video_stop_timer();
                log_debug("event_await: event Menu click\n");
                elkEvent = menu_handle_event(&event);
                video_start_timer();
                break;

            // Keyboard handling.
            case ALLEGRO_EVENT_KEY_DOWN:
            case ALLEGRO_EVENT_KEY_CHAR:
            case ALLEGRO_EVENT_KEY_UP:
                elkEvent = keyboard_handle_event(&event);
                break;

            // Mouse handling
            case ALLEGRO_EVENT_MOUSE_AXES:
                break;

            case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                break;

            case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
                break;

            //case ALLEGRO_EVENT_DISPLAY_RESIZE:
            //    video_update_window_size(&event);
            //    break;
            
            default:
                log_debug("event_await: event %d detected\n", event.type);
                break;        }
    }
    if(elkEvent & ELK_EVENT_TIMER_TRIGGERED)
    {
        //log_debug("timer_triggered\n");
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