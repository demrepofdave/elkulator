#include <allegro5/allegro.h>
#include "callback_handlers.h"
#include "config_vars.h"
#include "elk.h"
#include "host_abstraction_layer/event_handler.h"
#include "joystick_internal.h"
#include "logger.h"


void joystick_init()
{
    // Get number of joysticks
    int num_joysticks = al_get_num_joysticks();
    log_info("joystick_init: num joysticks %d\n", num_joysticks);

    for(int index = 0; index < num_joysticks; index++)
    {
        ALLEGRO_JOYSTICK * joystick = al_get_joystick(index);
        log_info("joystick_init: Joystick %s, buttons = %d, active %d\n", al_get_joystick_name(joystick), al_get_joystick_num_buttons(joystick), al_get_joystick_active(joystick));
        int num_sticks = al_get_joystick_num_sticks(joystick);
        for(int sticks = 0; sticks < num_sticks; sticks++)
        {
            log_info("joystick_init: - sticks %d - %s\n", sticks, al_get_joystick_stick_name(joystick, sticks));
            int num_axis = al_get_joystick_num_axes(joystick, sticks);
            for(int axis = 0; axis < num_axis; axis++)
            {
                log_info("joystick_init:   - axis %d - %s\n", axis, al_get_joystick_axis_name(joystick, sticks, axis));
            }
        }
    }
}


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
        case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
            log_info("ALLEGRO_EVENT_JOYSTICK_CONFIGURATION\n");
            al_reconfigure_joysticks();
            joystick_init();
            break;
    }

    return elkEvent;
}
