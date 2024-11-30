
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
    { "host_key_pad_slash",    "Keypad /"  },
    { "host_key_pad_asterisk", "Keypad *"  },
    { "host_key_pad_minus",    "Keypad -"  },
    { "host_key_pad_plus",     "Keypad +"  },
    { "host_key_pad_delete",   "Keypad Del"   },
    { "host_key_pad_enter",    "Keypad Enter"  },

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
    { "host_key_enter",       "Enter"         },
    { "host_key_lshift",      "Left Shift"    },
    { "host_key_rshift",      "Right Shift"   },
    { "host_key_lctrl",       "Left CTRL"     },
    { "host_key_rctrl",       "Left CTRL"     },
    { "host_key_alt",         "ALT"           },
    { "host_key_space",       "Space"         },
    { "host_key_altgr",       "ALTGR"         },
    { "host_key_lwin",        "Left WIN"      },
    { "host_key_rwin",        "Right WIN"     },
    { "host_key_menu",        "MENU"          },
    { "host_key_scroll_lock", "Scroll Lock"   },
    { "host_key_num_lock",    "Num Lock"      },
    { "host_key_caps_lock",   "Caps Lock"     },
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
