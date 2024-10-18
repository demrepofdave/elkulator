/*
 * Elkulator - An electron emulator originally written 
 *              by Sarah Walker
 *
 * menus.c
 * 
 * This file both creates the native fui menus via allegro5 and
 * processes the menu events.
 */

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>
#include <allegro5/allegro_native_dialog.h>
#include "common/video.h"
#include "menu_internal.h"
#include "logger.h"
#include "callback_handlers.h"
#include "config_vars.h"
#include "common/event_handler.h"
#include "event_handler_internal.h"


/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

/* pclose() and popen() on Windows are _pclose() and _popen() */
#ifdef _WIN32
#  define pclose _pclose
#  define popen _popen
#endif

#define MAX_MENU_CALLBACK_EVENT_HANDLERS 256


/******************************************************************************
* Typedefs
*******************************************************************************/

typedef struct {
    const char *label;
    int itemno;
} menu_map_t;

typedef struct 
{
    uint16_t menu_id;
    callback_event_handler_t handler_function;
} callback_menu_event_handler_list_t;

callback_menu_event_handler_list_t callback_menu_event_handler_list[MAX_MENU_CALLBACK_EVENT_HANDLERS];
uint16_t menu_registered_handlers;


/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
elk_event_t menu_null_handler(ALLEGRO_EVENT * event);

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

static inline int menu_id_num(menu_id_t id, int num)
{
    return (num << 8) | id;
}

// Now public.
static inline menu_id_t menu_get_id(ALLEGRO_EVENT *event)
{
    return event->user.data1 & 0xff;
}

static inline int menu_get_num(ALLEGRO_EVENT *event)
{
    return event->user.data1 >> 8;
}

// Public internal function;

void add_checkbox_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, bool checked, callback_event_handler_t menu_handler_function)
{
    int flags = ALLEGRO_MENU_ITEM_CHECKBOX;
    if (checked)
        flags |= ALLEGRO_MENU_ITEM_CHECKED;
    al_append_menu_item(parent, title, id, flags, NULL, NULL);
    if(menu_handler_function)
    {
        register_menu_event_handler(id, menu_handler_function);
    }
}

static void add_radio_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, int this_value, int cur_value)
{
    add_checkbox_item(parent, title, menu_id_num(id, this_value), this_value == cur_value, NULL);
}

void add_radio_set(ALLEGRO_MENU *parent, char const **labels, uint16_t id, int cur_value, callback_event_handler_t menu_handler_function)
{
    int i;
    const char *label;

    for (i = 0; (label = *labels++); i++)
        add_checkbox_item(parent, label, menu_id_num(id, i), i == cur_value, NULL);

    if(menu_handler_function)
    {
        register_menu_event_handler(id, menu_handler_function);
    }
}

static int menu_cmp(const void *va, const void *vb)
{
    menu_map_t *a = (menu_map_t *)va;
    menu_map_t *b = (menu_map_t *)vb;
    return strcasecmp(a->label, b->label);
}

static void add_sorted_set(ALLEGRO_MENU *parent, menu_map_t *map, size_t items, uint16_t id, int cur_value)
{
    int i, ino;

    qsort(map, items, sizeof(menu_map_t), menu_cmp);
    for (i = 0; i < items; i++) {
        ino = map[i].itemno;
        add_checkbox_item(parent, map[i].label, menu_id_num(id, ino), ino == cur_value, NULL);
    }
}

int radio_event_simple(ALLEGRO_EVENT *event, int current)
{
    int id = menu_get_id(event);
    int num = menu_get_num(event);
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);

    log_debug("radio_event_simple:menu %p id=%d, num=%d, current=%d data1=%lx\n", menu, id, num, current, event->user.data1);

    al_set_menu_item_flags(menu, menu_id_num(id, current), ALLEGRO_MENU_ITEM_CHECKBOX);
    return num;
}

static int radio_event_with_deselect(ALLEGRO_EVENT *event, int current)
{
    int id = menu_get_id(event);
    int num = menu_get_num(event);
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);

    if (num == current)
        num = -1;
    else
        al_set_menu_item_flags(menu, menu_id_num(id, current), ALLEGRO_MENU_ITEM_CHECKBOX);
    return num;
}

