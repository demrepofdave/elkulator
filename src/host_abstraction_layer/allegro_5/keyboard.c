#include <allegro5/allegro.h>
#include "host_abstraction_layer/keyboard.h"
#include "keyboard_internal.h"
#include "elk.h"
#include "host_abstraction_layer/event_handler.h"
#include "logger.h"

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
int keys[2][14][4]=
{
        {
                { ALLEGRO_KEY_RIGHT,  ALLEGRO_KEY_END,  0,                  ALLEGRO_KEY_SPACE    },
                { ALLEGRO_KEY_LEFT,   ALLEGRO_KEY_DOWN, ALLEGRO_KEY_ENTER,  ALLEGRO_KEY_DELETE   },
                { ALLEGRO_KEY_MINUS,  ALLEGRO_KEY_UP,   ALLEGRO_KEY_QUOTE,  0                    },
                { ALLEGRO_KEY_0,      ALLEGRO_KEY_P,    ALLEGRO_KEY_COLON2, ALLEGRO_KEY_SLASH    },  // TODO: Key COLON2 may not be correct.
                { ALLEGRO_KEY_9,      ALLEGRO_KEY_O,    ALLEGRO_KEY_L,      ALLEGRO_KEY_FULLSTOP },
                { ALLEGRO_KEY_8,      ALLEGRO_KEY_I,    ALLEGRO_KEY_K,      ALLEGRO_KEY_COMMA    },
                { ALLEGRO_KEY_7,      ALLEGRO_KEY_U,    ALLEGRO_KEY_J,      ALLEGRO_KEY_M        },
                { ALLEGRO_KEY_6,      ALLEGRO_KEY_Y,    ALLEGRO_KEY_H,      ALLEGRO_KEY_N        },
                { ALLEGRO_KEY_5,      ALLEGRO_KEY_T,    ALLEGRO_KEY_G,      ALLEGRO_KEY_B        },
                { ALLEGRO_KEY_4,      ALLEGRO_KEY_R,    ALLEGRO_KEY_F,      ALLEGRO_KEY_V        },
                { ALLEGRO_KEY_3,      ALLEGRO_KEY_E,    ALLEGRO_KEY_D,      ALLEGRO_KEY_C        },
                { ALLEGRO_KEY_2,      ALLEGRO_KEY_W,    ALLEGRO_KEY_S,      ALLEGRO_KEY_X        },
                { ALLEGRO_KEY_1,      ALLEGRO_KEY_Q,    ALLEGRO_KEY_A,      ALLEGRO_KEY_Z        },
                { ALLEGRO_KEY_ESCAPE, ALLEGRO_KEY_ALT,  ALLEGRO_KEY_LCTRL,  ALLEGRO_KEY_LSHIFT   }
        },
        {
                { ALLEGRO_KEY_RIGHT,  ALLEGRO_KEY_END,  0,                     ALLEGRO_KEY_SPACE     },
                { ALLEGRO_KEY_LEFT,   ALLEGRO_KEY_DOWN, ALLEGRO_KEY_ENTER,     ALLEGRO_KEY_BACKSPACE },
                { ALLEGRO_KEY_MINUS,  ALLEGRO_KEY_UP,   ALLEGRO_KEY_QUOTE,     0                     },
                { ALLEGRO_KEY_0,      ALLEGRO_KEY_P,    ALLEGRO_KEY_SEMICOLON, ALLEGRO_KEY_SLASH     },
                { ALLEGRO_KEY_9,      ALLEGRO_KEY_O,    ALLEGRO_KEY_L,         ALLEGRO_KEY_FULLSTOP  },
                { ALLEGRO_KEY_8,      ALLEGRO_KEY_I,    ALLEGRO_KEY_K,         ALLEGRO_KEY_COMMA     },
                { ALLEGRO_KEY_7,      ALLEGRO_KEY_U,    ALLEGRO_KEY_J,         ALLEGRO_KEY_M         },
                { ALLEGRO_KEY_6,      ALLEGRO_KEY_Y,    ALLEGRO_KEY_H,         ALLEGRO_KEY_N         },
                { ALLEGRO_KEY_5,      ALLEGRO_KEY_T,    ALLEGRO_KEY_G,         ALLEGRO_KEY_B         },
                { ALLEGRO_KEY_4,      ALLEGRO_KEY_R,    ALLEGRO_KEY_F,         ALLEGRO_KEY_V         },
                { ALLEGRO_KEY_3,      ALLEGRO_KEY_E,    ALLEGRO_KEY_D,         ALLEGRO_KEY_C         },
                { ALLEGRO_KEY_2,      ALLEGRO_KEY_W,    ALLEGRO_KEY_S,         ALLEGRO_KEY_X         },
                { ALLEGRO_KEY_1,      ALLEGRO_KEY_Q,    ALLEGRO_KEY_A,         ALLEGRO_KEY_Z         },
                { ALLEGRO_KEY_ESCAPE, ALLEGRO_KEY_TAB,  ALLEGRO_KEY_RCTRL,     ALLEGRO_KEY_RSHIFT    }
        }
};

bool key[ALLEGRO_KEY_MAX];

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
int keyl[ALLEGRO_KEY_MAX];

void keyboard_makelayout()
{
        int c,d,e;
        memset(keyl,0,sizeof(keyl));
        keydefining = false;

        /* Establish a mapping from emulated key presses to keyboard matrix values. */

        for (c=0;c<14;c++)
        {
                for (d=0;d<4;d++)
                {
                        for (e=0;e<2;e++)
                        {
                                // This creates a bitmap of 1dddcccc.
                                // Top bit set means this entry is defined.
                                // c = the column number wshich is used to create the address mask
                                // d is bit number, used to correctly set the bit in the electrons memory address (if key is pressed)
                                // For examples see above.
                                keyl[keys[e][c][d]]=c|(d<<4)|0x80;
                        }
                }
        }

        // Reset keyboard
        for(c = 0; c< ALLEGRO_KEY_MAX; c++)
        {
                key[c] = false;
                keylookup[c] = c;
        }

        /* Record the Break keys separately, along with configurable menu keys. */

        //update_break_keys();
        //update_menu_keys();
}

uint8_t keyboard_read(uint16_t addr)
{
        int d;
        uint8_t temp=0;
        for (d=0;d<ALLEGRO_KEY_MAX;d++)
        {
                if (key[d]) // If host key is pressed.
                {
                    if(keyl[keylookup[d]]&0x80 && !(addr&(1<<(keyl[keylookup[d]]&15))))
                    {
                        temp|=1<<((keyl[keylookup[d]]&0x30)>>4); // Set the bitmap for key to 1 in memory
                    }
                }
        }

        return temp;
}


void key_down(uint8_t allegro_keycode)
{
    log_debug("keydown %d", allegro_keycode);
    key[allegro_keycode] = true;
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
    key[allegro_keycode] = false;
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
