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
    al_append_menu_item(menu, "Rewind tape",  IDM_TAPE_REWIND, 0, NULL, NULL);  // TODO: Does not appear to work at the moment.
    al_append_menu_item(menu, "Eject tape",   IDM_TAPE_EJECT,  0, NULL, NULL);
    al_append_menu_item(menu, "Tape speed",   0,               0, NULL, sub_menu_tape_speed);
    
    add_radio_set(sub_menu_tape_speed, tape_speed_names, IDM_TAPE_SPEED, elkConfig.tape.speed);

    return menu;
}
// End of file.