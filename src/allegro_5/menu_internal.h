#ifndef __INC_GUI_ALLEGRO_H
#define __INC_GUI_ALLEGRO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "event_handler_internal.h"

typedef enum {
    IDM_ZERO,
    IDM_FILE_HARD_RESET,
    IDM_FILE_LOAD_STATE,
    IDM_FILE_SAVE_STATE,
    IDM_FILE_EXIT,
//    IDM_EDIT_PASTE,
//    IDM_EDIT_COPY,
    IDM_TAPE_LOAD,
    IDM_TAPE_REWIND,
    IDM_TAPE_EJECT,
    IDM_TAPE_CAT,
    IDM_TAPE_SPEED,
    IDM_DISC_LOAD_0_2,
    IDM_DISC_LOAD_1_3,
    IDM_DISC_EJECT_0_2,
    IDM_DISC_EJECT_1_3,
    IDM_DISC_WRITE_PROTECT_0_2,
    IDM_DISC_WRITE_PROTECT_1_3,
    IDM_DISC_WRITE_PROTECT_DEFAULT,
    IDM_ROMS_LOAD_CARTRIDGE1,
    IDM_ROMS_LOAD_CARTRIDGE2,
    IDM_ROMS_UNLOAD,
    IDM_ROMCARTS_MEGAGAMES,
    IDM_ROMCARTS_DAVIDS_FLASH_ROM,
    IDM_SETTINGS_VIDEO_FULLSCREEN,
    IDM_SETTINGS_VIDEO_DISPLAY,
    IDM_SETTINGS_SOUND_INTERNAL_SPEAKER,
    IDM_SETTINGS_SOUND_CSS_EXPANSION,
    IDM_SETTINGS_SOUND_DISC_DRIVE_NOISE,
    IDM_SETTINGS_SOUND_TAPE_NOISE,
    IDM_SETTINGS_SOUND_DISC_DRIVE_TYPE,
    IDM_SETTINGS_SOUND_DISC_DRIVE_VOLUME,
    IDM_SETTINGS_MEMORY_TURBO,
    IDM_SETTINGS_MEMORY_MASTER_RAM_BOARD,
    IDM_SETTINGS_MEMORY_JIM_PAGED_RAM,
    IDM_SETTINGS_MEMORY_MRB_MODE,
    IDM_SETTINGS_MEMORY_ENHANCED_ULA_STANDARD,
    IDM_SETTINGS_MEMORY_ENHANCED_ULA_8BITDUEL,
    IDM_SETTINGS_MEMORY_ENHANCED_ULA_9BITSINGLE,
    IDM_SETTINGS_DISC_PLUS3_ENABLE,
    IDM_SETTINGS_DISC_ADFS_ENABLE,
    IDM_SETTINGS_DISC_DFS_ENABLE,
    IDM_SETTINGS_JOYSTICK_PLUS1,
    IDM_SETTINGS_JOYSTICK_FIRSTBYTE,
    IDM_SETTINGS_KEYBOARD_REDEFINE,
    IDM_SETTINGS_MISC_SCREENSHOT,
    IDM_SETTINGS_MISC_START_MOVIE,
    IDM_SETTINGS_MISC_STOP_MOVIE,
    IDM_SETTINGS_MISC_START_DEBUGGING,
    IDM_DISC_AUTOBOOT,
    IDM_DISC_LOAD,
    IDM_DISC_EJECT,
    IDM_DISC_MMB_LOAD,
    IDM_DISC_MMB_EJECT,
    IDM_DISC_MMC_LOAD,
    IDM_DISC_MMC_EJECT,
    IDM_DISC_NEW_ADFS_S,
    IDM_DISC_NEW_ADFS_M,
    IDM_DISC_NEW_ADFS_L,
    IDM_DISC_NEW_DFS_10S_SIN_40T,
    IDM_DISC_NEW_DFS_10S_INT_40T,
    IDM_DISC_NEW_DFS_10S_SIN_80T,
    IDM_DISC_NEW_DFS_10S_INT_80T,
    IDM_DISC_NEW_DFS_16S_SIN_40T,
    IDM_DISC_NEW_DFS_16S_SIN_80T,
    IDM_DISC_NEW_DFS_16S_INT_80T,
    IDM_DISC_NEW_DFS_18S_SIN_40T,
    IDM_DISC_NEW_DFS_18S_SIN_80T,
    IDM_DISC_NEW_DFS_18S_INT_80T,
    IDM_DISC_WPROT,
    IDM_DISC_WPROT_D,
    IDM_DISC_VDFS_ENABLE,
    IDM_DISC_VDFS_ROOT,

    IDM_ROMS_LOAD,
    IDM_ROMS_CLEAR,
    IDM_ROMS_RAM,
    IDM_MODEL,
    IDM_VIDEO_DISPTYPE,
    IDM_VIDEO_PAL,
    IDM_VIDEO_BORDERS,
    IDM_VIDEO_WIN_MULT,
    IDM_VIDEO_WINSIZE,
    IDM_VIDEO_FULLSCR,
    IDM_VIDEO_NULA,
    IDM_VIDEO_LED_LOCATION,
    IDM_VIDEO_LED_VISIBILITY,
    IDM_VIDEO_MODE7_FONT,
    IDM_SOUND_INTERNAL,
    IDM_SOUND_BEEBSID,
    IDM_SOUND_PAULA,
    IDM_SOUND_DAC,
    IDM_SOUND_DDNOISE,
    IDM_SOUND_TAPE,
    IDM_SOUND_FILTER,
    IDM_WAVE,
    IDM_DISC_TYPE,
    IDM_DISC_VOL,
#ifdef HAVE_JACK_JACK_H
    IDM_MIDI_M4000_JACK,
    IDM_MIDI_M2000_OUT2_JACK,
    IDM_MIDI_M2000_OUT1_JACK,
    IDM_MIDI_M2000_OUT3_JACK,
#endif
#ifdef HAVE_ALSA_ASOUNDLIB_H
    IDM_MIDI_M4000_ASEQ,
    IDM_MIDI_M4000_ARAW,
    IDM_MIDI_M2000_OUT1_ASEQ,
    IDM_MIDI_M2000_OUT1_ARAW,
    IDM_MIDI_M2000_OUT2_ASEQ,
    IDM_MIDI_M2000_OUT2_ARAW,
    IDM_MIDI_M2000_OUT3_ASEQ,
    IDM_MIDI_M2000_OUT3_ARAW,
#endif
    IDM_KEY_REDEFINE,
    IDM_KEY_AS,
    IDM_KEY_LOGICAL,
    IDM_KEY_PAD,
    IDM_JIM_SIZE,
    IDM_AUTO_PAUSE,
    IDM_MOUSE_AMX,
    IDM_TRIACK_SEGA_ADAPTER,
    IDM_MOUSE_STICK,
    IDM_JOYMAP,
    IDM_JOYMAP2,
    IDM_JOYSTICK,
    IDM_JOYSTICK2,
    IDM_SPEED,
    IDM_AUTOSKIP,
    IDM_DEBUGGER,
    IDM_DEBUG_BREAK
} menu_id_t;


