#include "host_abstraction_layer/keyboard.h"
#include "elk.h"
#include "logger.h"
#include <allegro.h>
#include <stdbool.h>


#define NO_ALTERNATE_KEY 0
typedef struct {
    uint8_t allegro_keycode_main;
    uint8_t allegro_keycode_alternate;
} elk_key_defaults_t;

// Key defaults, if no alternative config is present in elk.cfg
static const elk_key_defaults_t elk_keycode_defaults[ELK_KEY_MAX] = {
    { NO_ALTERNATE_KEY,      NO_ALTERNATE_KEY      }, // ELK_KEY_NONE,
    { KEY_0,         NO_ALTERNATE_KEY      }, // ELK_KEY_0,
    { KEY_1,         NO_ALTERNATE_KEY      }, // ELK_KEY_1,
    { KEY_2,         NO_ALTERNATE_KEY      }, // ELK_KEY_2,
    { KEY_3,         NO_ALTERNATE_KEY      }, // ELK_KEY_3,
    { KEY_4,         NO_ALTERNATE_KEY      }, // ELK_KEY_4,
    { KEY_5,         NO_ALTERNATE_KEY      }, // ELK_KEY_5,
    { KEY_6,         NO_ALTERNATE_KEY      }, // ELK_KEY_6,
    { KEY_7,         NO_ALTERNATE_KEY      }, // ELK_KEY_7,
    { KEY_8,         NO_ALTERNATE_KEY      }, // ELK_KEY_8,
    { KEY_9,         NO_ALTERNATE_KEY      }, // ELK_KEY_9,
    { KEY_A,         NO_ALTERNATE_KEY      }, // ELK_KEY_A,
    { KEY_B,         NO_ALTERNATE_KEY      }, // ELK_KEY_B,
    { KEY_C,         NO_ALTERNATE_KEY      }, // ELK_KEY_C,
    { KEY_D,         NO_ALTERNATE_KEY      }, // ELK_KEY_D,
    { KEY_E,         NO_ALTERNATE_KEY      }, // ELK_KEY_E,
    { KEY_F,         NO_ALTERNATE_KEY      }, // ELK_KEY_F,
    { KEY_G,         NO_ALTERNATE_KEY      }, // ELK_KEY_G,
    { KEY_H,         NO_ALTERNATE_KEY      }, // ELK_KEY_H,
    { KEY_I,         NO_ALTERNATE_KEY      }, // ELK_KEY_I,
    { KEY_J,         NO_ALTERNATE_KEY      }, // ELK_KEY_J,
    { KEY_K,         NO_ALTERNATE_KEY      }, // ELK_KEY_K,
    { KEY_L,         NO_ALTERNATE_KEY      }, // ELK_KEY_L,
    { KEY_M,         NO_ALTERNATE_KEY      }, // ELK_KEY_M,
    { KEY_N,         NO_ALTERNATE_KEY      }, // ELK_KEY_N,
    { KEY_O,         NO_ALTERNATE_KEY      }, // ELK_KEY_O,
    { KEY_P,         NO_ALTERNATE_KEY      }, // ELK_KEY_P,
    { KEY_Q,         NO_ALTERNATE_KEY      }, // ELK_KEY_Q,
    { KEY_R,         NO_ALTERNATE_KEY      }, // ELK_KEY_R,
    { KEY_S,         NO_ALTERNATE_KEY      }, // ELK_KEY_S,
    { KEY_T,         NO_ALTERNATE_KEY      }, // ELK_KEY_T,
    { KEY_U,         NO_ALTERNATE_KEY      }, // ELK_KEY_U,
    { KEY_V,         NO_ALTERNATE_KEY      }, // ELK_KEY_V,
    { KEY_W,         NO_ALTERNATE_KEY      }, // ELK_KEY_W,
    { KEY_X,         NO_ALTERNATE_KEY      }, // ELK_KEY_X,
    { KEY_Y,         NO_ALTERNATE_KEY      }, // ELK_KEY_Y,
    { KEY_Z,         NO_ALTERNATE_KEY      }, // ELK_KEY_Z,
    { KEY_MINUS,     NO_ALTERNATE_KEY      }, // ELK_KEY_EQUALS,
    { KEY_COMMA,     NO_ALTERNATE_KEY      }, // ELK_KEY_COMMA,
    { KEY_STOP,  NO_ALTERNATE_KEY      }, // ELK_KEY_FULLSTOP,
    { KEY_SLASH,     NO_ALTERNATE_KEY      }, // ELK_KEY_FORWARD_SLASH,
    { KEY_SEMICOLON, NO_ALTERNATE_KEY      }, // ELK_KEY_SEMICOLON,
    { KEY_COLON2,    NO_ALTERNATE_KEY      }, // ELK_KEY_COLON,
    { KEY_LEFT,      KEY_4_PAD             }, // ELK_KEY_LEFT,
    { KEY_RIGHT,     KEY_6_PAD             }, // ELK_KEY_RIGHT,
    { KEY_UP,        KEY_8_PAD             }, // ELK_KEY_UP,
    { KEY_DOWN,      KEY_2_PAD             }, // ELK_KEY_DOWN,
    { KEY_TAB,       NO_ALTERNATE_KEY      }, // ELK_KEY_FUNCTION,
    { KEY_END,       NO_ALTERNATE_KEY      }, // ELK_KEY_COPY,
    { KEY_CAPSLOCK,  NO_ALTERNATE_KEY      }, // ELK_KEY_CONTROL,
    { KEY_LSHIFT,    KEY_RSHIFT            }, // ELK_KEY_SHIFT,
    { KEY_BACKSPACE, KEY_DEL               }, // ELK_KEY_DEL,
    { KEY_SPACE,     NO_ALTERNATE_KEY      }, // ELK_KEY_SPACE,
    { KEY_ENTER,     KEY_ENTER_PAD         }, // ELK_KEY_RETURN,
    { KEY_ESC,       NO_ALTERNATE_KEY      }, // ELK_KEY_ESCAPE,
    { KEY_F12,       NO_ALTERNATE_KEY      }, // ELK_KEY_BREAK,
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
bool elk_key_pressed[ELK_KEY_MAX];

// keylookup allows the user to redefine a native key with another native key(?)
uint8_t keylookup[KEY_MAX];

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
                        // c = the column number wshich is used to create the address mask
                        // d is bit number, used to correctly set the bit in the electrons memory address (if key is pressed)
                        // For examples see above.
                        keyl[keys[c][d]]=c|(d<<4)|0x80;
                }
        }

        // Reset keyboard
        for(c = 0; c < KEY_MAX; c++)
        {
            keylookup[c] = ELK_KEY_MAX; // This signifies no ELK key assignemnt
        }

        // Now assign defaults.
        for(c = 0; c < ELK_KEY_MAX; c++)
        {
            elk_key_pressed[c] = false;
            keylookup[elk_keycode_defaults[c].allegro_keycode_main] = c; // Assign PC key to elk key.
            if(elk_keycode_defaults[c].allegro_keycode_alternate)
            {
                keylookup[elk_keycode_defaults[c].allegro_keycode_alternate] = c; // Assign alternate PC key to elk key (if defined).
            }
        }

        keyboard_debug_dump();
        /* Record the Break keys separately, along with configurable menu keys. */

        update_break_keys();
        update_menu_keys();
}

void keyboard_debug_dump()
{
    int c;
    for(c = 0; c < KEY_MAX; c++)
    {
        if(keylookup[c] < ELK_KEY_MAX)
        {
            log_debug("keylookup[%d] = (%d)", c, keylookup[c]);
        }
    }
}

uint8_t keyboard_read(uint16_t addr)
{
        int d;
        uint8_t temp=0;

        // Allegro4 has a array called key which indicates which allegro keys are being pressed.
        // We need to convert that to pressed elk keys prior to "reading" the elk keyboard.
        for(d = 0; d < ELK_KEY_MAX; d++)  // TODO: Move this somewhere more efficient.
        {
            elk_key_pressed[d] = false; // Reset keyboard.
        }

        for(d = 0; d < KEY_MAX; d++)
        {
            if(keylookup[d] != ELK_KEY_MAX)
            {
                elk_key_pressed[keylookup[d]] |= key[d]; // Update keyboard if key is assigned.
            }
        }

        for (d=0;d<ELK_KEY_MAX;d++)
        {
                if (elk_key_pressed[d] && keyl[d]&0x80 && !(addr&(1<<(keyl[d]&15)))) temp|=1<<((keyl[d]&0x30)>>4);
        }

        return temp;
}