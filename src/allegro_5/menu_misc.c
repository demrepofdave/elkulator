/*
 * Elkulator - An electron emulator originally written 
 *              by Sarah Walker
 *
 * menu_misc.c
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

ALLEGRO_MENU *create_misc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Save screenshot", IDM_SETTINGS_MISC_SCREENSHOT,      ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Start movie",     IDM_SETTINGS_MISC_START_MOVIE,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Stop movie",      IDM_SETTINGS_MISC_STOP_MOVIE,      ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Start debugging", IDM_SETTINGS_MISC_START_DEBUGGING, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}