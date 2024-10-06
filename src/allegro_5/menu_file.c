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

/******************************************************************************
* Private Function Definitions
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_file_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    al_append_menu_item(menu, "Hard Reset", IDM_FILE_HARD_RESET, 0, NULL, NULL);
    al_append_menu_item(menu, "Load state...", IDM_FILE_LOAD_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Save State...", IDM_FILE_SAVE_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Exit", IDM_FILE_EXIT, 0, NULL, NULL);
    return menu;
}
