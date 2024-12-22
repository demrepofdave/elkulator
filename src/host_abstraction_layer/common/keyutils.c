
#include <string.h>
#include "host_abstraction_layer/keyboard.h"
#include "logger.h"

typedef struct 
{
    const char * config_key_string;
    const char * key_longname;
} key_strings_t;

static key_strings_t elk_keycode_config_string[ELK_KEY_MAX] =
{
    { "",                     ""  },
    { "elk_key_0",            "0" },
    { "elk_key_1",            "1" },
    { "elk_key_2",            "2" },
    { "elk_key_3",            "3" },
    { "elk_key_4",            "4" },
    { "elk_key_5",            "5" },
    { "elk_key_6",            "6" },
    { "elk_key_7",            "7" },
    { "elk_key_8",            "8" },
    { "elk_key_9",            "9" },
    { "elk_key_a",            "A" },
    { "elk_key_b",            "B" },
    { "elk_key_c",            "C" },
    { "elk_key_d",            "D" },
    { "elk_key_e",            "E" },
    { "elk_key_f",            "F" },
    { "elk_key_g",            "G" },
    { "elk_key_h",            "H" },
    { "elk_key_i",            "I" },
    { "elk_key_j",            "J" },
    { "elk_key_k",            "K" },
    { "elk_key_l",            "L" },
    { "elk_key_m",            "M" },
    { "elk_key_n",            "N" },
    { "elk_key_o",            "O" },
    { "elk_key_p",            "P" },
    { "elk_key_q",            "Q" },
    { "elk_key_r",            "R" },
    { "elk_key_s",            "S" },
    { "elk_key_t",            "T" },
    { "elk_key_u",            "U" },
    { "elk_key_v",            "V" },
    { "elk_key_w",            "W" },
    { "elk_key_x",            "X" },
    { "elk_key_y",            "Y" },
    { "elk_key_z",            "Z" },
    { "elk_key_equals",       "=" },
    { "elk_key_comma",        "," },
    { "elk_key_fullstop",     "." },
    { "elk_key_slash",        "/" },
    { "elk_key_semicolon",    ";" },
    { "elk_key_colon",        ":" },
    { "elk_key_left",         "Left" },
    { "elk_key_right",        "Right" },
    { "elk_key_up",           "Up" },
    { "elk_key_down",         "Down" },
    { "elk_key_function",     "Function" },
    { "elk_key_copy",         "Copy" },
    { "elk_key_control",      "Control" },
    { "elk_key_shift",        "Shift" },
    { "elk_key_del",          "Delete" },
    { "elk_key_space",        "Spacebar" },
    { "elk_key_return",       "Return" },
    { "elk_key_escape",       "Escape" },
    { "elk_key_break",        "Break" },
    { "elk_special_key_menu", "<menu>" }
};


