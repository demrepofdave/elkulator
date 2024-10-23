/******************************************************************************
 * Elkulator - An electron emulator originally written by Sarah Walker
 *
 * menu_file.c
 * 
 * This handles the creation of the File section of the native menu and
 * registers all the callback handlers to handle the file menu events
 ******************************************************************************/

/******************************************************************************
* Include files
*******************************************************************************/

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "config_vars.h"
#include "menu_internal.h"
#include "common/event_handler.h"
#include "callback_handlers.h"
#include "logger.h"


/******************************************************************************
* Function Prototypes
*******************************************************************************/
elk_event_t handle_menu_event_file_exit(ALLEGRO_EVENT * event);
elk_event_t handle_menu_event_file_hard_reset(ALLEGRO_EVENT * event);
elk_event_t handle_menu_event_file_load_state(ALLEGRO_EVENT * event);
elk_event_t handle_menu_event_menu_save_gui(ALLEGRO_EVENT * event);


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_file_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    append_menu_item(menu, "Hard Reset",    IDM_FILE_HARD_RESET, 0, handle_menu_event_file_hard_reset);
    append_menu_separator(menu);
    append_menu_item(menu, "Load state...", IDM_FILE_LOAD_STATE, 0, handle_menu_event_file_load_state);
    append_menu_item(menu, "Save State...", IDM_FILE_SAVE_STATE, 0, handle_menu_event_menu_save_gui);
    append_menu_separator(menu);
    append_menu_item(menu, "Exit",          IDM_FILE_EXIT,       0, handle_menu_event_file_exit);
    return menu;
}

/******************************************************************************
* Menu Event Handler Function Definitions
*******************************************************************************/

// Called when IDM_FILE_HARD_RESET event is recieved.
elk_event_t  handle_menu_event_file_exit(ALLEGRO_EVENT * event)
{
    return(ELK_EVENT_EXIT);
}

// Called when IDM_FILE_HARD_RESET event is recieved.
elk_event_t  handle_menu_event_file_hard_reset(ALLEGRO_EVENT * event)
{
    return(ELK_EVENT_RESET);
}

ALLEGRO_PATH *state_fn = NULL;

// Called when IDM_FILE_LOAD_STATE event is recieved.
elk_event_t  handle_menu_event_file_load_state(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_load_gui(event, "Load state from file", "*.snp", state_fn);

    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("path_str='%s'\n", path_str);
            callback_handlers.handler_load_state(path_str);
            if(state_fn)
            {
                al_destroy_path(state_fn);
                state_fn = path;
            }
        } 
        else
        {
            log_debug("path_str is NULL\n");
            al_destroy_path(path);  
        }
    }
    return(0);

    //file_load_state(event, "Load state from file", "*.snp");
    // TODO: anything further?
    return(0);
}

elk_event_t handle_menu_event_menu_save_gui(ALLEGRO_EVENT * event)
{
    ALLEGRO_PATH *path = menu_save_gui(event, "Save state to file", "*.snp", state_fn);

    if(path)
    {
        const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
        if(path_str)
        {
            log_debug("path_str='%s'", path_str);
            callback_handlers.handler_save_state(path_str);
            if(state_fn)
            {
                al_destroy_path(state_fn);
                state_fn = path;
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
