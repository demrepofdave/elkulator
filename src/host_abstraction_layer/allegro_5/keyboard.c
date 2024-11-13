#include <allegro5/allegro.h>
#include "host_abstraction_layer/keyboard.h"
#include "keyboard_internal.h"
#include "elk.h"
#include "host_abstraction_layer/event_handler.h"
#include "logger.h"

#define NO_ALTERNATE_KEY 0
typedef struct {
    uint8_t allegro_keycode_main;
    uint8_t allegro_keycode_alternate;
} elk_key_defaults_t;

// Key defaults, if no alternative config is present in elk.cfg
static const elk_key_defaults_t elk_keycode_defaults[ELK_KEY_MAX] = {
    { NO_ALTERNATE_KEY,      NO_ALTERNATE_KEY      }, // ELK_KEY_NONE,
    { ALLEGRO_KEY_0,         NO_ALTERNATE_KEY      }, // ELK_KEY_0,
    { ALLEGRO_KEY_1,         NO_ALTERNATE_KEY      }, // ELK_KEY_1,
    { ALLEGRO_KEY_2,         NO_ALTERNATE_KEY      }, // ELK_KEY_2,
    { ALLEGRO_KEY_3,         NO_ALTERNATE_KEY      }, // ELK_KEY_3,
    { ALLEGRO_KEY_4,         NO_ALTERNATE_KEY      }, // ELK_KEY_4,
    { ALLEGRO_KEY_5,         NO_ALTERNATE_KEY      }, // ELK_KEY_5,
    { ALLEGRO_KEY_6,         NO_ALTERNATE_KEY      }, // ELK_KEY_6,
    { ALLEGRO_KEY_7,         NO_ALTERNATE_KEY      }, // ELK_KEY_7,
    { ALLEGRO_KEY_8,         NO_ALTERNATE_KEY      }, // ELK_KEY_8,
    { ALLEGRO_KEY_9,         NO_ALTERNATE_KEY      }, // ELK_KEY_9,
    { ALLEGRO_KEY_A,         NO_ALTERNATE_KEY      }, // ELK_KEY_A,
    { ALLEGRO_KEY_B,         NO_ALTERNATE_KEY      }, // ELK_KEY_B,
    { ALLEGRO_KEY_C,         NO_ALTERNATE_KEY      }, // ELK_KEY_C,
    { ALLEGRO_KEY_D,         NO_ALTERNATE_KEY      }, // ELK_KEY_D,
    { ALLEGRO_KEY_E,         NO_ALTERNATE_KEY      }, // ELK_KEY_E,
    { ALLEGRO_KEY_F,         NO_ALTERNATE_KEY      }, // ELK_KEY_F,
    { ALLEGRO_KEY_G,         NO_ALTERNATE_KEY      }, // ELK_KEY_G,
    { ALLEGRO_KEY_H,         NO_ALTERNATE_KEY      }, // ELK_KEY_H,
    { ALLEGRO_KEY_I,         NO_ALTERNATE_KEY      }, // ELK_KEY_I,
    { ALLEGRO_KEY_J,         NO_ALTERNATE_KEY      }, // ELK_KEY_J,
    { ALLEGRO_KEY_K,         NO_ALTERNATE_KEY      }, // ELK_KEY_K,
    { ALLEGRO_KEY_L,         NO_ALTERNATE_KEY      }, // ELK_KEY_L,
    { ALLEGRO_KEY_M,         NO_ALTERNATE_KEY      }, // ELK_KEY_M,
    { ALLEGRO_KEY_N,         NO_ALTERNATE_KEY      }, // ELK_KEY_N,
    { ALLEGRO_KEY_O,         NO_ALTERNATE_KEY      }, // ELK_KEY_O,
    { ALLEGRO_KEY_P,         NO_ALTERNATE_KEY      }, // ELK_KEY_P,
    { ALLEGRO_KEY_Q,         NO_ALTERNATE_KEY      }, // ELK_KEY_Q,
    { ALLEGRO_KEY_R,         NO_ALTERNATE_KEY      }, // ELK_KEY_R,
    { ALLEGRO_KEY_S,         NO_ALTERNATE_KEY      }, // ELK_KEY_S,
    { ALLEGRO_KEY_T,         NO_ALTERNATE_KEY      }, // ELK_KEY_T,
    { ALLEGRO_KEY_U,         NO_ALTERNATE_KEY      }, // ELK_KEY_U,
    { ALLEGRO_KEY_V,         NO_ALTERNATE_KEY      }, // ELK_KEY_V,
    { ALLEGRO_KEY_W,         NO_ALTERNATE_KEY      }, // ELK_KEY_W,
    { ALLEGRO_KEY_X,         NO_ALTERNATE_KEY      }, // ELK_KEY_X,
    { ALLEGRO_KEY_Y,         NO_ALTERNATE_KEY      }, // ELK_KEY_Y,
    { ALLEGRO_KEY_Z,         NO_ALTERNATE_KEY      }, // ELK_KEY_Z,
    { ALLEGRO_KEY_MINUS,     NO_ALTERNATE_KEY      }, // ELK_KEY_EQUALS,
    { ALLEGRO_KEY_COMMA,     NO_ALTERNATE_KEY      }, // ELK_KEY_COMMA,
    { ALLEGRO_KEY_FULLSTOP,  NO_ALTERNATE_KEY      }, // ELK_KEY_FULLSTOP,
    { ALLEGRO_KEY_SLASH,     NO_ALTERNATE_KEY      }, // ELK_KEY_FORWARD_SLASH,
    { ALLEGRO_KEY_SEMICOLON, NO_ALTERNATE_KEY      }, // ELK_KEY_SEMICOLON,
    { ALLEGRO_KEY_COLON2,    NO_ALTERNATE_KEY      }, // ELK_KEY_COLON,
    { ALLEGRO_KEY_LEFT,      ALLEGRO_KEY_PAD_4     }, // ELK_KEY_LEFT,
    { ALLEGRO_KEY_RIGHT,     ALLEGRO_KEY_PAD_6     }, // ELK_KEY_RIGHT,
    { ALLEGRO_KEY_UP,        ALLEGRO_KEY_PAD_8     }, // ELK_KEY_UP,
    { ALLEGRO_KEY_DOWN,      ALLEGRO_KEY_PAD_2     }, // ELK_KEY_DOWN,
    { ALLEGRO_KEY_TAB,       NO_ALTERNATE_KEY      }, // ELK_KEY_FUNCTION,
    { ALLEGRO_KEY_END,       NO_ALTERNATE_KEY      }, // ELK_KEY_COPY,
    { ALLEGRO_KEY_CAPSLOCK,  NO_ALTERNATE_KEY      }, // ELK_KEY_CONTROL,
    { ALLEGRO_KEY_LSHIFT,    ALLEGRO_KEY_RSHIFT    }, // ELK_KEY_SHIFT,
    { ALLEGRO_KEY_BACKSPACE, ALLEGRO_KEY_DELETE    }, // ELK_KEY_DEL,
    { ALLEGRO_KEY_SPACE,     NO_ALTERNATE_KEY      }, // ELK_KEY_SPACE,
    { ALLEGRO_KEY_ENTER,     ALLEGRO_KEY_PAD_ENTER }, // ELK_KEY_RETURN,
    { ALLEGRO_KEY_ESCAPE,    NO_ALTERNATE_KEY      }, // ELK_KEY_ESCAPE,
    { ALLEGRO_KEY_F12,       NO_ALTERNATE_KEY      }, // ELK_KEY_BREAK,
};

