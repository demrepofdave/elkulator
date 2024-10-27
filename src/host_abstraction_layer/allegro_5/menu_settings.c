/*
 * Elkulator - An electron emulator originally written 
 *              by Sarah Walker
 *
 * menu_settings.c
 * 
 * This handles the creation of the Settings section of the native menu.
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>

#include "config_vars.h"
#include "logger.h"
#include "menu_internal.h"

#include "host_abstraction_layer/event_handler.h"
#include "host_abstraction_layer/video.h"

elk_event_t menu_handle_video_display_set(ALLEGRO_EVENT * event);
elk_event_t menu_handle_toggle_aspect_ratio(ALLEGRO_EVENT * event);

elk_event_t menu_handle_toggle_internal_sound(ALLEGRO_EVENT * event);
elk_event_t menu_handle_toggle_sound_css_expansion(ALLEGRO_EVENT * event);
elk_event_t menu_handle_toggle_sound_dd_noise(ALLEGRO_EVENT * event);
elk_event_t menu_handle_toggle_sound_tape_noise(ALLEGRO_EVENT * event);
elk_event_t menu_handle_disc_drive_type(ALLEGRO_EVENT * event);
elk_event_t menu_handle_disc_drive_volume(ALLEGRO_EVENT * event);

elk_event_t menu_handle_memory_turbo_toggle(ALLEGRO_EVENT * event);
elk_event_t menu_handle_master_ram_board_enable(ALLEGRO_EVENT * event);
elk_event_t menu_handle_master_ram_board_mode(ALLEGRO_EVENT * event);
elk_event_t menu_handle_toggle_paged_ram_jim(ALLEGRO_EVENT * event);

elk_event_t menu_handle_plus_3_enable(ALLEGRO_EVENT * event);
elk_event_t menu_handle_adfs_enable(ALLEGRO_EVENT * event);
elk_event_t menu_handle_dfs_enable(ALLEGRO_EVENT * event);

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

#define VIDEO_DISPLAY_TYPE_MAX   4
static const char *settings_video_display_type[VIDEO_DISPLAY_TYPE_MAX] = { "Scanlines",
                                                     "Line doubling",
//                                                     "2xSai (disabled)",
//                                                     "Scale2X (disabled)",
//                                                     "Super Eagle (disabled)",
                                                     "PAL Filter", NULL };

#define DISC_DRIVE_TYPE_MAX  3

static const char *settings_disc_drive_type_names[DISC_DRIVE_TYPE_MAX] = { "5.25",
                                                        "3.5", NULL };

#define DISC_DRIVE_VOL_TYPE_MAX  4
static const char *settings_disc_drive_volume_names[DISC_DRIVE_VOL_TYPE_MAX] = { "33%",
                                                        "66%",
                                                        "100%", NULL };

#define RAMBOARD_ITEM_STANDARD_ELK           0
#define RAMBOARD_ITEM_ELECTUUR_SLOGGER_TURBO 1
#define RAMBOARD_MRB_TURBO                   2
#define RAMBOARD_MRB_SHADOW                  3

#define RAMBOARD_NAMES_MAX 5

static const char *settings_ramboard_names[RAMBOARD_NAMES_MAX] = {
                                 "Standard Electron",
                                 "Elektuur/Slogger turbo board",
                                 "Slogger/Jafa Master RAM board - Turbo",
                                 "Slogger/Jafa Master RAM board - Shadow", NULL };

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

static ALLEGRO_MENU *create_settings_memory_enhancedula_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Standard",                        IDM_SETTINGS_MEMORY_ENHANCED_ULA_STANDARD,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Enhanced (8-bit, dual access)",   IDM_SETTINGS_MEMORY_ENHANCED_ULA_8BITDUEL,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Enhanced (8-bit, single access)", IDM_SETTINGS_MEMORY_ENHANCED_ULA_9BITSINGLE, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

static ALLEGRO_MENU *create_settings_video_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    ALLEGRO_MENU *sub_menu_display_type = al_create_menu();

    add_checkbox_item(menu, "Maintain Aspect Ratio",    IDM_SETTINGS_VIDEO_MAINTAIN_RATIO, elkConfig.sound.sndint,      menu_handle_toggle_aspect_ratio);
    //disable_menu_item(sub_menu_settings_display_type, -2);

    al_append_menu_item(menu, "Display type",   0,  0, NULL, sub_menu_display_type);

    add_radio_set(sub_menu_display_type, settings_video_display_type, IDM_SETTINGS_VIDEO_DISPLAY, elkConfig.display.drawmode, menu_handle_video_display_set);

    al_append_menu_item(menu, "Fullscreen",            IDM_SETTINGS_VIDEO_FULLSCREEN, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

static ALLEGRO_MENU *create_settings_sound_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    ALLEGRO_MENU *sub_menu_disc_drive_type = al_create_menu();
    ALLEGRO_MENU *sub_menu_disc_drive_volume = al_create_menu();

    add_checkbox_item(menu, "Internal speaker",    IDM_SETTINGS_SOUND_INTERNAL_SPEAKER, elkConfig.sound.sndint,      menu_handle_toggle_internal_sound);
    add_checkbox_item(menu, "CSS Sound Expansion", IDM_SETTINGS_SOUND_CSS_EXPANSION,     elkConfig.sound.sndex,      menu_handle_toggle_sound_css_expansion);
    add_checkbox_item(menu, "Disc drive noise",    IDM_SETTINGS_SOUND_DISC_DRIVE_NOISE,  elkConfig.sound.sndddnoise, menu_handle_toggle_sound_dd_noise);   // TODO: When disc noise is played this crashes elkulator
    add_checkbox_item(menu, "Tape noise",          IDM_SETTINGS_SOUND_TAPE_NOISE,        elkConfig.sound.sndtape,    menu_handle_toggle_sound_tape_noise); // TODO: Tape noise and fast mode = lots of noise - not turning off correctly.
    append_menu_separator(menu);
    
    al_append_menu_item(menu, "Disc drive type",   0,  0, NULL, sub_menu_disc_drive_type);
    add_radio_set(sub_menu_disc_drive_type, settings_disc_drive_type_names, IDM_SETTINGS_SOUND_DISC_DRIVE_TYPE, elkConfig.sound.ddtype,  menu_handle_disc_drive_type);

    al_append_menu_item(menu, "Disc drive volume",   0,  0, NULL, sub_menu_disc_drive_volume);
    add_radio_set(sub_menu_disc_drive_volume, settings_disc_drive_volume_names, IDM_SETTINGS_SOUND_DISC_DRIVE_VOLUME, elkConfig.sound.ddtype,  menu_handle_disc_drive_volume);
    return menu;
}

static ALLEGRO_MENU *create_settings_memory_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    //add_checkbox_item(menu, "Elektuur/Slogger turbo board",  IDM_SETTINGS_MEMORY_TURBO,            elkConfig.expansion.turbo, menu_handle_memory_turbo_toggle);
    //append_menu_separator(menu);

    // Calculate starting setting for next menu item.
    uint8_t item = RAMBOARD_ITEM_STANDARD_ELK; // Default to standard electron
    if(elkConfig.expansion.turbo)
    {
        item = RAMBOARD_ITEM_ELECTUUR_SLOGGER_TURBO; // Elektuur/Slogger turbo board
    }
    else if(elkConfig.expansion.mrb && elkConfig.expansion.mrbmode > 0)
    {
        item = elkConfig.expansion.mrbmode + 1; // Master RAM board active.
    }

    add_radio_set(menu, settings_ramboard_names, IDM_SETTINGS_MEMORY_MRB_MODE, item, menu_handle_master_ram_board_mode);

    append_menu_separator(menu);

    al_append_menu_item(menu, "Enhanced ULA mode",        0, 0, NULL, create_settings_memory_enhancedula_menu());
    add_checkbox_item  (menu, "Paged RAM in FD (JIM)",         IDM_SETTINGS_MEMORY_JIM_PAGED_RAM, elkConfig.expansion.enable_jim, menu_handle_toggle_paged_ram_jim);

    return menu;
}

static ALLEGRO_MENU *create_settings_disc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Plus 3 enable", IDM_SETTINGS_DISC_PLUS3_ENABLE, elkConfig.expansion.plus3,   menu_handle_plus_3_enable);
    add_checkbox_item(menu, "ADFS enable",   IDM_SETTINGS_DISC_ADFS_ENABLE,  elkConfig.expansion.adfsena, menu_handle_adfs_enable);
    add_checkbox_item(menu, "DFS enable",    IDM_SETTINGS_DISC_DFS_ENABLE,   elkConfig.expansion.dfsena,  menu_handle_dfs_enable);
    return menu;
}

static ALLEGRO_MENU *create_settings_joystick_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Plus 1 joystick interface",     IDM_SETTINGS_JOYSTICK_PLUS1, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "First Byte joystick interface", IDM_SETTINGS_JOYSTICK_FIRSTBYTE,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}


static ALLEGRO_MENU *create_settings_keyboard_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Redefine keyboard", IDM_SETTINGS_KEYBOARD_REDEFINE,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

ALLEGRO_MENU *create_settings_menu(void)
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


// Menu event handler functions.

// Called when IDM_SETTINGS_VIDEO_DISPLAY event is recieved.
elk_event_t menu_handle_video_display_set(ALLEGRO_EVENT * event)
{
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);
    elkConfig.display.drawmode = menu_get_num(event);
    update_radio_set(event, VIDEO_DISPLAY_TYPE_MAX - 1);
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_SOUND_INTERNAL_SPEAKER event is recieved.
elk_event_t menu_handle_toggle_aspect_ratio(ALLEGRO_EVENT * event)
{
    elkConfig.display.maintain_aspect_ratio=!elkConfig.display.maintain_aspect_ratio;
    video_resize_elk_window((elkConfig.display.maintain_aspect_ratio == 1));
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_SOUND_INTERNAL_SPEAKER event is recieved.
elk_event_t menu_handle_toggle_internal_sound(ALLEGRO_EVENT * event)
{
    elkConfig.sound.sndint=!elkConfig.sound.sndint;
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_SOUND_CSS_EXPANSION event is recieved.
elk_event_t menu_handle_toggle_sound_css_expansion(ALLEGRO_EVENT * event)
{
    elkConfig.sound.sndex=!elkConfig.sound.sndex;
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_SOUND_DISC_DRIVE_NOISE event is recieved.
elk_event_t menu_handle_toggle_sound_dd_noise(ALLEGRO_EVENT * event)
{
    elkConfig.sound.sndddnoise=!elkConfig.sound.sndddnoise;
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_SOUND_TAPE_NOISE event is recieved.
elk_event_t menu_handle_toggle_sound_tape_noise(ALLEGRO_EVENT * event)
{
    elkConfig.sound.sndtape=!elkConfig.sound.sndtape;
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_SOUND_DISC_DRIVE_TYPE event is recieved.
elk_event_t menu_handle_disc_drive_type(ALLEGRO_EVENT * event)
{
    elkConfig.sound.ddtype = menu_get_num(event);
    update_radio_set(event, DISC_DRIVE_TYPE_MAX - 1);
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_SOUND_DISC_DRIVE_VOLUME event is recieved.
elk_event_t menu_handle_disc_drive_volume(ALLEGRO_EVENT * event)
{
    elkConfig.sound.ddvol = menu_get_num(event);
    update_radio_set(event, DISC_DRIVE_VOL_TYPE_MAX - 1);
    return(ELK_EVENT_NONE);
}

// Called when IDM_SETTINGS_MEMORY_MASTER_RAM_BOARD event is recieved.
elk_event_t menu_handle_master_ram_board_enable(ALLEGRO_EVENT * event)
{
    elkConfig.expansion.mrb = !elkConfig.expansion.mrb;
    return(ELK_EVENT_RESET);
}

// Called when IDM_SETTINGS_MEMORY_TURBO event is recieved.
elk_event_t menu_handle_memory_turbo_toggle(ALLEGRO_EVENT * event)
{
    ALLEGRO_MENU * menu = (ALLEGRO_MENU *)(event->user.data3);
    elkConfig.expansion.turbo = !elkConfig.expansion.turbo;
    if (elkConfig.expansion.turbo)
    {
        elkConfig.expansion.mrb = 0;
        check_menu_item_id_num  (menu, IDM_SETTINGS_MEMORY_MRB_MODE, 0);
        uncheck_menu_item_id_num(menu, IDM_SETTINGS_MEMORY_MRB_MODE, 1);
        uncheck_menu_item_id_num(menu, IDM_SETTINGS_MEMORY_MRB_MODE, 2);
    }
    return(ELK_EVENT_RESET);
}

// Called when IDM_SETTINGS_MEMORY_MRB_MODE event is recieved.
elk_event_t menu_handle_master_ram_board_mode(ALLEGRO_EVENT * event)
{
    ALLEGRO_MENU * menu = (ALLEGRO_MENU *)(event->user.data3);
    int item = menu_get_num(event);
    update_radio_set(event, RAMBOARD_NAMES_MAX - 1);

    switch(item)
    {
        case RAMBOARD_ITEM_STANDARD_ELK:
            elkConfig.expansion.turbo   = 0;
            elkConfig.expansion.mrb     = 0;
            elkConfig.expansion.mrbmode = 0;
            break;

        case RAMBOARD_ITEM_ELECTUUR_SLOGGER_TURBO:
            elkConfig.expansion.turbo   = 1;
            elkConfig.expansion.mrb     = 0;
            elkConfig.expansion.mrbmode = 0;
            break;

        case RAMBOARD_MRB_TURBO:
            elkConfig.expansion.turbo   = 0;
            elkConfig.expansion.mrb     = 1;
            elkConfig.expansion.mrbmode = 1;
            break;

        case RAMBOARD_MRB_SHADOW:
            elkConfig.expansion.turbo   = 0;
            elkConfig.expansion.mrb     = 1;
            elkConfig.expansion.mrbmode = 2;
            break;

        default:
            break;
    }

    return(ELK_EVENT_RESET);
}

// Called when IDM_SETTINGS_MEMORY_MRB_MODE event is recieved.
elk_event_t menu_handle_toggle_paged_ram_jim(ALLEGRO_EVENT * event)
{
    elkConfig.expansion.enable_jim = !elkConfig.expansion.enable_jim;
    return(ELK_EVENT_RESET);
}


// Called when IDM_SETTINGS_DISC_PLUS3_ENABLE event is recieved.
elk_event_t menu_handle_plus_3_enable(ALLEGRO_EVENT * event)
{
    elkConfig.expansion.plus3 = !elkConfig.expansion.plus3;
    return(ELK_EVENT_RESET);
}
// Called when IDM_SETTINGS_DISC_ADFS_ENABLE event is recieved.
elk_event_t menu_handle_adfs_enable(ALLEGRO_EVENT * event)
{
    ALLEGRO_MENU * menu = (ALLEGRO_MENU *)(event->user.data3);
    elkConfig.expansion.adfsena = !elkConfig.expansion.adfsena;
    //uncheck_menu_item(menu, IDM_SETTINGS_DISC_DFS_ENABLE);
    if(elkConfig.expansion.adfsena && elkConfig.expansion.dfsena)
    {
        elkConfig.expansion.dfsena = 0;
        al_set_menu_item_flags(menu, IDM_SETTINGS_DISC_DFS_ENABLE, ALLEGRO_MENU_ITEM_CHECKBOX);
    }
    return(ELK_EVENT_RESET);
}
// Called when IDM_SETTINGS_DISC_DFS_ENABLE event is recieved.
elk_event_t menu_handle_dfs_enable(ALLEGRO_EVENT * event)
{
    ALLEGRO_MENU * menu = (ALLEGRO_MENU *)(event->user.data3);
    elkConfig.expansion.dfsena ^= 1;
    //uncheck_menu_item(menu, IDM_SETTINGS_DISC_ADFS_ENABLE);
    if(elkConfig.expansion.dfsena && elkConfig.expansion.adfsena)
    {
        elkConfig.expansion.adfsena = 0;
        al_set_menu_item_flags(menu, IDM_SETTINGS_DISC_ADFS_ENABLE, ALLEGRO_MENU_ITEM_CHECKBOX);
    }
    return(ELK_EVENT_RESET);
}