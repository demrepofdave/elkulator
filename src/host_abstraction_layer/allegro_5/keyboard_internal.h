#ifndef _KEYBOARD_INTERNAL_H
#define _KEYBOARD_INTERNAL_H

#include <stdint.h>
#include <allegro5/allegro.h>
#include "host_abstraction_layer/event_handler.h"

extern bool keydefining;
extern int keylookup[ALLEGRO_KEY_MAX];

elk_event_t keyboard_handle_event(ALLEGRO_EVENT *event);

# endif // _KEYBOARD_INTERNAL_H