// Represents the internal mapping of the keyboard to memory locations
// 0x9FFF to 0xBFFE.
//
// See page 218 of the Advanced User Guide for the Acorn Electron
// for full details.
//
// Note the mappings are repeated twice as some Allegro distinguishes
// between keys more than the electron.  For example Left Shift and Right shift
// are mapped to the Electron shift key.
//
int keys[14][4]=
{
    // TODO: Check below the advanced user manual
    { ELK_KEY_RIGHT,  ELK_KEY_COPY, 0,              ELK_KEY_SPACE    },
    { ELK_KEY_LEFT,   ELK_KEY_DOWN, ELK_KEY_RETURN, ELK_KEY_DEL      },
    { ELK_KEY_EQUALS, ELK_KEY_UP,   ELK_KEY_SEMICOLON,  0                    },
    { ELK_KEY_0,      ELK_KEY_P,    ELK_KEY_COLON,  ELK_KEY_FORWARD_SLASH    },  // TODO: Key COLON2 may not be correct.
    { ELK_KEY_9,      ELK_KEY_O,    ELK_KEY_L,      ELK_KEY_FULLSTOP },
    { ELK_KEY_8,      ELK_KEY_I,    ELK_KEY_K,      ELK_KEY_COMMA    },
    { ELK_KEY_7,      ELK_KEY_U,    ELK_KEY_J,      ELK_KEY_M        },
    { ELK_KEY_6,      ELK_KEY_Y,    ELK_KEY_H,      ELK_KEY_N        },
    { ELK_KEY_5,      ELK_KEY_T,    ELK_KEY_G,      ELK_KEY_B        },
    { ELK_KEY_4,      ELK_KEY_R,    ELK_KEY_F,      ELK_KEY_V        },
    { ELK_KEY_3,      ELK_KEY_E,    ELK_KEY_D,      ELK_KEY_C        },
    { ELK_KEY_2,      ELK_KEY_W,    ELK_KEY_S,      ELK_KEY_X        },
    { ELK_KEY_1,      ELK_KEY_Q,    ELK_KEY_A,      ELK_KEY_Z        },
    { ELK_KEY_ESCAPE, ELK_KEY_FUNCTION,  ELK_KEY_CONTROL,  ELK_KEY_SHIFT   }
};

