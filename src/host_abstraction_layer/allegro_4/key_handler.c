#include "host_abstraction_layer/keyboard.h"
#include "elk.h"
#include "logger.h"
#include <allegro.h>
#include <stdbool.h>


#define NO_ALTERNATE_KEY 0
typedef struct {
    uint8_t host_keycode_main;
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
    { KEY_STOP,      NO_ALTERNATE_KEY      }, // ELK_KEY_FULLSTOP,
    { KEY_SLASH,     NO_ALTERNATE_KEY      }, // ELK_KEY_SLASH,
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
    { KEY_MENU,      NO_ALTERNATE_KEY      } // ELK_SPECIAL_KEY_MENU
};

// Records if native key is pressed or not (true = pressed, false = not pressed)
bool elk_key_state[ELK_KEY_MAX];

// keylookup allows the user to redefine a native key with another native key(?)
uint8_t keylookup[KEY_MAX];

void keyboard_makelayout()
{
        int c;

        // Reset keyboard
        for(c = 0; c < KEY_MAX; c++)
        {
            keylookup[c] = ELK_KEY_MAX; // This signifies no ELK key assignemnt
        }

        // Now assign defaults.
        for(c = 0; c < ELK_KEY_MAX; c++)
        {
            elk_key_state[c] = false;
            keylookup[elk_keycode_defaults[c].host_keycode_main] = c; // Assign PC key to elk key.
            if(elk_keycode_defaults[c].allegro_keycode_alternate)
            {
                keylookup[elk_keycode_defaults[c].allegro_keycode_alternate] = c; // Assign alternate PC key to elk key (if defined).
            }
        }
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

void keyhandler_refresh_elkkeys()
{
        int d;

        // Allegro4 has a array called key which indicates which allegro keys are being pressed.
        // We need to convert that to pressed elk keys prior to "reading" the elk keyboard.
        for(d = 0; d < ELK_KEY_MAX; d++)  // TODO: Move this somewhere more efficient.
        {
            elk_key_state[d] = false; // Reset keyboard.
        }

        for(d = 0; d < KEY_MAX; d++)
        {
            if(keylookup[d] != ELK_KEY_MAX)
            {
                elk_key_state[keylookup[d]] |= key[d]; // Update keyboard if key is assigned.
            }
        }

}

bool keyhandler_elk_key_state(elk_key_id_t elk_key_code)
{
    return(elk_key_state[elk_key_code]);
}
