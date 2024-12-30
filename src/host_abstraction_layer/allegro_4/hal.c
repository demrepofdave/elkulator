/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * hal.c
 * 
 * Main initialisation, timer, and other general functions to do with 
 * the overall host abstraction layer.
 *
 * This is the allegro 4 implementation of the abstraction layer.
 *
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include "logger.h"
#include "elk.h"
#include "video_internal.h"
#include "host_abstraction_layer/sound.h"
#include "host_abstraction_layer/video.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/


/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/


/******************************************************************************
* Private Function Definitions
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

// Called from linux.c (main)
int hal_init_begin()
{
    int result = video_init_begin();
    if(result == 0)
    {
        sound_init_begin(0,NULL);
    }
    return result;
}

void hal_init_complete()
{
    video_init_complete();
    sound_init_complete();
}

void hal_shutdown()
{
    // Nothing to do for allegro 5.
}


// Called from main.c (initelk)
void hal_install_timer_callback(void (*timer_function)(void))
{
#ifndef WIN32
    install_keyboard();
#endif
    install_timer();
    install_int_ex(timer_function,MSEC_TO_TIMER(20));
    install_joystick(JOY_TYPE_AUTODETECT);
    install_mouse();
}

void hal_timer_rest(unsigned int period)
{
    return rest(period);
}

void hal_start_timer()
{
    // For allegro4 there is nothing to do.
}

void hal_stop_timer()
{
    // For allegro4 there is nothing to do. 
}
