#ifndef _EVENT_HANDLER_INTERNAL_H
#define _EVENT_HANDLER_INTERNAL_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include "common/event_handler.h"

typedef elk_event_t (*callback_event_handler_t)(ALLEGRO_EVENT *event);

bool event_init_queue();
void event_register_event_source(ALLEGRO_EVENT_SOURCE * event_source);

ALLEGRO_EVENT_QUEUE * event_get_event_queue();


#endif // _EVENT_HANDLER_INTERNAL_H