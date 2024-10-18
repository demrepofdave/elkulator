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
#include "callback_handlers.h"
#include "logger.h"
// TODO: The following should be removed.
#include "6502.h"
#include "ula.h"

elk_event_t menu_handle_load_cart1(ALLEGRO_EVENT * event);
elk_event_t menu_handle_load_cart2(ALLEGRO_EVENT * event);
elk_event_t menu_handle_unload_rom_carts(ALLEGRO_EVENT * event);
elk_event_t menu_handle_toggle_mega_games_cart(ALLEGRO_EVENT * event);
elk_event_t menu_handle_toggle_db_flash_cartridge(ALLEGRO_EVENT * event);

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

static ALLEGRO_MENU *create_romscarts_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Mega Games Cartridge", IDM_ROMCARTS_MEGAGAMES, elkConfig.expansion.enable_mgc,   menu_handle_toggle_mega_games_cart);
    add_checkbox_item(menu, "David's Flash ROM Cartridge", IDM_ROMCARTS_DAVIDS_FLASH_ROM, elkConfig.expansion.enable_db_flash_cartridge,   menu_handle_toggle_db_flash_cartridge);
    return menu;
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_roms_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    append_menu_item(menu, "Load ROM cartridge 1...",  IDM_ROMS_LOAD_CARTRIDGE1,   0, menu_handle_load_cart1);
    append_menu_item(menu, "Load ROM cartridge 2...",  IDM_ROMS_LOAD_CARTRIDGE2,   0, menu_handle_load_cart2);
    append_menu_separator(menu);
    append_menu_item(menu, "Unload ROM cartridges...", IDM_ROMS_UNLOAD,            0, menu_handle_unload_rom_carts);
    append_menu_separator(menu);
    al_append_menu_item(menu, "Multi-ROM expansions",  0, 0, NULL, create_romscarts_menu());
    return menu;
}

ALLEGRO_PATH *cart_fn = NULL;

// Menu event handler functions.

// Called when IDM_ROMS_LOAD_CARTRIDGE1 event is recieved.
elk_event_t menu_handle_load_cart1(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_load_gui(event, "Please choose a ROM image", "*.rom", cart_fn);

    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("menu_handle_load_cart1: path_str='%s'", path_str);
            callback_handlers.handler_load_cart1(path_str);
            reset6502e(); // TODO: To this another way?
            resetula();   // TODO: To this another way?

            if(cart_fn)
            {
                al_destroy_path(cart_fn);
                cart_fn = path;
            }
        } 
        else
        {
            log_debug("menu_handle_load_cart1: path_str is NULL");
            al_destroy_path(path);  
        }
    }
    return(0);
}

// Called when IDM_ROMS_LOAD_CARTRIDGE1 event is recieved.
elk_event_t menu_handle_load_cart2(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_load_gui(event, "Please choose a ROM image", "*.rom", cart_fn);

    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("menu_handle_load_cart2: path_str='%s'", path_str);
            callback_handlers.handler_load_cart2(path_str);
            reset6502e(); // TODO: To this another way?
            resetula();   // TODO: To this another way?

            if(cart_fn)
            {
                al_destroy_path(cart_fn);
                cart_fn = path;
            }
        } 
        else
        {
            log_debug("menu_handle_load_cart2: path_str is NULL\n");
            al_destroy_path(path);  
        }
    }
    return(0);
}

// Called when IDM_ROMS_UNLOAD event is recieved.
elk_event_t menu_handle_unload_rom_carts(ALLEGRO_EVENT * event)
{
    callback_handlers.handle_unload_carts();
    reset6502e();  // TODO: To this another way?
    resetula();    // TODO: To this another way?
    return(0);
}

// Called when IDM_SETTINGS_DISC_PLUS3_ENABLE event is recieved.
elk_event_t menu_handle_toggle_mega_games_cart(ALLEGRO_EVENT * event)
{
    elkConfig.expansion.enable_mgc=!elkConfig.expansion.enable_mgc;
    return(0);
}

// Called when IDM_SETTINGS_DISC_PLUS3_ENABLE event is recieved.
elk_event_t menu_handle_toggle_db_flash_cartridge(ALLEGRO_EVENT * event)
{
    elkConfig.expansion.enable_db_flash_cartridge=!elkConfig.expansion.enable_db_flash_cartridge;
    return(0);
}