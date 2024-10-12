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

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

static ALLEGRO_MENU *create_tape_speed_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Normal",      IDM_TAPE_SPEED_NORMAL,      (elkConfig.tape.speed == TAPE_NORMAL));
    add_checkbox_item(menu, "Fast",        IDM_TAPE_SPEED_FAST,        (elkConfig.tape.speed == TAPE_FAST));
    add_checkbox_item(menu, "Really Fast", IDM_TAPE_SPEED_REALLY_FAST, (elkConfig.tape.speed == TAPE_REALLY_FAST));
    return menu;
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_tape_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Load tape...", IDM_TAPE_LOAD,   0,                          NULL, NULL);
    al_append_menu_item(menu, "Rewind tape",  IDM_TAPE_REWIND, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Eject tape",   IDM_TAPE_EJECT,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Tape speed",   0,               0,                          NULL, create_tape_speed_menu());
    return menu;
}
// End of file.