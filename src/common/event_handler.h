#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <stdint.h>


typedef uint16_t elk_event_t;

// Following allegro5 only.
#define ELK_EVENT_EXIT                   0x0001
#define ELK_EVENT_RESET                  0x0002
#define ELK_EVENT_TIMER_TRIGGERED        0x0004
#define ELK_EVENT_MENU_ITEM_STATE_CHANGE 0x0080
#define ELK_EVENT_HANDLED                0x8000

uint32_t event_await();

#endif // _EVENT_HANDLER_H