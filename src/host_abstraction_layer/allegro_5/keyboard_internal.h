#ifndef _KEYBOARD_INTERNAL_H
#define _KEYBOARD_INTERNAL_H

#include <stdint.h>
#include <allegro5/allegro.h>
#include "host_abstraction_layer/event_handler.h"
#include "host_abstraction_layer/keyboard.h"

extern bool keydefining;
extern host_key_t keylookup[HOST_KEY_MAX];

elk_event_t keyboard_handle_event(ALLEGRO_EVENT *event);

# endif // _KEYBOARD_INTERNAL_H