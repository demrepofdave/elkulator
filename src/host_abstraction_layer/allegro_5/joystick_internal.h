#ifndef _JOYSTICK_INTERNAL_H
#define _JOYSTICK_INTERNAL_H

#include <stdint.h>
#include <allegro5/allegro.h>
#include "host_abstraction_layer/event_handler.h"

void joystick_init(void);
elk_event_t joystick_handler_handle_event(ALLEGRO_EVENT *event);

# endif // _JOYSTICK_INTERNAL_H