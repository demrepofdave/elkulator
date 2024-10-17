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
#include "logger.h"


elk_event_t menu_handle_load_tape(ALLEGRO_EVENT * event);
elk_event_t menu_handle_rewind_tape(ALLEGRO_EVENT * event);
elk_event_t menu_handle_tape_eject(ALLEGRO_EVENT * event);
elk_event_t menu_handle_tape_speed(ALLEGRO_EVENT * event);

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

    log_debug("create_tape_menu:menu %p\n", menu);
    log_debug("create_tape_menu:sub_menu_tape_speed %p\n", sub_menu_tape_speed);
    append_menu_item(menu, "Load tape...", IDM_TAPE_LOAD,   0, menu_handle_load_tape);
    append_menu_separator(menu);
    append_menu_item(menu, "Rewind tape",  IDM_TAPE_REWIND, 0, menu_handle_rewind_tape);  // TODO: Does not appear to work at the moment.
    append_menu_item(menu, "Eject tape",   IDM_TAPE_EJECT,  0, menu_handle_tape_eject);
    append_menu_separator(menu);
    al_append_menu_item(menu, "Tape speed",   0,               0, NULL, sub_menu_tape_speed);
    
    add_radio_set(sub_menu_tape_speed, tape_speed_names, IDM_TAPE_SPEED, elkConfig.tape.speed, menu_handle_tape_speed);

    return menu;
}


ALLEGRO_PATH *tape_fn = NULL;

// Menu event handler functions.
// Called when IDM_TAPE_LOAD event is recieved.
elk_event_t menu_handle_load_tape(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_load_gui(event, "Choose a tape to load", "*.uef;*.csw", tape_fn);
    //tape_load_ui(event, "Choose a tape to load", "*.uef;*.csw");
    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("menu_handle_load_tape: path_str='%s'\n", path_str);
            callback_handlers.handler_load_tape(path_str);
            if(tape_fn)
            {
                al_destroy_path(tape_fn);
                tape_fn = path;
            }
        } 
        else
        {
            log_debug("menu_handle_load_tape: path_str is NULL\n");
            al_destroy_path(path);  
        }
    }
    return(0);
}


// Called when IDM_TAPE_REWIND event is recieved.
elk_event_t menu_handle_rewind_tape(ALLEGRO_EVENT * event)
{
    callback_handlers.rewind_tape();
    return(0);
}

// Called when IDM_FILE_LOAD_STATE event is recieved.
elk_event_t menu_handle_tape_eject(ALLEGRO_EVENT * event)
{
    callback_handlers.eject_tape();
    return(0);
}

// Called when IDM_TAPE_SPEED event is recieved.
elk_event_t menu_handle_tape_speed(ALLEGRO_EVENT * event)
{
    log_debug("menu_handle_tape_speed: old = %d\n", elkConfig.tape.speed);
    elkConfig.tape.speed = radio_event_simple(event, elkConfig.tape.speed);
    log_debug("menu_handle_tape_speed: new = %d\n", elkConfig.tape.speed);
    return(0);
}


// End of file.