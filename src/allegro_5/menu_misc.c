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
#include "callback_handlers.h"
#include "menu_internal.h"
#include "logger.h"

elk_event_t menu_handle_screenshot(ALLEGRO_EVENT * event);

/******************************************************************************
* Private Function Definitions
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_misc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    append_menu_item(menu, "Save screenshot", IDM_SETTINGS_MISC_SCREENSHOT,  0, menu_handle_screenshot);
    al_append_menu_item(menu, "Start movie",     IDM_SETTINGS_MISC_START_MOVIE,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Stop movie",      IDM_SETTINGS_MISC_STOP_MOVIE,      ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Start debugging", IDM_SETTINGS_MISC_START_DEBUGGING, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

ALLEGRO_PATH *screenshot_fn = NULL;

// Called when IDM_ROMS_UNLOAD event is recieved.
elk_event_t menu_handle_screenshot(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_save_gui(event, "Save state to file", "*.snp", screenshot_fn);

    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("path_str='%s'", path_str);
            callback_handlers.handle_screenshot(path_str);
            if(screenshot_fn)
            {
                al_destroy_path(screenshot_fn);
                screenshot_fn = path;
            }
        } 
        else
        {
            log_debug("path_str is NULL");
            al_destroy_path(path);  
        }
    }

    return(0);
}