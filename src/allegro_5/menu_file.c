/*
 * Elkulator - An electron emulator originally written 
 *              by Sarah Walker
 *
 * menu_file.c
 * 
 * This handles the creation of the File section of the native menu.
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "config_vars.h"
#include "menu_internal.h"
#include "common/event_handler.h"

uint16_t handle_menu_event_file_hard_reset(ALLEGRO_EVENT * event);

/******************************************************************************
* Private Function Definitions
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_file_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    append_menu_item(menu, "Hard Reset", IDM_FILE_HARD_RESET, 0, handle_menu_event_file_hard_reset);
    al_append_menu_item(menu, "Load state...", IDM_FILE_LOAD_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Save State...", IDM_FILE_SAVE_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Exit", IDM_FILE_EXIT, 0, NULL, NULL);
    return menu;
}

// Menu event handler functions.

// Called when IDM_FILE_HARD_RESET event is recieved.
uint16_t handle_menu_event_file_hard_reset(ALLEGRO_EVENT * event)
{
    return(ELK_EVENT_RESET);
}

// Called when IDM_FILE_LOAD_STATE event is recieved.
uint16_t handle_menu_event_file_load_state(ALLEGRO_EVENT * event)
{
    return(ELK_EVENT_RESET);
}
