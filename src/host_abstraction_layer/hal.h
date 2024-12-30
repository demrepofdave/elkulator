/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * hal.h
 * 
 * Main initialisation, timer, and other general functions to do with 
 * the overall host abstraction layer.
 * 
 */

#ifndef _HAL_H
#define _HAL_H

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/



/******************************************************************************
* Typedefs
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

int  hal_init_begin();                                // Called from main() in linux.c
void hal_init_complete();                                // Called from ulainit() in ula.c
void hal_shutdown();

// Timer functions

void hal_install_timer_callback(void (*timer_function)(void));    // Called from initelk() in main.c
void hal_start_timer();
void hal_stop_timer();
void video_timer_rest(unsigned int period);  // Only used in allegro4.

#endif // _VIDEO_H
