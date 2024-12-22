#ifndef _KEYBOARD_INTERNAL_H
#define _KEYBOARD_INTERNAL_H

#include <stdint.h>
#include <allegro5/allegro.h>
#include "host_abstraction_layer/event_handler.h"
#include "host_abstraction_layer/keyboard.h"

extern bool keydefining;

uint8_t keyboard_allegro5_key_to_host_key(uint8_t allegro_key);
elk_event_t keyboard_handle_event(ALLEGRO_EVENT *event);

# endif // _KEYBOARD_INTERNAL_H