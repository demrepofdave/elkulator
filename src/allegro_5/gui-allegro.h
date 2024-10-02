#ifndef __INC_GUI_ALLEGRO_H
#define __INC_GUI_ALLEGRO_H

typedef enum {
    IDM_ZERO,
    IDM_FILE_HARD_RESET,
    IDM_FILE_LOAD_STATE,
    IDM_FILE_SAVE_STATE,
    IDM_FILE_EXIT,
    IDM_EDIT_PASTE,
    IDM_EDIT_COPY,
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
    IDM_TAPE_LOAD,
    IDM_TAPE_REWIND,
    IDM_TAPE_EJECT,
    IDM_TAPE_CAT,
    IDM_TAPE_SPEED_NORMAL,
    IDM_TAPE_SPEED_FAST,
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

extern void gui_allegro_init(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display);
extern void gui_allegro_destroy(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display);
extern uint32_t gui_allegro_event(ALLEGRO_EVENT *event);

#endif