// Records if native key is pressed or not (true = pressed, false = not pressed)
bool key[ELK_KEY_MAX];

// keylookup allows the user to redefine a native key with another native key(?)
uint8_t keylookup[ALLEGRO_KEY_MAX];

bool keydefining = false;

// Allegro allows 128 keys to be used.
//
// This array contains a mapping for each and every one of them.
//
// Binary format for each entry is: edddcccc
//    e = 1 - The allegro key is mapped to an electron key.
//        0 - The allegro key is not mapped to an electron key (not in use)
//    d = bit number for mapping the key to the electron key colum bitmap.
//          - 000 = Mapped to bit 0
//          - 001 = Mapped to bit 1
//          - 010 = Mapped to bit 2
//          - 011 = Mapped to bit 3
//    c = The address bitmap mask for the key (tells us which Column address this key belongs to)
//          - 0000 = Address bitmap shifted by 0 = 00000001 = When applied if matches to address for column 0 = BFFE
//          - 0001 = Address bitmap shifted by 1 = 00000010 = When applied if matches to address for column 0 = BFFD
//          - 0010 = Address bitmap shifted by 2 = 00000100 = When applied if matches to address for column 0 = BFFB
//          - 0011 = Address bitmap shifted by 3 = 00001000 = When applied if matches to address for column 0 = BFF7
//          - 0100 = Address bitmap shifted by 4 = 00010000 = When applied if matches to address for column 0 = BFEF
//          ...
//
int keyl[ELK_KEY_MAX];

void keyboard_makelayout()
{
        int c,d,e;
        memset(keyl,0,sizeof(keyl));

        /* Establish a mapping from emulated key presses to keyboard matrix values. */

        for (c=0;c<14;c++)
        {
                for (d=0;d<4;d++)
                {
                    // This creates a bitmap of 1dddcccc.
                    // Top bit set means this entry is defined.
                    // c = the column number which is used to create the address mask
                    // d is bit number, used to correctly set the bit in the electrons memory address (if key is pressed)
                    // For examples see above.
                    keyl[keys[c][d]]=c|(d<<4)|0x80;
                }
        }

        // Reset keyboard
        for(c = 0; c< ALLEGRO_KEY_MAX; c++)
        {
            keylookup[c] = ELK_KEY_MAX; // This signifies no ELK key assignemnt
        }

        // Now assign defaults.
        for(c = 0; c < ELK_KEY_MAX; c++)
        {
            key[c] = false;
            keylookup[elk_keycode_defaults[c].allegro_keycode_main] = c; // Assign PC key to elk key.
            if(elk_keycode_defaults[c].allegro_keycode_alternate)
            {
                keylookup[elk_keycode_defaults[c].allegro_keycode_alternate] = c; // Assign alternate PC key to elk key (if defined).
            }
        }
        keyboard_debug_dump();

        /* Record the Break keys separately, along with configurable menu keys. */

        //update_break_keys();
        //update_menu_keys();
}

void keyboard_debug_dump()
{
    int c;
    for(c = 0; c< ALLEGRO_KEY_MAX; c++)
    {
        if(keylookup[c] < ELK_KEY_MAX)
        {
            log_debug("keylookup[%d] = %s(%d)", c, al_keycode_to_name(keylookup[c]), keylookup[c]);
        }
    }
}

uint8_t keyboard_read(uint16_t addr)
{
        int d;
        uint8_t temp=0;
        for (d=0;d<ELK_KEY_MAX;d++)
        {
                if (key[d]) // If host key is pressed.
                {
                    log_debug("Elk key %d pressed - lookup %02x", d, keylookup[d]);
                    // If key is mapped to an electron key, then set bit to 1 for
                    // the electron key (if key is mapped into elk memory at addr).
                    if(keyl[d]&0x80 && !(addr&(1<<(keyl[d]&15))))
                    {
                        temp |= 1<<((keyl[d]&0x30)>>4); // Set the bitmap for key to 1 in memory
                    }
                }
        }

        return temp;
}


