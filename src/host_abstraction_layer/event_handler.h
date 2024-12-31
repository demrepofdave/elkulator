/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * event_handle.h
 * 
 * Abstracts the event handling. 
 * 
 * When event_await() is called this function will wait for the
 * various events from the graphics / sound / keyboard native drivers
 * and if require will use callback functions setup (in callback_handler.c)
 * to allow the electron code to respond accordingly.
 * 
 */

#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdint.h>

/******************************************************************************
* Preprocessor Macros and defines
*******************************************************************************/

// ELK EVENTS are a bitmap, more than one event may be present in elk_event_t
#define ELK_EVENT_NONE                   0x0000
#define ELK_EVENT_EXIT                   0x0001
#define ELK_EVENT_RESET                  0x0002
#define ELK_EVENT_TIMER_TRIGGERED        0x0004
#define ELK_EVENT_MENU_ITEM_STATE_CHANGE 0x0080
#define ELK_EVENT_HANDLED                0x8000

/******************************************************************************
* Typedefs
*******************************************************************************/

typedef uint16_t elk_event_t;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

uint32_t event_await();

#endif // _EVENT_HANDLER_H