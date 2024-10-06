#include <allegro5/allegro_native_dialog.h>
#include "common/video.h"
#include "gui-allegro.h"
#include "logger.h"
#include "callback_handlers.h"
#include "config_vars.h"

/* pclose() and popen() on Windows are _pclose() and _popen() */
#ifdef _WIN32
#  define pclose _pclose
#  define popen _popen
#endif

typedef struct {
    const char *label;
    int itemno;
} menu_map_t;

// Private functions.

static inline int menu_id_num(menu_id_t id, int num)
{
    return (num << 8) | id;
}

static inline menu_id_t menu_get_id(ALLEGRO_EVENT *event)
{
    return event->user.data1 & 0xff;
}

static inline int menu_get_num(ALLEGRO_EVENT *event)
{
    return event->user.data1 >> 8;
}

static void add_checkbox_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, bool checked)
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

static void add_radio_set(ALLEGRO_MENU *parent, char const **labels, uint16_t id, int cur_value)
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

// Variable and config handlers

static void tape_speed_set(ALLEGRO_EVENT *event)
{
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);
    // TODO: Working code but clean up a bit later.
    if(elkConfig.tape.speed == TAPE_NORMAL && menu_get_id(event) != IDM_TAPE_SPEED_NORMAL)
    {
        al_set_menu_item_flags(menu, IDM_TAPE_SPEED_NORMAL, ALLEGRO_MENU_ITEM_CHECKBOX);
    }

    if(elkConfig.tape.speed == TAPE_FAST && menu_get_id(event) != IDM_TAPE_SPEED_FAST)
    {
        al_set_menu_item_flags(menu, IDM_TAPE_SPEED_FAST, ALLEGRO_MENU_ITEM_CHECKBOX);
    }

    if(elkConfig.tape.speed == TAPE_REALLY_FAST && menu_get_id(event) != IDM_TAPE_SPEED_REALLY_FAST)
    {
        al_set_menu_item_flags(menu, IDM_TAPE_SPEED_REALLY_FAST, ALLEGRO_MENU_ITEM_CHECKBOX);
    }

    switch(menu_get_id(event))
    {
        case IDM_TAPE_SPEED_NORMAL:
            elkConfig.tape.speed = TAPE_NORMAL;
            break;
        case IDM_TAPE_SPEED_FAST:
            elkConfig.tape.speed = TAPE_FAST;
            break;
        case IDM_TAPE_SPEED_REALLY_FAST:
            elkConfig.tape.speed = TAPE_REALLY_FAST;
            break;

    }

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


/*MENU miscmenu[5]=
{
        {"Save screenshot",gui_scrshot,NULL,0,NULL},
        {"Start movie",gui_startmovie,NULL,0,NULL},
        {"Stop movie",gui_stopmovie,NULL,0,NULL},
        {"Start debugging",gui_startdebugging,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_misc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Save screenshot", IDM_SETTINGS_MISC_SCREENSHOT,      ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Start movie",     IDM_SETTINGS_MISC_START_MOVIE,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Stop movie",      IDM_SETTINGS_MISC_STOP_MOVIE,      ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Start debugging", IDM_SETTINGS_MISC_START_DEBUGGING, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}
/*
MENU displaymenu[7]=
{
        {"Scanlines",gui_disp,NULL,0,(void *)0},
        {"Line doubling",gui_disp,NULL,0,(void *)1},
        {"2xSaI",gui_disp,NULL,0,(void *)2},
        {"Scale2X",gui_disp,NULL,0,(void *)3},
        {"Super Eagle",gui_disp,NULL,0,(void *)4},
        {"PAL Filter",gui_disp,NULL,0,(void *)5},
        {NULL,NULL,NULL,0,NULL}
};
*/
static ALLEGRO_MENU *create_settings_video_displaytype_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Scanlines",     IDM_SETTINGS_VIDEO_DISPLAY_SCANLINES,        0, NULL, NULL);
    al_append_menu_item(menu, "Line doubling", IDM_SETTINGS_VIDEO_DISPLAY_LINEDOUBLING, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "2xSaI",         IDM_SETTINGS_VIDEO_DISPLAY_2XSAI,        ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Scale2X",       IDM_SETTINGS_VIDEO_DISPLAY_SCALE2X,      ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Super Eagle",   IDM_SETTINGS_VIDEO_DISPLAY_SUPEREAGLE,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "PAL Filter",    IDM_SETTINGS_VIDEO_DISPLAY_PALFILTER,    ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}