// Helper functions
void add_checkbox_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, bool checked, callback_event_handler_t menu_handler_function);
void add_radio_set(ALLEGRO_MENU *parent, char const **labels, uint16_t id, int cur_value,  callback_event_handler_t menu_handler_function);
void disable_menu_item(ALLEGRO_MENU *menu, int id);
void check_menu_item_id_num(ALLEGRO_MENU *menu, int id, int num);
void uncheck_menu_item(ALLEGRO_MENU *menu, int id);
void uncheck_menu_item_id_num(ALLEGRO_MENU *menu, int id, int num);
bool append_menu_item(ALLEGRO_MENU *menu, const char * title, uint16_t id, int flags, callback_event_handler_t menu_handler_function);
void append_menu_separator(ALLEGRO_MENU *menu);

bool register_menu_event_handler(int id, callback_event_handler_t menu_handler);

int radio_event_simple(ALLEGRO_EVENT *event, int current);
ALLEGRO_PATH * menu_load_gui(ALLEGRO_EVENT *event, const char * title, const char * patterns, ALLEGRO_PATH * starting_path);

// Menu creation functions
ALLEGRO_MENU *create_file_menu(void);
ALLEGRO_MENU *create_tape_menu(void);
ALLEGRO_MENU *create_disc_menu(void);
ALLEGRO_MENU *create_roms_menu(void);
ALLEGRO_MENU *create_settings_menu(void);
ALLEGRO_MENU *create_misc_menu(void);

// General main menu handling functions.

extern void menu_init(ALLEGRO_DISPLAY *display);
extern void menu_destroy(ALLEGRO_DISPLAY *display);

elk_event_t  menu_handle_event(ALLEGRO_EVENT *event);

#endif