// see https://planet.racket-lang.org/package-source/kazzmir/allegro.plt/1/6/allegro-4.2.0/examples/exkeys.c
static const key_strings_t host_key_string_table[HOST_KEY_MAX] = 
{
    { "", "" },
    { "host_key_a", "A" },
    { "host_key_b", "B" },
    { "host_key_c", "C" },
    { "host_key_d", "D" },
    { "host_key_e", "E" },
    { "host_key_f", "F" },
    { "host_key_g", "G" },
    { "host_key_h", "H" },
    { "host_key_i", "I" },
    { "host_key_j", "J" },
    { "host_key_k", "K" },
    { "host_key_l", "L" },
    { "host_key_m", "M" },
    { "host_key_n", "N" },
    { "host_key_o", "O" },
    { "host_key_p", "P" },
    { "host_key_q", "Q" },
    { "host_key_r", "R" },
    { "host_key_s", "S" },
    { "host_key_t", "T" },
    { "host_key_u", "U" },
    { "host_key_v", "V" },
    { "host_key_w", "W" },
    { "host_key_x", "X" },
    { "host_key_y", "Y" },
    { "host_key_z", "Z" },

    { "host_key_0", "0" },
    { "host_key_1", "1" },
    { "host_key_2", "2" },
    { "host_key_3", "3" },
    { "host_key_4", "4" },
    { "host_key_5", "5" },
    { "host_key_6", "6" },
    { "host_key_7", "7" },
    { "host_key_8", "8" },
    { "host_key_9", "9" },

    { "host_key_pad_0",        "Keypad 0" },
    { "host_key_pad_1",        "Keypad 1" },
    { "host_key_pad_2",        "Keypad 2" },
    { "host_key_pad_3",        "Keypad 3" },
    { "host_key_pad_4",        "Keypad 4" },
    { "host_key_pad_5",        "Keypad 5" },
    { "host_key_pad_6",        "Keypad 6"  },
    { "host_key_pad_7",        "Keypad 7" },
    { "host_key_pad_8",        "Keypad 8" },
    { "host_key_pad_9",        "Keypad 9"  },

    { "host_key_f1",  "F1"   },
    { "host_key_f2",  "F2"   },
    { "host_key_f3",  "F3"   },
    { "host_key_f4",  "F4"   },
    { "host_key_f5",  "F5"   },
    { "host_key_f6",  "F6"   },
    { "host_key_f7",  "F7"   },
    { "host_key_f8",  "F8"   },
    { "host_key_f9",  "F9"   },
    { "host_key_f10", "F10"   },
    { "host_key_f11", "F11"   },
    { "host_key_f12", "F12"   },

    { "host_key_escape",      "Escape"        },
    { "host_key_tilde",       "` (tilde)"     },
    { "host_key_minus",       "-"             },
    { "host_key_equals",      "="             },
    { "host_key_backspace",   "Backspace"     },

    { "host_key_tab",         "Tab"           },
    { "host_key_openbrace",   "["             },
    { "host_key_closebrace",  "]"             },
    { "host_key_enter",       "Enter"         },
    { "host_key_semicolon",   ";"             },
    { "host_key_apostrophe",  "'"             },
    { "host_key_backslash",   "\\"            },
    { "host_key_backslash2",  "\\2"           },
    { "host_key_comma",       ","             },
    { "host_key_fullstop",    "."             },
    { "host_key_slash",       "/"             },
    { "host_key_space",       "Space"         },
    { "host_key_insert",      "Insert"        },
    { "host_key_delete",      "Delete"        },
    { "host_key_home",        "Home"          },
    { "host_key_end",         "End"           },
    { "host_key_pageup",      "Page Up"       },
    { "host_key_pagedown",    "Page Down"     },

    { "host_key_left",        "Arrow Left"    },
    { "host_key_right",       "Arrow Right"   },
    { "host_key_up",          "Arrow Up"      },
    { "host_key_down",        "Arror Down"    },

    { "host_key_pad_slash",    "Keypad /"  },
    { "host_key_pad_asterisk", "Keypad *"  },
    { "host_key_pad_minus",    "Keypad -"  },
    { "host_key_pad_plus",     "Keypad +"  },
    { "host_key_pad_delete",   "Keypad Del"   },
    { "host_key_pad_enter",    "Keypad Enter"  },
    { "host_key_print_screen", "Print Screen"  },
    { "host_key_pause",        "Pause"  },

    { "host_key_abnt_c1",      "AbntC1"  },
    { "host_key_yen",          "Yen"     },
    { "host_key_kana",         "Kana"    },
    { "host_key_convert",      "Convert"    },
    { "host_key_noconvert",    "Noconvert"    },

    { "host_key_at",           "AT"    },
    { "host_key_circumflex",   "Circumflex" },
    { "host_key_colon2",       "Colon2"    },
    { "host_key_kanji",        "Kanji"    },
    { "host_key_pad_equals",   "Keypad ="    },
    { "host_key_backquote",    "Backquote"    },
    { "host_key_semicolon2",   "Semicolon2"    },
    { "host_key_command",      "Command"    },

    { "host_key_lshift",      "Left Shift"    },
    { "host_key_rshift",      "Right Shift"   },
    { "host_key_lctrl",       "Left CTRL"     },
    { "host_key_rctrl",       "Left CTRL"     },
    { "host_key_alt",         "ALT"           },
    { "host_key_altgr",       "ALTGR"         },

    { "host_key_lwin",        "Left WIN"      },
    { "host_key_rwin",        "Right WIN"     },
    { "host_key_menu",        "MENU"          },
    { "host_key_scroll_lock", "Scroll Lock"   },
    { "host_key_num_lock",    "Num Lock"      },
    { "host_key_caps_lock",   "Caps Lock"     },
};


