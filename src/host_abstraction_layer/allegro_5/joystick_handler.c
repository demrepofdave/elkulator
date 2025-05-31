#include <allegro5/allegro.h>
#include "callback_handlers.h"
#include "config_vars.h"
#include "elk.h"
#include "host_abstraction_layer/event_handler.h"
#include "joystick_internal.h"
#include "logger.h"


// Main event handling Code
elk_event_t joystick_handler_handle_event(ALLEGRO_EVENT *event)
{
    elk_event_t elkEvent = 0;

    // Was the key pressed in our main screen?
    // If not it was on the keyboard redefining screen
    // so we ignore.
    switch(event->type)
    {
        case ALLEGRO_EVENT_JOYSTICK_AXIS:
            log_info("ALLEGRO_EVENT_JOYSTICK_AXIS - stick %d, axis %d, pos %f\n", event->joystick.stick, event->joystick.axis, event->joystick.pos);
            break;

        case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
            log_info("ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN - id %d, button %d\n", event->joystick.button);
            break;

        case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
            log_info("ALLEGRO_EVENT_JOYSTICK_BUTTON_UP - id %d, button %d\n", event->joystick.button);
            break;
    }

    return elkEvent;
}
