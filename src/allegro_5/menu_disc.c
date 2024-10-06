/*
 * Elkulator - An electron emulator originally written 
 *              by Sarah Walker
 *
 * menu_disc.c
 * 
 * This handles the creation of the Disc section of the native menu.
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

ALLEGRO_MENU *create_disc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    al_append_menu_item(menu, "Load disc :0/2...",       IDM_DISC_LOAD_0_2,              ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Load disc :1/3...",       IDM_DISC_LOAD_1_3,              ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Eject disc :0/2",         IDM_DISC_EJECT_0_2,             ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Eject disc :1/3",         IDM_DISC_EJECT_1_3,             ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Write protect disc :0/2", IDM_DISC_WRITE_PROTECT_0_2,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Write protect disc :1/3", IDM_DISC_WRITE_PROTECT_1_3,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Default write protect",   IDM_DISC_WRITE_PROTECT_DEFAULT, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}