// see https://planet.racket-lang.org/package-source/kazzmir/allegro.plt/1/6/allegro-4.2.0/examples/exkeys.c
static const int old_config_keys_to_new_host_keys_table[128] =
{
    HOST_KEY_NONE, // key_define_000
    HOST_KEY_A,  // key_define_001
    HOST_KEY_B,  // key_define_002
    HOST_KEY_C,  // key_define_003
    HOST_KEY_D,  // key_define_004
    HOST_KEY_E,  // key_define_005
    HOST_KEY_F,  // key_define_006
    HOST_KEY_G,  // key_define_007
    HOST_KEY_H,  // key_define_008
    HOST_KEY_I,  // key_define_009
    HOST_KEY_J,  // key_define_010
    HOST_KEY_K,  // key_define_011
    HOST_KEY_L,  // key_define_012
    HOST_KEY_M,  // key_define_013
    HOST_KEY_N,  // key_define_014
    HOST_KEY_O,  // key_define_015
    HOST_KEY_P,  // key_define_016
    HOST_KEY_Q,  // key_define_017
    HOST_KEY_R,  // key_define_018
    HOST_KEY_S,  // key_define_019
    HOST_KEY_T,  // key_define_020
    HOST_KEY_U,  // key_define_021
    HOST_KEY_V,  // key_define_022
    HOST_KEY_W,  // key_define_023
    HOST_KEY_X,  // key_define_024
    HOST_KEY_Y,  // key_define_025
    HOST_KEY_Z,  // key_define_026


    HOST_KEY_0,		// key_define_027,
    HOST_KEY_1,		// key_define_028,
    HOST_KEY_2,		// key_define_029,
    HOST_KEY_3,		// key_define_030,
    HOST_KEY_4,		// key_define_031,
    HOST_KEY_5,		// key_define_032,
    HOST_KEY_6,		// key_define_033,
    HOST_KEY_7,		// key_define_034,
    HOST_KEY_8,		// key_define_035,
    HOST_KEY_9,		// key_define_036,

    HOST_KEY_PAD_0,		// key_define_037,
    HOST_KEY_PAD_1,		// key_define_038,
    HOST_KEY_PAD_2,		// key_define_039,
    HOST_KEY_PAD_3,		// key_define_040,
    HOST_KEY_PAD_4,		// key_define_041,
    HOST_KEY_PAD_5,		// key_define_042,
    HOST_KEY_PAD_6,		// key_define_043,
    HOST_KEY_PAD_7,		// key_define_044,
    HOST_KEY_PAD_8,		// key_define_045,
    HOST_KEY_PAD_9,		// key_define0_46,

    HOST_KEY_F1,	    	// key_define_047,
    HOST_KEY_F2,		    // key_define_048,
    HOST_KEY_F3,		    // key_define_049,
    HOST_KEY_F4,	    	// key_define_050,
    HOST_KEY_F5,	    	// key_define_051,
    HOST_KEY_F6,	    	// key_define_052,
    HOST_KEY_F7,	    	// key_define_053,
    HOST_KEY_F8,	    	// key_define_054,
    HOST_KEY_F9,	    	// key_define_055,
    HOST_KEY_F10,	        // key_define_056,
    HOST_KEY_F11,	        // key_define_057,
    HOST_KEY_F12,	        // key_define_058,

    HOST_KEY_ESCAPE,	    // key_define_059,
    HOST_KEY_TILDE,	        // key_define_060,
    HOST_KEY_MINUS,		    // key_define_061,
    HOST_KEY_EQUALS,	    // key_define_062,
    HOST_KEY_BACKSPACE,	    // key_define_063,
    HOST_KEY_TAB,		    // key_define_064,
    HOST_KEY_OPENBRACE,	    // key_define_065,
    HOST_KEY_CLOSEBRACE,	// key_define_066,
    HOST_KEY_ENTER,		    // key_define_067,
    HOST_KEY_SEMICOLON,	    // key_define_068,
    HOST_KEY_APOSTROPHE,    // key_define_069,
    HOST_KEY_BACKSLASH,	    // key_define_070,
    HOST_KEY_BACKSLASH2,	// key_define_071, /* DirectInput calls this DIK_OEM_102: "< > | on UK/Germany keyboards" */
    HOST_KEY_COMMA,		    // key_define_072,
    HOST_KEY_FULLSTOP,	    // key_define_073,
    HOST_KEY_SLASH,		    // key_define_074,
    HOST_KEY_SPACE,		    // key_define_075,

    HOST_KEY_INSERT,	    // key_define_076,
    HOST_KEY_DELETE,	    // key_define_077,
    HOST_KEY_HOME,		    // key_define_078,
    HOST_KEY_END,		    // key_define_079,
    HOST_KEY_PAGEUP,		// key_define_080,
    HOST_KEY_PAGEDOWN,	    // key_define_081,
    HOST_KEY_LEFT,		    // key_define_082,
    HOST_KEY_RIGHT,		    // key_define_083,
    HOST_KEY_UP,		    // key_define_084,
    HOST_KEY_DOWN,		    // key_define_085,

    HOST_KEY_PAD_SLASH,	    // key_define_086
    HOST_KEY_PAD_ASTERISK,	// key_define_087
    HOST_KEY_PAD_MINUS,	    // key_define_088
    HOST_KEY_PAD_PLUS,	    // key_define_089
    HOST_KEY_PAD_DELETE,	// key_define_090
    HOST_KEY_PAD_ENTER,	    // key_define_091

    HOST_KEY_PRINT_SCREEN,	// key_define_092
    HOST_KEY_PAUSE,		    // key_define_093

    HOST_KEY_ABNT_C1,	    // key_define_094
    HOST_KEY_YEN,		    // key_define_095
    HOST_KEY_KANA,		    // key_define_096
    HOST_KEY_CONVERT,	    // key_define_097
    HOST_KEY_NOCONVERT,	    // key_define_098
    HOST_KEY_AT,		    // key_define_099
    HOST_KEY_CIRCUMFLEX,	// key_define_100
    HOST_KEY_COLON2,	    // key_define_101
    HOST_KEY_KANJI,		    // key_define_102

    HOST_KEY_PAD_EQUALS,	// key_define_103	MacOS X
    HOST_KEY_BACKQUOTE,	    // key_define_104	MacOS X
    HOST_KEY_SEMICOLON2,	// key_define_105	MacOS X
    HOST_KEY_COMMAND,	    // key_define_106	MacOS X

    HOST_KEY_NONE,      // key_define_107
    HOST_KEY_NONE,      // key_define_108
    HOST_KEY_NONE,      // key_define_109
    HOST_KEY_NONE,      // key_define_110
    HOST_KEY_NONE,      // key_define_111
    HOST_KEY_NONE,      // key_define_112
    HOST_KEY_NONE,      // key_define_113
    HOST_KEY_NONE,      // key_define_114

    HOST_KEY_LSHIFT,        // key_define_115
    HOST_KEY_RSHIFT,        // key_define_116
    HOST_KEY_LCTRL,         // key_define_117
    HOST_KEY_RCTRL,         // key_define_118
    HOST_KEY_ALT,           // key_define_119
    HOST_KEY_ALTGR,         // key_define_120

    HOST_KEY_LWIN,          // key_define_121
    HOST_KEY_RWIN,          // key_define_122
    HOST_KEY_MENU,          // key_define_123
    HOST_KEY_SCROLLLOCK,    // key_define_124
    HOST_KEY_NUMLOCK,       // key_define_125
    HOST_KEY_CAPSLOCK,      // key_define_126

    HOST_KEY_MAX,           // key_define_127,

};

