/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * keyboard.c
 * 
 * Keyboard handling functions for the elctron (outside of the
 * host abstraction layer keyboard).
 * 
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <string.h>
#include "config_vars.h"
#include "keyboard.h"
#include "logger.h"


/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

#define NO_ALTERNATE_KEY HOST_KEY_MAX

/******************************************************************************
* Typedefs
*******************************************************************************/

typedef struct {
    host_key_t host_keycode_main;
    host_key_t host_keycode_alternate;
} elk_key_defaults_t;

// Key defaults, if no alternative config is present in elk.cfg
 static const elk_key_defaults_t elk_keycode_defaults[ELK_KEY_MAX] = {
    { NO_ALTERNATE_KEY,   NO_ALTERNATE_KEY   }, // ELK_KEY_NONE,
    { HOST_KEY_0,         NO_ALTERNATE_KEY   }, // ELK_KEY_0,
    { HOST_KEY_1,         NO_ALTERNATE_KEY   }, // ELK_KEY_1,
    { HOST_KEY_2,         NO_ALTERNATE_KEY   }, // ELK_KEY_2,
    { HOST_KEY_3,         NO_ALTERNATE_KEY   }, // ELK_KEY_3,
    { HOST_KEY_4,         NO_ALTERNATE_KEY   }, // ELK_KEY_4,
    { HOST_KEY_5,         NO_ALTERNATE_KEY   }, // ELK_KEY_5,
    { HOST_KEY_6,         NO_ALTERNATE_KEY   }, // ELK_KEY_6,
    { HOST_KEY_7,         NO_ALTERNATE_KEY   }, // ELK_KEY_7,
    { HOST_KEY_8,         NO_ALTERNATE_KEY   }, // ELK_KEY_8,
    { HOST_KEY_9,         NO_ALTERNATE_KEY   }, // ELK_KEY_9,
    { HOST_KEY_A,         NO_ALTERNATE_KEY   }, // ELK_KEY_A,
    { HOST_KEY_B,         NO_ALTERNATE_KEY   }, // ELK_KEY_B,
    { HOST_KEY_C,         NO_ALTERNATE_KEY   }, // ELK_KEY_C,
    { HOST_KEY_D,         NO_ALTERNATE_KEY   }, // ELK_KEY_D,
    { HOST_KEY_E,         NO_ALTERNATE_KEY   }, // ELK_KEY_E,
    { HOST_KEY_F,         NO_ALTERNATE_KEY   }, // ELK_KEY_F,
    { HOST_KEY_G,         NO_ALTERNATE_KEY   }, // ELK_KEY_G,
    { HOST_KEY_H,         NO_ALTERNATE_KEY   }, // ELK_KEY_H,
    { HOST_KEY_I,         NO_ALTERNATE_KEY   }, // ELK_KEY_I,
    { HOST_KEY_J,         NO_ALTERNATE_KEY   }, // ELK_KEY_J,
    { HOST_KEY_K,         NO_ALTERNATE_KEY   }, // ELK_KEY_K,
    { HOST_KEY_L,         NO_ALTERNATE_KEY   }, // ELK_KEY_L,
    { HOST_KEY_M,         NO_ALTERNATE_KEY   }, // ELK_KEY_M,
    { HOST_KEY_N,         NO_ALTERNATE_KEY   }, // ELK_KEY_N,
    { HOST_KEY_O,         NO_ALTERNATE_KEY   }, // ELK_KEY_O,
    { HOST_KEY_P,         NO_ALTERNATE_KEY   }, // ELK_KEY_P,
    { HOST_KEY_Q,         NO_ALTERNATE_KEY   }, // ELK_KEY_Q,
    { HOST_KEY_R,         NO_ALTERNATE_KEY   }, // ELK_KEY_R,
    { HOST_KEY_S,         NO_ALTERNATE_KEY   }, // ELK_KEY_S,
    { HOST_KEY_T,         NO_ALTERNATE_KEY   }, // ELK_KEY_T,
    { HOST_KEY_U,         NO_ALTERNATE_KEY   }, // ELK_KEY_U,
    { HOST_KEY_V,         NO_ALTERNATE_KEY   }, // ELK_KEY_V,
    { HOST_KEY_W,         NO_ALTERNATE_KEY   }, // ELK_KEY_W,
    { HOST_KEY_X,         NO_ALTERNATE_KEY   }, // ELK_KEY_X,
    { HOST_KEY_Y,         NO_ALTERNATE_KEY   }, // ELK_KEY_Y,
    { HOST_KEY_Z,         NO_ALTERNATE_KEY   }, // ELK_KEY_Z,
    { HOST_KEY_MINUS,     HOST_KEY_EQUALS    }, // ELK_KEY_EQUALS,
    { HOST_KEY_COMMA,     NO_ALTERNATE_KEY   }, // ELK_KEY_COMMA,
    { HOST_KEY_FULLSTOP,  NO_ALTERNATE_KEY   }, // ELK_KEY_FULLSTOP,
    { HOST_KEY_SLASH,     NO_ALTERNATE_KEY   }, // ELK_KEY_SLASH,
    { HOST_KEY_SEMICOLON, NO_ALTERNATE_KEY   }, // ELK_KEY_SEMICOLON,
    { HOST_KEY_APOSTROPHE,NO_ALTERNATE_KEY   }, // ELK_KEY_COLON,
    { HOST_KEY_LEFT,      HOST_KEY_PAD_4     }, // ELK_KEY_LEFT,
    { HOST_KEY_RIGHT,     HOST_KEY_PAD_6     }, // ELK_KEY_RIGHT,
    { HOST_KEY_UP,        HOST_KEY_PAD_8     }, // ELK_KEY_UP,
    { HOST_KEY_DOWN,      HOST_KEY_PAD_2     }, // ELK_KEY_DOWN,
    { HOST_KEY_TAB,       NO_ALTERNATE_KEY   }, // ELK_KEY_FUNCTION,
    { HOST_KEY_END,       NO_ALTERNATE_KEY   }, // ELK_KEY_COPY,
    { HOST_KEY_CAPSLOCK,  NO_ALTERNATE_KEY   }, // ELK_KEY_CONTROL,
    { HOST_KEY_LSHIFT,    HOST_KEY_RSHIFT    }, // ELK_KEY_SHIFT,
    { HOST_KEY_BACKSPACE, HOST_KEY_DELETE    }, // ELK_KEY_DEL,
    { HOST_KEY_SPACE,     NO_ALTERNATE_KEY   }, // ELK_KEY_SPACE,
    { HOST_KEY_ENTER,     HOST_KEY_PAD_ENTER }, // ELK_KEY_RETURN,
    { HOST_KEY_ESCAPE,    NO_ALTERNATE_KEY   }, // ELK_KEY_ESCAPE,
    { HOST_KEY_F12,       NO_ALTERNATE_KEY   }, // ELK_KEY_BREAK,
    { HOST_KEY_F11,       HOST_KEY_MENU      }  // ELK_SPECIAL_KEY_MENU (only used for allegro 4).
};

