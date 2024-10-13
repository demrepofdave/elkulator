/*
 * Elkulator - An electron emulator originally written 
 *              by Sarah Walker
 *
 * menu_tape.c
 * 
 * This handles the creation of the tape section of the native menu.
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "config_vars.h"
#include "menu_internal.h"
#include "callback_handlers.h"


uint16_t menu_handle_rewind_tape(ALLEGRO_EVENT * event);
uint16_t menu_handle_tape_eject(ALLEGRO_EVENT * event);
uint16_t menu_handle_tape_speed(ALLEGRO_EVENT * event);

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

static const char *tape_speed_names[] = { "Normal", "Fast", "Really Fast", NULL };


/******************************************************************************
* Private Function Definitions
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_tape_menu(void)
{
    ALLEGRO_MENU *menu                 = al_create_menu();
    ALLEGRO_MENU *sub_menu_tape_speed  = al_create_menu();

    al_append_menu_item(menu, "Load tape...", IDM_TAPE_LOAD,   0, NULL, NULL);
    append_menu_item(menu, "Rewind tape",  IDM_TAPE_REWIND, 0, menu_handle_rewind_tape);  // TODO: Does not appear to work at the moment.
    append_menu_item(menu, "Eject tape",   IDM_TAPE_EJECT,  0, menu_handle_tape_eject);
    al_append_menu_item(menu, "Tape speed",   0,               0, NULL, sub_menu_tape_speed);
    
    add_radio_set(sub_menu_tape_speed, tape_speed_names, IDM_TAPE_SPEED, elkConfig.tape.speed, menu_handle_tape_speed);

    return menu;
}


// Menu event handler functions.

// Called when IDM_TAPE_REWIND event is recieved.
uint16_t menu_handle_rewind_tape(ALLEGRO_EVENT * event)
{
    callback_handlers.rewind_tape();
    return(0);
}

// Called when IDM_FILE_LOAD_STATE event is recieved.
uint16_t menu_handle_tape_eject(ALLEGRO_EVENT * event)
{
    callback_handlers.eject_tape();
    return(0);
}

// Called when IDM_TAPE_SPEED event is recieved.
uint16_t menu_handle_tape_speed(ALLEGRO_EVENT * event)
{
    elkConfig.tape.speed = radio_event_simple(event, elkConfig.tape.speed);
    return(0);
}


// End of file.