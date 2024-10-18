/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
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
#include "callback_handlers.h"
#include "logger.h"
// TODO: Remove below.
#include "elk.h"

elk_event_t menu_handle_disc_load_0_2 (ALLEGRO_EVENT * event);
elk_event_t menu_handle_disc_load_1_3 (ALLEGRO_EVENT * event);
elk_event_t menu_handle_disc_eject_0_2(ALLEGRO_EVENT * event);
elk_event_t menu_handle_disc_eject_1_3(ALLEGRO_EVENT * event);
elk_event_t menu_handle_default_write_protect(ALLEGRO_EVENT * event);

/******************************************************************************
* Private Function Definitions
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_disc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    append_menu_item(menu, "Load disc :0/2...", IDM_DISC_LOAD_0_2,   0, menu_handle_disc_load_0_2);
    append_menu_item(menu, "Load disc :1/3...", IDM_DISC_LOAD_1_3,   0, menu_handle_disc_load_1_3);
    append_menu_separator(menu);
    append_menu_item(menu, "Eject disc :0/2",         IDM_DISC_EJECT_0_2, 0, menu_handle_disc_eject_0_2);
    append_menu_item(menu, "Eject disc :1/3",         IDM_DISC_EJECT_1_3, 0, menu_handle_disc_eject_1_3);
    append_menu_separator(menu);
    al_append_menu_item(menu, "Write protect disc :0/2", IDM_DISC_WRITE_PROTECT_0_2,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL); // TODO:
    al_append_menu_item(menu, "Write protect disc :1/3", IDM_DISC_WRITE_PROTECT_1_3,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL); // TODO:
    append_menu_separator(menu);
    add_checkbox_item(menu, "Default write protect",   IDM_DISC_WRITE_PROTECT_DEFAULT,  elkConfig.disc.defaultwriteprot, menu_handle_default_write_protect);
    return menu;
}


ALLEGRO_PATH *disc_fn = NULL;

// Menu event handler functions.
// Called when IDM_DISC_LOAD_0_2 event is recieved.
elk_event_t menu_handle_disc_load_0_2(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_load_gui(event, "Please choose a disc image", "*.ssd;*.dsd;*.img;*.adf;*.adl;*.fdi", disc_fn);

    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("menu_handle_disc_load: path_str='%s'", path_str);
            callback_handlers.handler_load_disc0_2(path_str);
            if(disc_fn)
            {
                al_destroy_path(disc_fn);
                disc_fn = path;
            }
        } 
        else
        {
            log_debug("menu_handle_disc_load: path_str is NULL");
            al_destroy_path(path);  
        }
    }
    return(0);
}

// Called when IDM_DISC_LOAD_0_2 event is recieved.
elk_event_t menu_handle_disc_load_1_3(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_load_gui(event, "Please choose a disc image", "*.ssd;*.dsd;*.img;*.adf;*.adl;*.fdi", disc_fn);

    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("menu_handle_disc_load: path_str='%s'", path_str);
            callback_handlers.handler_load_disc1_3(path_str);
            if(disc_fn)
            {
                al_destroy_path(disc_fn);
                disc_fn = path;
            }
        } 
        else
        {
            log_debug("menu_handle_disc_load: path_str is NULL");
            al_destroy_path(path);  
        }
    }
    return(0);
}

// Called when IDM_DISC_EJECT_0_2 event is recieved.
elk_event_t menu_handle_disc_eject_0_2(ALLEGRO_EVENT * event)
{
    // TODO: proper callbacks
    closedisc(0);
    elkConfig.disc.discname[0]=0;
    return(0);
}

// Called when IDM_DISC_EJECT_1_3 event is recieved.
elk_event_t menu_handle_disc_eject_1_3(ALLEGRO_EVENT * event)
{
    // TODO: proper callbacks
    closedisc(1);
    elkConfig.disc.discname2[0]=0;
    return(0);
}

// Called when IDM_DISC_WRITE_PROTECT_DEFAULT event is recieved.
elk_event_t menu_handle_default_write_protect(ALLEGRO_EVENT * event)
{
    elkConfig.disc.defaultwriteprot = !elkConfig.disc.defaultwriteprot;
    return(0);
}