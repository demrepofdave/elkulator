
#include <string.h>
#include "host_abstraction_layer/keyboard.h"

static char * elk_keycode_config_string[ELK_KEY_MAX] =
{
    "",
    "elk_key_0",
    "elk_key_1",
    "elk_key_2",
    "elk_key_3",
    "elk_key_4",
    "elk_key_5",
    "elk_key_6",
    "elk_key_7",
    "elk_key_8",
    "elk_key_9",
    "elk_key_a",
    "elk_key_b",
    "elk_key_c",
    "elk_key_d",
    "elk_key_e",
    "elk_key_f",
    "elk_key_g",
    "elk_key_h",
    "elk_key_i",
    "elk_key_j",
    "elk_key_k",
    "elk_key_l",
    "elk_key_m",
    "elk_key_n",
    "elk_key_o",
    "elk_key_p",
    "elk_key_q",
    "elk_key_r",
    "elk_key_s",
    "elk_key_t",
    "elk_key_u",
    "elk_key_v",
    "elk_key_w",
    "elk_key_x",
    "elk_key_y",
    "elk_key_z",
    "elk_key_equals",
    "elk_key_comma",
    "elk_key_fullstop",
    "elk_key_slash",
    "elk_key_semicolon",
    "elk_key_colon",
    "elk_key_left",
    "elk_key_right",
    "elk_key_up",
    "elk_key_down",
    "elk_key_function",
    "elk_key_copy",
    "elk_key_control",
    "elk_key_shift",
    "elk_key_del",
    "elk_key_space",
    "elk_key_return",
    "elk_key_escape",
    "elk_key_break",
    "elk_special_key_menu",
};


typedef struct 
{
    const char * config_key_string;
    const char * key_longname;
} host_key_strings_t;


static const host_key_strings_t host_key_string_table[HOST_KEY_MAX] = 
{
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
    
    { "host_key_A", "A" },
    { "host_key_B", "B" },
    { "host_key_C", "C" },
    { "host_key_D", "D" },
    { "host_key_E", "E" },
    { "host_key_F", "F" },
    { "host_key_G", "G" },
    { "host_key_H", "H" },
    { "host_key_I", "I" },
    { "host_key_J", "J" },
    { "host_key_K", "K" },
    { "host_key_L", "L" },
    { "host_key_M", "M" },
    { "host_key_N", "N" },
    { "host_key_O", "O" },
    { "host_key_P", "P" },
    { "host_key_Q", "Q" },
    { "host_key_R", "R" },
    { "host_key_S", "S" },
    { "host_key_T", "T" },
    { "host_key_U", "U" },
    { "host_key_V", "V" },
    { "host_key_W", "W" },
    { "host_key_X", "X" },
    { "host_key_Y", "Y" },
    { "host_key_Z", "Z" },

    { "host_key_PAD_0",        "Keypad 0" },
    { "host_key_PAD_1",        "Keypad 1" },
    { "host_key_PAD_2",        "Keypad 2" },
    { "host_key_PAD_3",        "Keypad 3" },
    { "host_key_PAD_4",        "Keypad 4" },
    { "host_key_PAD_5",        "Keypad 5" },
    { "host_key_PAD_6",        "Keypad 6"  },
    { "host_key_PAD_7",        "Keypad 7" },
    { "host_key_PAD_8",        "Keypad 8" },
    { "host_key_PAD_9",        "Keypad 9"  },
    { "host_key_PAD_SLASH",    "Keypad /"  },
    { "host_key_PAD_ASTERISK", "Keypad *"  },
    { "host_key_PAD_MINUS",    "Keypad -"  },
    { "host_key_PAD_PLUS",     "Keypad +"  },
    { "host_key_PAD_DELETE",   "Keypad Del"   },
    { "host_key_PAD_ENTER",    "Keypad Enter"  },

    { "host_key_F1",  "F1"   },
    { "host_key_F2",  "F2"   },
    { "host_key_F3",  "F3"   },
    { "host_key_F4",  "F4"   },
    { "host_key_F5",  "F5"   },
    { "host_key_F6",  "F6"   },
    { "host_key_F7",  "F7"   },
    { "host_key_F8",  "F8"   },
    { "host_key_F9",  "F9"   },
    { "host_key_F10", "F10"   },
    { "host_key_F11", "F11"   },
    { "host_key_F12", "F12"   },

    { "host_key_LSHIFT",      "Left Shift"    },
    { "host_key_RSHIFT",      "Right Shift"   },
    { "host_key_LCTRL",       "Left CTRL"     },
    { "host_key_RCTRL",       "Left CTRL"     },
    { "host_key_ALT",         "ALT"           },
    { "host_key_ALTGR",       "ALTGR"         },
    { "host_key_LWIN",        "Left WIN"      },
    { "host_key_RWIN",        "Right WIN"     },
    { "host_key_MENU",        "MENU"          },
    { "host_key_SCROLL_LOCK", "Scroll Lock"   },
    { "host_key_NUM_LOCK",    "Num Lock"      },
    { "host_key_CAPS_LOCK",   "Caps Lock"     },
};



const char * keyboard_hostkey_to_config_str(host_key_t host_key)
{
    const char * config_str = NULL;
    if(host_key < HOST_KEY_MAX)
    {
        config_str = host_key_string_table[host_key].config_key_string;
    }
    return config_str;
}

char * keyboard_elkkey_to_config_str(elk_key_id_t elk_key)
{
    char * config_str = NULL;
    if(elk_key < ELK_KEY_MAX)
    {
        config_str = elk_keycode_config_string[elk_key];
    }
    return config_str;
}

elk_key_id_t keyboard_str_to_elk_key_id(const char * host_config_str)
{
    elk_key_id_t elk_key = ELK_KEY_NONE;
    bool found = false;
    while(!found && ++elk_key < ELK_KEY_MAX)
    {
        if(strcmp(host_config_str, elk_keycode_config_string[elk_key]) == 0)
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