void key_down(uint8_t allegro_keycode)
{
    log_debug("keydown %d", allegro_keycode);
    // TODO: Allegro to elk key conversion.
    if(keylookup[allegro_keycode] != ELK_KEY_MAX)
    {
        key[keylookup[allegro_keycode]] = true;
    }
}

void key_down_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
    log_debug("keyboard: key down event, keycode=%d:%s, modifiers=%04X", keycode, al_keycode_to_name(keycode), event->keyboard.modifiers);
//    if (keycode == ALLEGRO_KEY_ALT || keycode == ALLEGRO_KEY_ALTGR)
//        hostalt = true;
//    else if (keycode == ALLEGRO_KEY_CAPSLOCK)
//            key_down(keylookup[keycode]);
//    else {
//        bool shiftctrl = false;
//        if (keycode == ALLEGRO_KEY_LSHIFT || keycode == ALLEGRO_KEY_RSHIFT) {
//            hostshift = true;
//            shiftctrl = true;
//        }
//        else if (keycode == ALLEGRO_KEY_LCTRL || keycode == ALLEGRO_KEY_RCTRL) {
//            hostctrl = true;
//            shiftctrl = true;
//        }
//        if (shiftctrl) {
//            if (keylogical)
//                set_logical_shift_ctrl_if_idle();
//            else
                key_down(keycode);
//        }
//    }
}

void key_up(uint8_t allegro_keycode)
{
    log_debug("keyup %d", allegro_keycode);
    // TODO: Allegro to elk key conversion.
    if(keylookup[allegro_keycode] != ELK_KEY_MAX)
    {
        key[keylookup[allegro_keycode]] = false;
    }
}

void key_up_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
    log_debug("keyboard: key up event, keycode=%d:%s, modifiers=%04X", keycode, al_keycode_to_name(keycode), event->keyboard.modifiers);
    if (keycode < ALLEGRO_KEY_MAX)
    { 
        key_up(keycode);
//        if (keycode == ALLEGRO_KEY_ALT || keycode == ALLEGRO_KEY_ALTGR)
//            hostalt = false;
//        else if (keycode == ALLEGRO_KEY_CAPSLOCK)
//            key_up(keylookup[keycode]);
//        else {
//            int unichar = last_unichar[keycode];
//            bool shiftctrl = false;
//            if (keycode == ALLEGRO_KEY_LSHIFT || keycode == ALLEGRO_KEY_RSHIFT) {
//                hostshift = false;
//                shiftctrl = true;
//            }
//            else if (keycode == ALLEGRO_KEY_LCTRL || keycode == ALLEGRO_KEY_RCTRL) {
//                hostctrl = false;
//                shiftctrl = true;
//            }
//           else if (keycode == ALLEGRO_KEY_A && keyas && !keylogical)
//                keycode = ALLEGRO_KEY_CAPSLOCK;
//            else if (keycode == ALLEGRO_KEY_S && keyas && !keylogical)
//                keycode = ALLEGRO_KEY_LCTRL;
//            if (shiftctrl && keylogical)
//                set_logical_shift_ctrl_if_idle();
//            keycode = map_keypad_intern(keycode, unichar);
//            if (keylogical)
//                set_key_logical(keycode, unichar, false);
//            else
//                key_up(keylookup[keycode]);
//        }
    }
}

void key_char_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
    int unichar = event->keyboard.unichar;
    log_debug("keyboard: key char event, keycode=%d:%s, unichar=%d", keycode, al_keycode_to_name(keycode), unichar);
//    if ((!event->keyboard.repeat || unichar != last_unichar[keycode]) && keycode < ALLEGRO_KEY_MAX) {
//        last_unichar[keycode] = unichar;
//        keycode = map_keypad_intern(keycode, unichar);
//        if (keycode == ALLEGRO_KEY_A && keyas && !keylogical)
//            keycode = ALLEGRO_KEY_CAPSLOCK;
//        else if (keycode == ALLEGRO_KEY_S && keyas && !keylogical)
//            keycode = ALLEGRO_KEY_LCTRL;
//        if (keylogical)
//            set_key_logical(keycode, unichar, true);
//        else
//            key_down(keylookup[keycode]);
//    }
}


// Main event handling Code
elk_event_t keyboard_handle_event(ALLEGRO_EVENT *event)
{
    elk_event_t elkEvent = 0;
    if (!keydefining)
    {
        switch(event->type) 
        {
            // Keyboard handling.
            case ALLEGRO_EVENT_KEY_DOWN:
                key_down_event(event);
                break;

            case ALLEGRO_EVENT_KEY_CHAR:
                key_char_event(event);
                break;

            case ALLEGRO_EVENT_KEY_UP:
                key_up_event(event);
                break;
        }
    }
    return elkEvent;
}