host_key_t keyutils_get_hostkey_from_legacy_keyid(uint8_t old_key_id)
{
    host_key_t host_key = HOST_KEY_NONE;

    if(old_key_id < HOST_KEY_MAX)
    {
        host_key = old_config_keys_to_new_host_keys_table[old_key_id];
    }
    log_debug("old_key_id = %d, returns host_key %d", old_key_id, host_key);
    return(host_key);
}

const char * keyutils_get_hostkey_longname(host_key_t host_key)
{
    const char * config_str = NULL;
    if(host_key < HOST_KEY_MAX)
    {
        config_str = host_key_string_table[host_key].key_longname;
    }
    return config_str;
}


const char * keyutils_get_hostkey_config_string(host_key_t host_key)
{
    const char * config_str = NULL;
    if(host_key < HOST_KEY_MAX)
    {
        config_str = host_key_string_table[host_key].config_key_string;
    }
    return config_str;
}

const char * keyutils_get_elkkey_longname(elk_key_id_t elk_key)
{
    const char * config_str = NULL;
    if(elk_key < ELK_KEY_MAX)
    {
        config_str = elk_keycode_config_string[elk_key].key_longname;
    }
    return config_str;
}

const char * keyutils_get_elkkey_config_string(elk_key_id_t elk_key)
{
    const char * config_str = NULL;
    if(elk_key < ELK_KEY_MAX)
    {
        config_str = elk_keycode_config_string[elk_key].config_key_string;
    }
    return config_str;
}

elk_key_id_t keyutils_config_string_to_elk_key_id(const char * host_config_str)
{
    elk_key_id_t elk_key = ELK_KEY_NONE;
    bool found = false;
    while(!found && ++elk_key < ELK_KEY_MAX)
    {
        if(strcmp(host_config_str, elk_keycode_config_string[elk_key].config_key_string) == 0)
        {
            found = true;
        }
    }

    if(elk_key == ELK_KEY_MAX)
    {
        elk_key = ELK_KEY_NONE;
    }

    return(elk_key);
}