bool append_menu_item(ALLEGRO_MENU *menu, const char * title, uint16_t id, int flags, callback_event_handler_t menu_handler_function)
{
    al_append_menu_item(menu, title, id, flags, NULL, NULL);
    if(menu_handler_function)
    {
        register_menu_event_handler(id, menu_handler_function);
    }
    return(true); // TODO: Fix.
}

void append_menu_separator(ALLEGRO_MENU *menu)
{
    al_append_menu_item(menu, NULL, 0, 0, NULL, NULL);
}


void uncheck_menu_item(ALLEGRO_MENU *menu, int id)
{
    if(menu)
    {
        int flags = al_get_menu_item_flags(menu, id);
        log_debug("uncheck_menu_item: Menu item %d, flags %d\n", id, flags);
        if(flags & ALLEGRO_MENU_ITEM_CHECKED)
        {
            // If set, we untoggle
            flags ^= ALLEGRO_MENU_ITEM_CHECKED;
            log_debug("uncheck_menu_item: flags are %d, menu_item_checked\n", flags);
            al_set_menu_item_flags(menu, id, flags);

            // Post verification check
            flags = al_get_menu_item_flags(menu, id);
            log_debug("uncheck_menu_item: post flags are %d, menu_item_checked\n", flags);
        }
    }
}

// TODO: Does not seem to work for the moment.
void disable_menu_item(ALLEGRO_MENU *menu, int id)
{
    if(menu)
    {
        int flags = al_get_menu_item_flags(menu, id);
        if(flags & ALLEGRO_MENU_ITEM_DISABLED)
        {
            // If set, we untoggle
            flags = ALLEGRO_MENU_ITEM_DISABLED;
            al_set_menu_item_flags(menu, id, flags);
        }
    }
}


// File Menu - All functions helpers

