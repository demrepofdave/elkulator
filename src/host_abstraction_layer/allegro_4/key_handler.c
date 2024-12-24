#include "callback_handlers.h"
#include "keyboard.h"
#include "host_abstraction_layer/keyutils.h"
#include "elk.h"
#include "logger.h"
#include <allegro.h>
#include <stdbool.h>


// HOST_KEY get mapped to ALLEGRO_KEY.
// 
// ALLEGRO KEY pressed.. need to map to HOST_KEY or ELK_KEY.
static const uint8_t allegro_key_to_host_key_mapping[KEY_MAX] = 
{
    0,
    HOST_KEY_A,              // KEY_A
    HOST_KEY_B,              // KEY_B
    HOST_KEY_C,              // KEY_C
    HOST_KEY_D,              // KEY_D
    HOST_KEY_E,              // KEY_E
    HOST_KEY_F,              // KEY_F
    HOST_KEY_G,              // KEY_G
    HOST_KEY_H,              // KEY_H
    HOST_KEY_I,              // KEY_I
    HOST_KEY_J,              // KEY_J
    HOST_KEY_K,              // KEY_K
    HOST_KEY_L,              // KEY_L
    HOST_KEY_M,              // KEY_M
    HOST_KEY_N,              // KEY_N
    HOST_KEY_O,              // KEY_O
    HOST_KEY_P,              // KEY_P
    HOST_KEY_Q,              // KEY_Q
    HOST_KEY_R,              // KEY_R
    HOST_KEY_S,              // KEY_S
    HOST_KEY_T,              // KEY_T
    HOST_KEY_U,              // KEY_U
    HOST_KEY_V,              // KEY_V
    HOST_KEY_W,              // KEY_W
    HOST_KEY_X,              // KEY_X
    HOST_KEY_Y,              // KEY_Y
    HOST_KEY_Z,              // KEY_Z

    HOST_KEY_0,              // KEY_0
    HOST_KEY_1,              // KEY_1
    HOST_KEY_2,              // KEY_2
    HOST_KEY_3,              // KEY_3
    HOST_KEY_4,              // KEY_4
    HOST_KEY_5,              // KEY_5
    HOST_KEY_6,              // KEY_6
    HOST_KEY_7,              // KEY_7
    HOST_KEY_8,              // KEY_8
    HOST_KEY_9,              // KEY_9

    HOST_KEY_PAD_0,          // KEY_PAD_0
    HOST_KEY_PAD_1,          // KEY_PAD_1
    HOST_KEY_PAD_2,          // KEY_PAD_2
    HOST_KEY_PAD_3,          // KEY_PAD_3
    HOST_KEY_PAD_4,          // KEY_PAD_4
    HOST_KEY_PAD_5,          // KEY_PAD_5
    HOST_KEY_PAD_6,          // KEY_PAD_6
    HOST_KEY_PAD_7,          // KEY_PAD_7
    HOST_KEY_PAD_8,          // KEY_PAD_8
    HOST_KEY_PAD_9,          // KEY_PAD_9

    HOST_KEY_F1,             // KEY_F1
    HOST_KEY_F2,             // KEY_F2
    HOST_KEY_F3,             // KEY_F3
    HOST_KEY_F4,             // KEY_F4
    HOST_KEY_F5,             // KEY_F5
    HOST_KEY_F6,             // KEY_F6
    HOST_KEY_F7,             // KEY_F7
    HOST_KEY_F8,             // KEY_F8
    HOST_KEY_F9,             // KEY_F9
    HOST_KEY_F10,            // KEY_F10
    HOST_KEY_F11,            // KEY_F11
    HOST_KEY_F12,            // KEY_F12

    HOST_KEY_ESCAPE,         // KEY_ESCAPE
    HOST_KEY_TILDE,          // KEY_TILDE
    HOST_KEY_MINUS,          // KEY_MINUS
    HOST_KEY_EQUALS,         // KEY_EQUALS
    HOST_KEY_BACKSPACE,      // KEY_BACKSPACE
    HOST_KEY_TAB,            // KEY_TAB
    HOST_KEY_OPENBRACE,      // KEY_OPENBRACE
    HOST_KEY_CLOSEBRACE,     // KEY_CLOSEBRACE
    HOST_KEY_ENTER,          // KEY_ENTER
    HOST_KEY_SEMICOLON,      // KEY_SEMICOLON
    HOST_KEY_APOSTROPHE,     // KEY_QUOTE
    HOST_KEY_BACKSLASH,      // KEY_BACKSLASH,
    HOST_KEY_BACKSLASH2,     // KEY_BACKSLASH2
    HOST_KEY_COMMA,          // KEY_COMMA
    HOST_KEY_FULLSTOP,       // KEY_FULLSTOP
    HOST_KEY_SLASH,          // KEY_SLASH
    HOST_KEY_SPACE,          // KEY_SPACE
    HOST_KEY_INSERT,         // KEY_INSERT,
    HOST_KEY_DELETE,         // KEY_DELETE
    HOST_KEY_HOME,           // KEY_HOME
    HOST_KEY_END,            // KEY_END
    HOST_KEY_PAGEUP,         // KEY_PGUP
    HOST_KEY_PAGEDOWN,       // KEY_PGDN

    HOST_KEY_LEFT,           // KEY_LEFT
    HOST_KEY_RIGHT,          // KEY_RIGHT
    HOST_KEY_UP,             // KEY_UP
    HOST_KEY_DOWN,           // KEY_DOWN

    HOST_KEY_PAD_SLASH,      // KEY_SLASH_PAD
    HOST_KEY_PAD_ASTERISK,   // KEY_ASTERISK
    HOST_KEY_PAD_MINUS,      // KEY_MINUS_PAD
    HOST_KEY_PAD_PLUS,       // KEY_PLUS_PAD
    HOST_KEY_PAD_DELETE,     // KEY_DELETE_PAD
    HOST_KEY_PAD_ENTER,      // KEY_ENTER_PAD

    HOST_KEY_PRINT_SCREEN,   // KEY_PRTSCR
    HOST_KEY_PAUSE,          // KEY_PAUSE

    HOST_KEY_ABNT_C1,        // KEY_ABNT_C1
    HOST_KEY_YEN,            // KEY_YEN
    HOST_KEY_KANA,           // KEY_KANA
    HOST_KEY_CONVERT,        // KEY_CONVERT,
    HOST_KEY_NOCONVERT,      // KEY_NOCONVERT,
    HOST_KEY_AT,             // KEY_AT  
    HOST_KEY_CIRCUMFLEX,     // KEY_CIRCUMFLEX
    HOST_KEY_COLON2,         // KEY_COLON2
    HOST_KEY_KANJI,          // KEY_KANJI
    HOST_KEY_PAD_EQUALS,     // KEY_EQUALS_PAD
    HOST_KEY_BACKQUOTE,      // KEY_BACKQUOTE
    HOST_KEY_SEMICOLON2,     // KEY_SEMICOLON
    HOST_KEY_COMMAND,        // KEY_COMMAND

    HOST_KEY_NONE,           // KEY_UNKNOWN1
    HOST_KEY_NONE,           // KEY_UNKNOWN2
    HOST_KEY_NONE,           // KEY_UNKNOWN3
    HOST_KEY_NONE,           //  KEY_UNKNOWN4
    HOST_KEY_NONE,           // KEY_UNKNOWN5
    HOST_KEY_NONE,           // KEY_UNKNOWN6
    HOST_KEY_NONE,           // KEY_UNKNOWN7
    HOST_KEY_NONE,           // KEY_UNKNOWN8

    HOST_KEY_LSHIFT,         // KEY_LSHIFT
    HOST_KEY_RSHIFT,         // KEY_RSHIFT
    HOST_KEY_LCTRL,          // KEY_LCONTROL
    HOST_KEY_RCTRL,          // KEY_RCONTROL
    HOST_KEY_ALT,            // KEY_ALT
    HOST_KEY_ALTGR,          // KEY_ALTGR

    HOST_KEY_LWIN,           // KEY_LWIN
    HOST_KEY_RWIN,           // KEY_RWIN
    HOST_KEY_MENU,           // KEY_MENU
    HOST_KEY_SCROLLLOCK,     // KEY_SCRLOCK
    HOST_KEY_NUMLOCK,        // KEY_NUMLOCK
    HOST_KEY_CAPSLOCK        // KEY_CAPSLOCK
};

