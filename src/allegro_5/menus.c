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


/******************************************************************************
* Typedefs
*******************************************************************************/

typedef struct {
    const char *label;
    int itemno;
} menu_map_t;

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/

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

void add_checkbox_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, bool checked)
{
    int flags = ALLEGRO_MENU_ITEM_CHECKBOX;
    if (checked)
        flags |= ALLEGRO_MENU_ITEM_CHECKED;
    al_append_menu_item(parent, title, id, flags, NULL, NULL);
}

static void add_radio_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, int this_value, int cur_value)
{
    add_checkbox_item(parent, title, menu_id_num(id, this_value), this_value == cur_value);
}

void add_radio_set(ALLEGRO_MENU *parent, char const **labels, uint16_t id, int cur_value)
{
    int i;
    const char *label;

    for (i = 0; (label = *labels++); i++)
        add_checkbox_item(parent, label, menu_id_num(id, i), i == cur_value);
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
        add_checkbox_item(parent, map[i].label, menu_id_num(id, ino), ino == cur_value);
    }
}

static int radio_event_simple(ALLEGRO_EVENT *event, int current)
{
    int id = menu_get_id(event);
    int num = menu_get_num(event);
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);

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

static void uncheck_menu_item(ALLEGRO_MENU *menu, int id)
{
    if(menu)
    {
        int flags = al_get_menu_item_flags(menu, id);
        if(flags & ALLEGRO_MENU_ITEM_CHECKED)
        {
            // If set, we untoggle
            flags = flags ^ ALLEGRO_MENU_ITEM_CHECKED;
            al_set_menu_item_flags(menu, id, flags);
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

// Variable and config handlers

void tape_speed_set(uint8_t new_speed)
{
    elkConfig.tape.speed = new_speed;
}

void video_display_set(uint8_t new_drawmode)
{
    //elkConfig.display.drawmode = new_drawmode;
}
void master_ram_board_mode_set(uint8_t new_mrbmode)
{
    elkConfig.expansion.mrbmode = new_mrbmode;
}

// File Menu - All functions helpers

char * savestate_name = NULL;
static void file_load_state(ALLEGRO_EVENT *event)
{
    ALLEGRO_DISPLAY *display = (ALLEGRO_DISPLAY *)(event->user.data2);
    ALLEGRO_FILECHOOSER *chooser = al_create_native_file_dialog(savestate_name, "Load state from file", "*.snp", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
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

ALLEGRO_PATH *tape_fn = NULL;

static void tape_load_ui(ALLEGRO_EVENT *event, const char * title, const char * patterns)
{
    ALLEGRO_FILECHOOSER *chooser;
    ALLEGRO_DISPLAY *display;
    const char *fpath;

    if (!tape_fn || !(fpath = al_path_cstr(tape_fn, ALLEGRO_NATIVE_PATH_SEP)))
        fpath = ".";
    if ((chooser = al_create_native_file_dialog(fpath, title, patterns, ALLEGRO_FILECHOOSER_FILE_MUST_EXIST))) 
    {
        display = (ALLEGRO_DISPLAY *)(event->user.data2);
        if (al_show_native_file_dialog(display, chooser)) 
        {
            if (al_get_native_file_dialog_count(chooser) > 0) 
            {
                //tape_close();
                ALLEGRO_PATH *path = al_create_path(al_get_native_file_dialog_path(chooser, 0));
                const char * path_str = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
                if(path_str)
                {
                    log_debug("tape_load_ui: path_str='%s'\n", path_str);
                    callback_handlers.handler_load_tape(path_str);
                } 
                else
                {
                    log_debug("tape_load_ui: path_str is NULL\n");
                }
                tape_fn = path;
            }
        }
    }
}

static void file_save_state(ALLEGRO_EVENT *event, const char * title, const char * patterns)
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

void menu_init(ALLEGRO_DISPLAY *display)
{
    log_debug("menu_init\n");
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

extern void tape_speed_set(uint8_t new_speed);

uint32_t menu_handle_event(ALLEGRO_EVENT *event)
{
    uint32_t elkEvent = 0;
    switch(menu_get_id(event)) 
    {
        case IDM_ZERO:
            break;
        case IDM_FILE_EXIT:
            elkEvent = ELK_EVENT_EXIT;
            break;
        case IDM_FILE_HARD_RESET:
            elkEvent = ELK_EVENT_RESET;
            break;
        case IDM_FILE_LOAD_STATE:
            file_load_state(event);
            break;
        case IDM_FILE_SAVE_STATE:
            file_save_state(event, "Save state to file", "*.snp");
            break;
        case IDM_TAPE_LOAD:
            tape_load_ui(event, "Choose a tape to load", "*.uef;*.csw");
            elkEvent = ELK_EVENT_MENU_ITEM_STATE_CHANGE;
            break;
        case IDM_TAPE_REWIND:
            callback_handlers.rewind_tape();
            break;
        case IDM_TAPE_EJECT:
            callback_handlers.eject_tape();
            break;
        case IDM_TAPE_SPEED:
            tape_speed_set(radio_event_simple(event, elkConfig.tape.speed));
            break;

        case IDM_SETTINGS_VIDEO_DISPLAY:
            video_display_set(radio_event_simple(event, elkConfig.display.drawmode));
            break;

        case IDM_SETTINGS_MEMORY_MASTER_RAM_BOARD:
            elkConfig.expansion.mrb ^= 1;
            elkEvent = ELK_EVENT_RESET;

        case IDM_SETTINGS_MEMORY_MRB_MODE:
            master_ram_board_mode_set(radio_event_simple(event, elkConfig.expansion.mrbmode));
            elkEvent = ELK_EVENT_RESET;
            break;

        case IDM_SETTINGS_DISC_PLUS3_ENABLE:
            elkConfig.expansion.plus3 = 1;
            elkEvent = ELK_EVENT_RESET;
            break;

        case IDM_SETTINGS_DISC_ADFS_ENABLE:
        {
            // TODO: Identiy of create helper function
            elkConfig.expansion.adfsena = 1;
            uncheck_menu_item((ALLEGRO_MENU *)(event->user.data3), IDM_SETTINGS_DISC_DFS_ENABLE);
            //ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);
            //if(elkConfig.expansion.dfsena)
            //{
            //    al_set_menu_item_flags(menu, IDM_SETTINGS_DISC_DFS_ENABLE, ALLEGRO_MENU_ITEM_CHECKBOX);
            //    elkConfig.expansion.dfsena = 0;
            //}
            elkEvent = ELK_EVENT_RESET;
            break;
        }

        case IDM_SETTINGS_DISC_DFS_ENABLE:
        {
            // TODO: Identiy of create helper function
            elkConfig.expansion.dfsena = 1;
            uncheck_menu_item((ALLEGRO_MENU *)(event->user.data3), IDM_SETTINGS_DISC_ADFS_ENABLE);
            //ALLEGRO_MENU *menu = ;
            //if(elkConfig.expansion.adfsena)
            //{
            //    al_set_menu_item_flags(menu, IDM_SETTINGS_DISC_ADFS_ENABLE, ALLEGRO_MENU_ITEM_CHECKBOX);
            //    elkConfig.expansion.adfsena = 0;
            //}
            elkEvent = ELK_EVENT_RESET;
            break;
        }
    }
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