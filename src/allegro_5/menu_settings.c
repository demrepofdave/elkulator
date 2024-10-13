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
#include "menu_internal.h"


/******************************************************************************
* Private Variable Definitions
*******************************************************************************/
static const char *settings_memory_master_ramboard_items[] = { "Off",
                                                               "Turbo",
                                                               "Shadow", NULL };

static const char *settings_video_display_type[] = { "Scanlines", 
                                                     "Line doubling (disabled)", 
                                                     "2xSai (disabled)",
                                                     "Scale2X (disabled)",
                                                     "Super Eagle (disabled)",
                                                     "PAL Filter (disabled)", NULL };

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

//static ALLEGRO_MENU *create_settings_memory_masterramboard_menu(void)
//{
//    ALLEGRO_MENU *menu = al_create_menu();
//    al_append_menu_item(menu, "Off",    IDM_SETTINGS_MEMORY_MRB_OFF,    ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
//    al_append_menu_item(menu, "Turbo",  IDM_SETTINGS_MEMORY_MRB_TURBO,  ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
//    al_append_menu_item(menu, "Shadow", IDM_SETTINGS_MEMORY_MRB_SHADOW, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
//    return menu;
//}

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

    //disable_menu_item(sub_menu_settings_display_type, -2);

    al_append_menu_item(menu, "Display type",   0,  0, NULL, sub_menu_display_type);

    add_radio_set(sub_menu_display_type, settings_video_display_type, IDM_SETTINGS_VIDEO_DISPLAY, elkConfig.display.drawmode);

    al_append_menu_item(menu, "Fullscreen",            IDM_SETTINGS_VIDEO_FULLSCREEN, ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

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

static ALLEGRO_MENU *create_settings_memory_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    ALLEGRO_MENU *sub_menu_master_ram_board_mode = al_create_menu();

    al_append_menu_item(menu, "Elektuur/Slogger turbo board",  IDM_SETTINGS_MEMORY_TURBO,            ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    al_append_menu_item(menu, "Slogger/Jafa Master RAM board", IDM_SETTINGS_MEMORY_MASTER_RAM_BOARD, 0, NULL, NULL);

    al_append_menu_item(menu, "Master RAM board mode",   0,  0, NULL, sub_menu_master_ram_board_mode);

    add_radio_set(sub_menu_master_ram_board_mode, settings_memory_master_ramboard_items, IDM_SETTINGS_MEMORY_MRB_MODE, elkConfig.expansion.mrbmode);

    al_append_menu_item(menu, "Enhanced ULA mode",        0, 0, NULL, create_settings_memory_enhancedula_menu());
    al_append_menu_item(menu, "Paged RAM in FD (JIM)",         IDM_SETTINGS_MEMORY_JIM_PAGED_RAM,   ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL);
    return menu;
}

static ALLEGRO_MENU *create_settings_disc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Plus 3 enable", IDM_SETTINGS_DISC_PLUS3_ENABLE, elkConfig.expansion.plus3);
    add_checkbox_item(menu, "ADFS enable",   IDM_SETTINGS_DISC_ADFS_ENABLE,  elkConfig.expansion.adfsena);
    add_checkbox_item(menu, "DFS enable",    IDM_SETTINGS_DISC_DFS_ENABLE,   elkConfig.expansion.dfsena);
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