bool allegro4_keystate[KEY_MAX];

uint8_t keyhandler_allegro4_key_to_host_key(uint8_t allegro_key)
{
    uint8_t host_key = HOST_KEY_NONE;
    if(allegro_key < KEY_MAX)
    {
        host_key = allegro_key_to_host_key_mapping[allegro_key];
    }
    return host_key;
}

void keyhandler_init()
{
    int key;

    for(key = 0; key < HOST_KEY_MAX; key++)
    {
        allegro4_keystate[key] = false;
    }
}

void keyhandler_refresh_elkkeys()
{
    int allegro4_key;
    host_key_t hostkeyid;

    // Detect any change from the last time this function was called and run the
    // correct keyboard callbacks so that the electron logic can decide
    for(allegro4_key = 0; allegro4_key < KEY_MAX; allegro4_key++)
    {
        if(allegro4_keystate[allegro4_key] != key[allegro4_key])
        {
            hostkeyid = keyhandler_allegro4_key_to_host_key(allegro4_key);
            // Get hostkey id and report if necessary.
            if(hostkeyid != HOST_KEY_NONE)
            {
                if(key[allegro4_key])
                {
                    callback_handlers.handle_key_down(hostkeyid);
                }
                else
                {
                    callback_handlers.handle_key_up(hostkeyid);
                }
            }
            allegro4_keystate[allegro4_key] = key[allegro4_key];
        }
    }
}