char * savestate_name = NULL;
void file_load_state(ALLEGRO_EVENT *event, const char * title, const char * patterns)
{
    ALLEGRO_DISPLAY *display = (ALLEGRO_DISPLAY *)(event->user.data2);
    ALLEGRO_FILECHOOSER *chooser = al_create_native_file_dialog(savestate_name, title, patterns, ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
    if (chooser) 
    {
        if (al_show_native_file_dialog(display, chooser)) 
        {
            if (al_get_native_file_dialog_count(chooser) > 0)
            {
                ALLEGRO_PATH *path = al_create_path(al_get_native_file_dialog_path(chooser, 0));
                const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
                if(path_str)
                {
                    log_debug("file_load_state: path_str='%s'\n", path_str);
                    callback_handlers.handler_load_state(path_str);
                } 
                else
                {
                    log_debug("file_load_state: path_str is NULL\n");
                }
            }
        }
        al_destroy_native_file_dialog(chooser);
    }
}

// Tape menu.

ALLEGRO_PATH * menu_load_gui(ALLEGRO_EVENT *event, const char * title, const char * patterns, ALLEGRO_PATH * starting_path)
{
    ALLEGRO_FILECHOOSER *chooser;
    ALLEGRO_DISPLAY *display;
    const char *fpath;
    ALLEGRO_PATH *path = NULL;

    if(starting_path)
    {
        fpath = al_path_cstr(starting_path, ALLEGRO_NATIVE_PATH_SEP);
    }
    else
    {
        fpath = ".";
    }

    if ((chooser = al_create_native_file_dialog(fpath, title, patterns, ALLEGRO_FILECHOOSER_FILE_MUST_EXIST))) 
    {
        display = (ALLEGRO_DISPLAY *)(event->user.data2);
        if (al_show_native_file_dialog(display, chooser)) 
        {
            if (al_get_native_file_dialog_count(chooser) > 0) 
            {
                path = al_create_path(al_get_native_file_dialog_path(chooser, 0));
            }
        }
    }

    return(path);
}

void file_save_state(ALLEGRO_EVENT *event, const char * title, const char * patterns)
{
    ALLEGRO_FILECHOOSER *chooser;
    ALLEGRO_DISPLAY *display;

    if ((chooser = al_create_native_file_dialog(savestate_name, "Save state to file", "*.snp", ALLEGRO_FILECHOOSER_SAVE))) 
    {
        display = (ALLEGRO_DISPLAY *)(event->user.data2);
        if (al_show_native_file_dialog(display, chooser))
        {
            if (al_get_native_file_dialog_count(chooser) > 0)
            {
                ALLEGRO_PATH *path = al_create_path(al_get_native_file_dialog_path(chooser, 0));
                const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
                if(path_str)
                {
                    log_debug("file_save_state: path_str='%s'\n", path_str);
                    callback_handlers.handler_save_state(path_str);
                } 
                else
                {
                    log_debug("file_save_state: path_str is NULL\n");
                }
            }
        }
        al_destroy_native_file_dialog(chooser);
    }
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

bool register_menu_event_handler(int id, callback_event_handler_t menu_handler)
{
    bool registered = false;
    if( menu_registered_handlers < MAX_MENU_CALLBACK_EVENT_HANDLERS )
    {
        callback_menu_event_handler_list[menu_registered_handlers].menu_id = id;
        callback_menu_event_handler_list[menu_registered_handlers].handler_function = menu_handler;
        menu_registered_handlers++;
        registered = true;
        log_debug("Registered menu %d to %p", id, menu_handler);
    }
    return (registered);
}

void menu_init(ALLEGRO_DISPLAY *display)
{
    log_debug("menu_init\n");
    menu_registered_handlers = 0;

    register_menu_event_handler(IDM_ZERO, menu_null_handler);

    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "File",     0, 0, NULL, create_file_menu());
    al_append_menu_item(menu, "Tape",     0, 0, NULL, create_tape_menu());
    al_append_menu_item(menu, "Disc",     0, 0, NULL, create_disc_menu());
    al_append_menu_item(menu, "ROM",      0, 0, NULL, create_roms_menu());
    al_append_menu_item(menu, "Settings", 0, 0, NULL, create_settings_menu());
    al_append_menu_item(menu, "Misc",     0, 0, NULL, create_misc_menu());
    al_set_display_menu(display, menu);
    al_register_event_source(event_get_event_queue(), al_get_default_menu_event_source());
}

void menu_destroy(ALLEGRO_DISPLAY *display)
{
    al_unregister_event_source(event_get_event_queue(), al_get_default_menu_event_source());
    al_set_display_menu(display, NULL);
}

static const char all_dext[] = "*.ssd;*.dsd;*.img;*.adf;*.ads;*.adm;*.adl;*.sdd;*.ddd;*.fdi;*.imd;*.hfe;"
                               "*.SSD;*.DSD;*.IMG;*.ADF;*.ADS;*.ADM;*.ADL;*.SDD;*.DDD;*.FDI;*.IMD;*.HFE";

elk_event_t menu_handle_event(ALLEGRO_EVENT *event)
{
    elk_event_t elkEvent = 0;
    uint16_t count = 0;
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);
    menu_id_t menu_id = menu_get_id(event);
    int num = menu_get_num(event);

    video_stop_timer();

    log_debug("menu_handle_event: menu %p, id %d, num %d\n", menu, menu_id, num);

    // Handle menu events.
    while(count < menu_registered_handlers && !(elkEvent & ELK_EVENT_HANDLED))
    {
        if(menu_id == callback_menu_event_handler_list[count].menu_id)
        {
            // Handler for the event found.
            elkEvent = callback_menu_event_handler_list[count].handler_function(event);
            elkEvent |= ELK_EVENT_HANDLED;
        }
        count++;
    }

    if(!(elkEvent & ELK_EVENT_HANDLED))
    {
        log_debug("menu_handle_event: menu event %d detected\n", menu_id);
    }

    video_start_timer();

    log_debug("menu_handle_event elkEvent = 0x%02x\n", elkEvent);

    return(elkEvent);
}

void entergui()
{
}

void setejecttext(int d, char *s)
{
}

void setquit()
{
}

void menu_update_on_state_change()
{

}

elk_event_t menu_null_handler(ALLEGRO_EVENT * event)
{
    return 0;
}