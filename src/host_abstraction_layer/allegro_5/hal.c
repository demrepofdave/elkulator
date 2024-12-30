/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * hal.c
 * 
 * Main initialisation, timer, and other general functions to do with 
 * the overall host abstraction layer.
 *
 * This is the allegro 5 implementation of the abstraction layer.
 *
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "host_abstraction_layer/sound.h"
#include "host_abstraction_layer/video.h"
#include "host_abstraction_layer/allegro_5//menu_internal.h"
#include "logger.h"
#include "elk.h"
#include "video_internal.h"
#include "event_handler_internal.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/


/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

static ALLEGRO_EVENT_SOURCE evsrc;
static ALLEGRO_TIMER *timer;

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
        // Continue with initialization
        sound_init_begin(0,NULL);
    }

    return result;
}

void hal_init_complete()
{
    video_init_complete();
    sound_init_complete();

    if (!(timer = al_create_timer(0.02)))
    {
        log_fatal("main: unable to create timer");
        exit(1);
    }
    event_register_event_source(al_get_timer_event_source(timer));
    al_init_user_event_source(&evsrc);
    event_register_event_source(&evsrc);

    event_register_event_source(al_get_keyboard_event_source());

    al_install_mouse();
    event_register_event_source(al_get_mouse_event_source());
}

void hal_shutdown()
{
    // Nothing to do for allegro 5.
}


// Called from main.c (initelk)
void hal_install_timer_callback(void (*timer_function)(void))
{
    // Nothing to do.
}

void hal_timer_rest(unsigned int period)
{
    return;
}

void hal_start_timer()
{
    al_start_timer(timer);
}

void hal_stop_timer()
{
    al_stop_timer(timer);   
}
