#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <stdint.h>

// Following allegro5 only.
#define ELK_EVENT_EXIT  0x01
#define ELK_EVENT_RESET 0x02
#define ELK_EVENT_TIMER_TRIGGERED 0x04

uint32_t event_await();

#endif // _EVENT_HANDLER_H