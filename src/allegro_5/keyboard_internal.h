#ifndef _KEYBOARD_INTERNAL_H
#define _KEYBOARD_INTERNAL_H

#include <stdint.h>
#include <allegro5/allegro.h>
#include "common/event_handler.h"

elk_event_t keyboard_handle_event(ALLEGRO_EVENT *event);

# endif // _KEYBOARD_INTERNAL_H