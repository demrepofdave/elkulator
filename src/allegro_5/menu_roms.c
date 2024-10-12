/*
 * Elkulator - An electron emulator originally written 
 *              by Sarah Walker
 *
 * menu_roms.c
 * 
 * This handles the creation of the Roms section of the native menu.
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

static ALLEGRO_MENU *create_romscarts_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Mega Games Cartridge",        IDM_ROMCARTS_MEGAGAMES,        ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "David's Flash ROM Cartridge", IDM_ROMCARTS_DAVIDS_FLASH_ROM, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_roms_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Load ROM cartridge &1...", IDM_ROMS_LOAD_CARTRIDGE1, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Load ROM cartridge &2...", IDM_ROMS_LOAD_CARTRIDGE2, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Unload ROM cartridges...", IDM_ROMS_UNLOAD,          ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Multi-ROM expansions",  0, 0, NULL, create_romscarts_menu());
    return menu;
}