typedef struct 
{
    const char * config_key_string;
    const char * key_longname;
} key_strings_t;

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

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
    { "host_key_down",        "Arrow Down"    },

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

// Records if native key is pressed or not (true = pressed, false = not pressed)
bool elk_key_state[ELK_KEY_MAX];

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

/* Key reading control. */
static bool special_key_pressed(elk_key_id_t elk_keycode)
{
    return elk_key_state[elk_keycode];
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void keyboard_makelayout()
{
    int c;

    // Config will aleady have any redefined keys in it prior to entering
    // this function.  We fill in the remaining defaults.
    log_debug("Make layout");

    // Now assign defaults.
    for(c = 0; c < ELK_KEY_MAX; c++)
    {
        elk_key_state[c] = false;
        
        // Map default
        if(elkConfig.keyboard.host_key_mapping[elk_keycode_defaults[c].host_keycode_main] == ELK_KEY_NONE)
        {
            elkConfig.keyboard.host_key_mapping[elk_keycode_defaults[c].host_keycode_main] = c;
        }

        // Map alternate if it exists
        if(elk_keycode_defaults[c].host_keycode_alternate != NO_ALTERNATE_KEY &&
           elkConfig.keyboard.host_key_mapping[elk_keycode_defaults[c].host_keycode_alternate] == ELK_KEY_NONE)
        {
            elkConfig.keyboard.host_key_mapping[elk_keycode_defaults[c].host_keycode_alternate] = c;
        }
    }
}

void keyboard_debug_dump()
{
    int c;
    for(c = 0; c< HOST_KEY_MAX; c++)
    {
        if(elkConfig.keyboard.host_key_mapping[c] != ELK_KEY_NONE)
        {
            log_debug("%s=%s", keyboard_get_hostkey_config_string(c), keyboard_get_elkkey_config_string(elkConfig.keyboard.host_key_mapping[c]));
        }
    }
}

bool keyboard_elk_key_state(elk_key_id_t elk_key_code)
{
    return(elk_key_state[elk_key_code]);
}

const char * keyboard_get_hostkey_longname(host_key_t host_key)
{
    const char * config_str = NULL;
    if(host_key < HOST_KEY_MAX)
    {
        config_str = host_key_string_table[host_key].key_longname;
    }
    return config_str;
}


const char * keyboard_get_hostkey_config_string(host_key_t host_key)
{
    const char * config_str = NULL;
    if(host_key < HOST_KEY_MAX)
    {
        config_str = host_key_string_table[host_key].config_key_string;
    }
    return config_str;
}

const char * keyboard_get_elkkey_longname(elk_key_id_t elk_key)
{
    const char * config_str = NULL;
    if(elk_key < ELK_KEY_MAX)
    {
        config_str = elk_keycode_config_string[elk_key].key_longname;
    }
    return config_str;
}

const char * keyboard_get_elkkey_config_string(elk_key_id_t elk_key)
{
    const char * config_str = NULL;
    if(elk_key < ELK_KEY_MAX)
    {
        config_str = elk_keycode_config_string[elk_key].config_key_string;
    }
    return config_str;
}

elk_key_id_t keyboard_config_string_to_elk_key_id(const char * host_config_str)
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

elk_key_id_t keyboard_get_default_elk_key_from_host_key(host_key_t host_key)
{
    elk_key_id_t elk_key = ELK_KEY_NONE;
    int index = 0;
    for(index = 0; index < ELK_KEY_MAX; index++)
    {
        if(elk_keycode_defaults[index].host_keycode_main == host_key || 
           elk_keycode_defaults[index].host_keycode_alternate == host_key)
        {
            elk_key = index;
        }
    }
    return elk_key;
}

bool break_pressed()
{
    return special_key_pressed(ELK_KEY_BREAK);
}

bool menu_pressed()
{
    return special_key_pressed(ELK_SPECIAL_KEY_MENU);
}

void keyboard_keydown(host_key_t hostkey)
{
    elk_key_id_t elkkey = elkConfig.keyboard.host_key_mapping[hostkey];

    if(elkkey != ELK_KEY_MAX)
    {
        elk_key_state[elkkey] = true;
    }
}

void keyboard_keyup(host_key_t hostkey)
{
    elk_key_id_t elkkey = elkConfig.keyboard.host_key_mapping[hostkey];

    if(elkkey != ELK_KEY_MAX)
    {
        elk_key_state[elkkey] = false;
    }
}