/*
MENU videomenu[3]=
{
        {"Display type",NULL,displaymenu,0,NULL},
        {"Fullscreen",gui_fullscreen,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};
*/
static ALLEGRO_MENU *create_settings_video_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Display type",    0, 0, NULL, create_settings_video_displaytype_menu());
    al_append_menu_item(menu, "Fullscreen",            IDM_SETTINGS_VIDEO_FULLSCREEN, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*MENU soundmenu[7]=
{
        {"Internal speaker",gui_internalsnd,NULL,0,NULL},
        {"CSS Sound Expansion",gui_sndex,NULL,0,NULL},
        {"Disc drive noise",gui_ddnoise,NULL,0,NULL},
        {"Tape noise",gui_tnoise,NULL,0,NULL},
        {"Disc drive type",NULL,ddtypemenu,0,NULL},
        {"Disc drive volume",NULL,ddvolmenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};
*/
static ALLEGRO_MENU *create_settings_sound_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Internal speaker",    IDM_SETTINGS_SOUND_INTERNAL_SPEAKER,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "CSS Sound Expansion", IDM_SETTINGS_SOUND_CSS_EXPANSION,     ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Disc drive noise",    IDM_SETTINGS_SOUND_DISC_DRIVE_NOISE,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Tape noise",          IDM_SETTINGS_SOUND_TAPE_NOISE,        ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Disc drive type",     IDM_SETTINGS_SOUND_DISC_DRIVE_TYPE,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Disc drive volume",   IDM_SETTINGS_SOUND_DISC_DRIVE_VOLUME, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*MENU ulamenu[4]=
{
        {"&Standard",gui_ulamode,NULL,0,(void *)0},
        {"&Enhanced (8-bit, dual access)",gui_ulamode,NULL,0,(void *)ULA_RAM_8BIT_DUAL_ACCESS},
        {"&Enhanced (8-bit, single access)",gui_ulamode,NULL,0,(void *)ULA_RAM_8BIT_SINGLE_ACCESS},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_settings_memory_enhancedula_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Standard",                        IDM_SETTINGS_MEMORY_ENHANCED_ULA_STANDARD,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Enhanced (8-bit, dual access)",   IDM_SETTINGS_MEMORY_ENHANCED_ULA_8BITDUEL,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Enhanced (8-bit, single access)", IDM_SETTINGS_MEMORY_ENHANCED_ULA_9BITSINGLE, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*MENU mrbmenu[4]=
{
        {"&Off",gui_mrbmode,NULL,0,(void *)0},
        {"&Turbo",gui_mrbmode,NULL,0,(void *)1},
        {"&Shadow",gui_mrbmode,NULL,0,(void *)2},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_settings_memory_masterramboard_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Off",    IDM_SETTINGS_MEMORY_MRB_OFF,    ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Turbo",  IDM_SETTINGS_MEMORY_MRB_TURBO,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Shadow", IDM_SETTINGS_MEMORY_MRB_SHADOW, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*MENU memmenu[6]=
{
        {"&Elektuur/Slogger turbo board",gui_turbo,NULL,0,NULL},
        {"&Slogger/Jafa Master RAM board",gui_mrb,NULL,0,NULL},
        {"&Master RAM board mode",NULL,mrbmenu,0,NULL},
        {"Enhanced &ULA mode",NULL,ulamenu,0,NULL},
        {"&Paged RAM in FD (JIM)",gui_jim,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/

static ALLEGRO_MENU *create_settings_memory_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Elektuur/Slogger turbo board",  IDM_SETTINGS_MEMORY_TURBO,            ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Slogger/Jafa Master RAM board", IDM_SETTINGS_MEMORY_MASTER_RAM_BOARD, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Master RAM board mode",    0, 0, NULL, create_settings_memory_masterramboard_menu());
    al_append_menu_item(menu, "Enhanced ULA mode",        0, 0, NULL, create_settings_memory_enhancedula_menu());
    al_append_menu_item(menu, "Paged RAM in FD (JIM)",         IDM_SETTINGS_MEMORY_JIM_PAGED_RAM,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*MENU dischmenu[4]=
{
        {"&Plus 3 enable",gui_plus3,NULL,0,NULL},
        {"&ADFS enable",gui_adfs,NULL,0,NULL},
        {"&DFS enable",gui_dfs,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_settings_disc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Plus 3 enable", IDM_SETTINGS_DISC_PLUS3_ENABLE, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "ADFS enable",   IDM_SETTINGS_DISC_ADFS_ENABLE,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "DFS enable",    IDM_SETTINGS_DISC_DFS_ENABLE,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*
MENU joymenu[3]=
{
        {"&Plus 1 joystick interface",gui_plus1,NULL,0,NULL},
        {"&First Byte joystick interface",gui_first,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_settings_joystick_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Plus 1 joystick interface",     IDM_SETTINGS_JOYSTICK_PLUS1, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "First Byte joystick interface", IDM_SETTINGS_JOYSTICK_FIRSTBYTE,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*MENU keymenu[2]={
        {"Redefine keyboard",gui_keydefine,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/

static ALLEGRO_MENU *create_settings_keyboard_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Redefine keyboard", IDM_SETTINGS_KEYBOARD_REDEFINE,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/*
MENU settingsmenu[7]=
{
        {"&Video",NULL,videomenu,0,NULL},
        {"&Sound",NULL,soundmenu,0,NULL},
        {"&Memory",NULL,memmenu,0,NULL},
        {"&Disc",NULL,dischmenu,0,NULL},
        {"&Joystick",NULL,joymenu,0,NULL},
        {"&Keyboard",NULL,keymenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};
*/
static ALLEGRO_MENU *create_settings_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Video",    0, 0, NULL, create_settings_video_menu());
    al_append_menu_item(menu, "Sound",    0, 0, NULL, create_settings_sound_menu());
    al_append_menu_item(menu, "Memory",   0, 0, NULL, create_settings_memory_menu());
    al_append_menu_item(menu, "Disc",     0, 0, NULL, create_settings_disc_menu());
    al_append_menu_item(menu, "Joystick", 0, 0, NULL, create_settings_joystick_menu());
    al_append_menu_item(menu, "Keyboard", 0, 0, NULL, create_settings_keyboard_menu());
    return menu;
}
/*
MENU romcartsmenu[3]=
{
        {"Mega Games Cartridge",gui_mgc,0,0,NULL},
        {"David's Flash ROM Cartridge",gui_db_flash_cartridge,0,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};
*/
static ALLEGRO_MENU *create_romscarts_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Mega Games Cartridge",        IDM_ROMCARTS_MEGAGAMES,        ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "David's Flash ROM Cartridge", IDM_ROMCARTS_DAVIDS_FLASH_ROM, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}
/*
MENU rommenu[5]=
{
        {"Load ROM cartridge &1...",gui_romload0,NULL,0,NULL},
        {"Load ROM cartridge &2...",gui_romload1,NULL,0,NULL},
        {"&Unload ROM cartridges...",gui_romeject0,NULL,0,NULL},
        {"Multi-ROM expansions",NULL,romcartsmenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};
*/
static ALLEGRO_MENU *create_roms_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Load ROM cartridge &1...", IDM_ROMS_LOAD_CARTRIDGE1, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Load ROM cartridge &2...", IDM_ROMS_LOAD_CARTRIDGE2, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Unload ROM cartridges...", IDM_ROMS_UNLOAD,          ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Multi-ROM expansions",  0, 0, NULL, create_romscarts_menu());
    return menu;
}


/*MENU discmenu[8]=
{
        {"Load disc :&0/2...",gui_load0,NULL,0,NULL},
        {"Load disc :&1/3...",gui_load1,NULL,0,NULL},
        {"Eject disc :0/2",gui_eject0,NULL,0,NULL},
        {"Eject disc :1/3",gui_eject1,NULL,0,NULL},
        {"Write protect disc :0/2",gui_wprot0,NULL,0,NULL},
        {"Write protect disc :1/3",gui_wprot1,NULL,0,NULL},
        {"Default write protect",gui_wprotd,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_disc_menu(void)
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

/*MENU filemenu[6]=
{
        {"&Return",gui_return,NULL,0,NULL},
        {"&Hard reset",gui_reset,NULL,0,NULL},
        {"&Load state",gui_loads,NULL,0,NULL},
        {"&Save state",gui_saves,NULL,0,NULL},
        {"&Exit",gui_exit,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_file_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    al_append_menu_item(menu, "Hard Reset", IDM_FILE_HARD_RESET, 0, NULL, NULL);
    al_append_menu_item(menu, "Load state...", IDM_FILE_LOAD_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Save State...", IDM_FILE_SAVE_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Exit", IDM_FILE_EXIT, 0, NULL, NULL);
    return menu;
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

//MENU tapespdmenu[4]=
//{
//        {"Normal",gui_normal,NULL,0,NULL},
//        {"Fast",gui_fast,NULL,0,NULL},
//        {"Really Fast",gui_rfast,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_tape_speed_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Normal",      IDM_TAPE_SPEED_NORMAL,      (elkConfig.tape.speed == TAPE_NORMAL));
    add_checkbox_item(menu, "Fast",        IDM_TAPE_SPEED_FAST,        (elkConfig.tape.speed == TAPE_FAST));
    add_checkbox_item(menu, "Really Fast", IDM_TAPE_SPEED_REALLY_FAST, (elkConfig.tape.speed == TAPE_REALLY_FAST));
    return menu;
}

//MENU tapemenu[]=
//{
//        {"Load tape...",gui_loadt,NULL,0,NULL},
//        {"Rewind tape",gui_rewind,NULL,0,NULL},
//        {"Eject tape",gui_ejectt,NULL,0,NULL},
//        {"Tape speed",NULL,tapespdmenu,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_tape_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Load tape...", IDM_TAPE_LOAD,   0,                          NULL, NULL);
    al_append_menu_item(menu, "Rewind tape",  IDM_TAPE_REWIND, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Eject tape",   IDM_TAPE_EJECT,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Tape speed",   0,               0,                          NULL, create_tape_speed_menu());
    return menu;
}

// Public functions.

void gui_allegro_init(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display)
{
    log_debug("gui_allegro_init\n");
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "File",     0, 0, NULL, create_file_menu());
    al_append_menu_item(menu, "Tape",     0, 0, NULL, create_tape_menu());
    al_append_menu_item(menu, "Disc",     0, 0, NULL, create_disc_menu());
    al_append_menu_item(menu, "ROM",      0, 0, NULL, create_roms_menu());
    al_append_menu_item(menu, "Settings", 0, 0, NULL, create_settings_menu());
    al_append_menu_item(menu, "Misc",     0, 0, NULL, create_misc_menu());
    al_set_display_menu(display, menu);
    al_register_event_source(queue, al_get_default_menu_event_source());
}

void gui_allegro_destroy(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display)
{
    al_unregister_event_source(queue, al_get_default_menu_event_source());
    al_set_display_menu(display, NULL);
}

static const char all_dext[] = "*.ssd;*.dsd;*.img;*.adf;*.ads;*.adm;*.adl;*.sdd;*.ddd;*.fdi;*.imd;*.hfe;"
                               "*.SSD;*.DSD;*.IMG;*.ADF;*.ADS;*.ADM;*.ADL;*.SDD;*.DDD;*.FDI;*.IMD;*.HFE";

uint32_t gui_allegro_event(ALLEGRO_EVENT *event)
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
            break;
        case IDM_TAPE_REWIND:
            break;
        case IDM_TAPE_EJECT:
            break;
        case IDM_TAPE_SPEED_NORMAL:
        case IDM_TAPE_SPEED_FAST:
        case IDM_TAPE_SPEED_REALLY_FAST:
            tape_speed_set(event);
            break;
    }
    log_debug("gui_allegro_event elkEvent = 0x%02x\n", elkEvent);
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