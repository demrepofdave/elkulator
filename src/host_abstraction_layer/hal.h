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

// Initialisation functions.
int  hal_init_begin();
void hal_init_complete();
void hal_shutdown();

// Timer functions
void hal_install_timer_callback(void (*timer_function)(void));
void hal_start_timer();
void hal_stop_timer();
void hal_timer_rest(unsigned int period);

#endif // _VIDEO_H
