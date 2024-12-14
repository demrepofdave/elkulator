#include <allegro5/allegro.h>
#include "host_abstraction_layer/keyboard.h"
#include "host_abstraction_layer/keyutils.h"
#include "keyboard_internal.h"
#include "elk.h"
#include "host_abstraction_layer/event_handler.h"
#include "logger.h"
#include "config_vars.h"

#define NO_ALTERNATE_KEY HOST_KEY_MAX
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
    { NO_ALTERNATE_KEY,   NO_ALTERNATE_KEY   }  // ELK_SPECIAL_KEY_MENU (not used as native menus supported in allegro 5).
};


// Host key to allegro key and string lookup?
// HOST_KEY_xx=ELK_KEY_xx
//

// HOST_KEY get mapped to ALLEGRO_KEY.
// 
// ALLEGRO KEY pressed.. need to map to HOST_KEY or ELK_KEY.
// keylookup is populated from here.
static const uint8_t host_key_mapping_defaults[HOST_KEY_MAX] = 
{
    0,
    ALLEGRO_KEY_A,              // HOST_KEY_A
    ALLEGRO_KEY_B,              // HOST_KEY_B
    ALLEGRO_KEY_C,              // HOST_KEY_C
    ALLEGRO_KEY_D,              // HOST_KEY_D
    ALLEGRO_KEY_E,              // HOST_KEY_E
    ALLEGRO_KEY_F,              // HOST_KEY_F
    ALLEGRO_KEY_G,              // HOST_KEY_G
    ALLEGRO_KEY_H,              // HOST_KEY_H
    ALLEGRO_KEY_I,              // HOST_KEY_I
    ALLEGRO_KEY_J,              // HOST_KEY_J
    ALLEGRO_KEY_K,              // HOST_KEY_K
    ALLEGRO_KEY_L,              // HOST_KEY_L
    ALLEGRO_KEY_M,              // HOST_KEY_M
    ALLEGRO_KEY_N,              // HOST_KEY_N
    ALLEGRO_KEY_O,              // HOST_KEY_O
    ALLEGRO_KEY_P,              // HOST_KEY_P
    ALLEGRO_KEY_Q,              // HOST_KEY_Q
    ALLEGRO_KEY_R,              // HOST_KEY_R
    ALLEGRO_KEY_S,              // HOST_KEY_S
    ALLEGRO_KEY_T,              // HOST_KEY_T
    ALLEGRO_KEY_U,              // HOST_KEY_U
    ALLEGRO_KEY_V,              // HOST_KEY_V
    ALLEGRO_KEY_W,              // HOST_KEY_W
    ALLEGRO_KEY_X,              // HOST_KEY_X
    ALLEGRO_KEY_Y,              // HOST_KEY_Y
    ALLEGRO_KEY_Z,              // HOST_KEY_Z

    ALLEGRO_KEY_0,              // HOST_KEY_0
    ALLEGRO_KEY_1,              // HOST_KEY_1
    ALLEGRO_KEY_2,              // HOST_KEY_2
    ALLEGRO_KEY_3,              // HOST_KEY_3
    ALLEGRO_KEY_4,              // HOST_KEY_4
    ALLEGRO_KEY_5,              // HOST_KEY_5
    ALLEGRO_KEY_6,              // HOST_KEY_6
    ALLEGRO_KEY_7,              // HOST_KEY_7
    ALLEGRO_KEY_8,              // HOST_KEY_8
    ALLEGRO_KEY_9,              // HOST_KEY_9

    ALLEGRO_KEY_PAD_0,          // HOST_KEY_PAD_0
    ALLEGRO_KEY_PAD_1,          // HOST_KEY_PAD_1
    ALLEGRO_KEY_PAD_2,          // HOST_KEY_PAD_2
    ALLEGRO_KEY_PAD_3,          // HOST_KEY_PAD_3
    ALLEGRO_KEY_PAD_4,          // HOST_KEY_PAD_4
    ALLEGRO_KEY_PAD_5,          // HOST_KEY_PAD_5
    ALLEGRO_KEY_PAD_6,          // HOST_KEY_PAD_6
    ALLEGRO_KEY_PAD_7,          // HOST_KEY_PAD_7
    ALLEGRO_KEY_PAD_8,          // HOST_KEY_PAD_8
    ALLEGRO_KEY_PAD_9,          // HOST_KEY_PAD_9


    ALLEGRO_KEY_F1,             // HOST_KEY_F1
    ALLEGRO_KEY_F2,             // HOST_KEY_F2
    ALLEGRO_KEY_F3,             // HOST_KEY_F3
    ALLEGRO_KEY_F4,             // HOST_KEY_F4
    ALLEGRO_KEY_F5,             // HOST_KEY_F5
    ALLEGRO_KEY_F6,             // HOST_KEY_F6
    ALLEGRO_KEY_F7,             // HOST_KEY_F7
    ALLEGRO_KEY_F8,             // HOST_KEY_F8
    ALLEGRO_KEY_F9,             // HOST_KEY_F9
    ALLEGRO_KEY_F10,            // HOST_KEY_F10
    ALLEGRO_KEY_F11,            // HOST_KEY_F11
    ALLEGRO_KEY_F12,            // HOST_KEY_F12

    ALLEGRO_KEY_ESCAPE,         // HOST_KEY_ESCAPE
    ALLEGRO_KEY_TILDE,          // HOST_KEY_TILDE
    ALLEGRO_KEY_MINUS,          // HOST_KEY_MINUS
    ALLEGRO_KEY_EQUALS,         // HOST_KEY_EQUALS
    ALLEGRO_KEY_BACKSPACE,      // HOST_KEY_BACKSPACE
    ALLEGRO_KEY_TAB,            // HOST_KEY_TAB
    ALLEGRO_KEY_OPENBRACE,      // HOST_KEY_OPENBRACE
    ALLEGRO_KEY_CLOSEBRACE,     // HOST_KEY_CLOSEBRACE
    ALLEGRO_KEY_ENTER,          // HOST_KEY_ENTER
    ALLEGRO_KEY_SEMICOLON,      // HOST_KEY_SEMICOLON
    ALLEGRO_KEY_QUOTE,          // HOST_KEY_APOSTROPHE
    ALLEGRO_KEY_BACKSLASH,      // HOST_KEY_BACKSLASH,
    ALLEGRO_KEY_BACKSLASH2,     // HOST_KEY_BACKSLASH2
    ALLEGRO_KEY_COMMA,          // HOST_KEY_COMMA
    ALLEGRO_KEY_FULLSTOP,       // HOST_KEY_FULLSTOP
    ALLEGRO_KEY_SLASH,          // HOST_KEY_SLASH
    ALLEGRO_KEY_SPACE,          // HOST_KEY_SPACE
    ALLEGRO_KEY_INSERT,         // HOST_KEY_INSERT,
    ALLEGRO_KEY_DELETE,         // HOST_KEY_DELETE
    ALLEGRO_KEY_HOME,           // HOST_KEY_HOME
    ALLEGRO_KEY_END,            // HOST_KEY_END
    ALLEGRO_KEY_PGUP,           // HOST_KEY_PAGEUP
    ALLEGRO_KEY_PGDN,           // HOST_KEY_PAGEDOWN

    ALLEGRO_KEY_LEFT,           // HOST_KEY_LEFT
    ALLEGRO_KEY_RIGHT,          // HOST_KEY_RIGHT
    ALLEGRO_KEY_UP,             // HOST_KEY_UP
    ALLEGRO_KEY_DOWN,           // HOST_KEY_DOWN

    ALLEGRO_KEY_PAD_SLASH,      // HOST_KEY_PAD_SLASH
    ALLEGRO_KEY_PAD_ASTERISK,   // HOST_KEY_PAD_ASTERISK
    ALLEGRO_KEY_PAD_MINUS,      // HOST_KEY_PAD_MINUS
    ALLEGRO_KEY_PAD_PLUS,       // HOST_KEY_PAD_PLUS
    ALLEGRO_KEY_PAD_DELETE,     // HOST_KEY_PAD_DELETE
    ALLEGRO_KEY_PAD_ENTER,      // HOST_KEY_PAD_ENTER

    ALLEGRO_KEY_PRINTSCREEN,    // HOST_KEY_PRINT_SCREEN
    ALLEGRO_KEY_PAUSE,          // HOST_KEY_PAUSE
    ALLEGRO_KEY_AT,             // HOST_KEY_AT

    ALLEGRO_KEY_ABNT_C1,        // HOST_KEY_ABNT_C1
    ALLEGRO_KEY_YEN,            // HOST_KEY_YEN
    ALLEGRO_KEY_KANA,           // HOST_KEY_KANA
    ALLEGRO_KEY_CONVERT,        // HOST_KEY_CONVERT,
    ALLEGRO_KEY_NOCONVERT,      // HOST_KEY_NOCONVERT,
    ALLEGRO_KEY_CIRCUMFLEX,     // HOST_KEY_CIRCUMFLEX
    ALLEGRO_KEY_COLON2,         // HOST_KEY_COLON2
    ALLEGRO_KEY_KANJI,          // HOST_KEY_KANJI
    ALLEGRO_KEY_PAD_EQUALS,     // HOST_KEY_PAD_EQUALS,
    ALLEGRO_KEY_BACKQUOTE,      // HOST_KEY_BACKQUOTE
    ALLEGRO_KEY_SEMICOLON2,     // HOST_KEY_SEMICOLON2
    ALLEGRO_KEY_COMMAND,        // HOST_KEY_COMMAND
    
    ALLEGRO_KEY_LSHIFT,         // HOST_KEY_LSHIFT
    ALLEGRO_KEY_RSHIFT,         // HOST_KEY_RSHIFT
    ALLEGRO_KEY_LCTRL,          // HOST_KEY_LCTRL
    ALLEGRO_KEY_RCTRL,          // HOST_KEY_RCTRL
    ALLEGRO_KEY_ALT,            // HOST_KEY_ALT
    ALLEGRO_KEY_ALTGR,          // HOST_KEY_ALTGR

    ALLEGRO_KEY_LWIN,           // HOST_KEY_LWIN
    ALLEGRO_KEY_RWIN,           // HOST_KEY_RWIN
    ALLEGRO_KEY_MENU,           // HOST_KEY_MENU
    ALLEGRO_KEY_SCROLLLOCK,     // HOST_KEY_SCROLLLOCK
    ALLEGRO_KEY_NUMLOCK,        // HOST_KEY_NUMLOCK
    ALLEGRO_KEY_CAPSLOCK,       // HOST_KEY_CAPSLOCK
};

// Records if native key is pressed or not (true = pressed, false = not pressed)
bool elk_key_state[ELK_KEY_MAX];

// keylookup allows the user to redefine a native key with another native key(?)
uint8_t keylookup[ALLEGRO_KEY_MAX];

bool keydefining = false;

uint8_t keyboard_host_key_to_allegro5_key(host_key_t host_key)
{
    uint8_t allegro_key = 0;
    if(host_key < HOST_KEY_MAX)
    {
        allegro_key = host_key_mapping_defaults[host_key];
    }
    return allegro_key;
}

void keyboard_makelayout()
{
    int c;

    log_debug("Make layout");
    // Reset keyboard
    for(c = 0; c < ALLEGRO_KEY_MAX; c++)
    {
        keylookup[c] = ELK_KEY_MAX; // This signifies no ELK key assignemnt
    }

    // Now assign defaults.
    for(c = 0; c < ELK_KEY_MAX; c++)
    {
        elk_key_state[c] = false;
        keylookup[keyboard_host_key_to_allegro5_key(elk_keycode_defaults[c].host_keycode_main)] = c; // Assign PC key to elk key.
        if(elk_keycode_defaults[c].host_keycode_alternate != NO_ALTERNATE_KEY)
        {
            keylookup[keyboard_host_key_to_allegro5_key(elk_keycode_defaults[c].host_keycode_alternate)] = c; // Assign alternate PC key to elk key (if defined).
        }
    }

    // Now assign any keyboard changes made in the config file.
    for(c = 0; c < HOST_KEY_MAX; c++)
    {
        if(elkConfig.keyboard.host_key_mapping[c] != ELK_KEY_NONE)
        {
            log_debug("%s=%s", keyutils_get_hostkey_config_string(c), keyutils_get_elkkey_config_string(elkConfig.keyboard.host_key_mapping[c]));
            keylookup[c] = elkConfig.keyboard.host_key_mapping[c];
        }
    }
}

elk_key_id_t get_elk_key_from_host_key(host_key_t host_key)
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

void keyboard_debug_dump()
{
    int c;
    for(c = 0; c< ALLEGRO_KEY_MAX; c++)
    {
        if(keylookup[c] < ELK_KEY_MAX)
        {
            log_debug("keylookup[%s]=%s", keyutils_get_hostkey_config_string(c), keyutils_get_elkkey_config_string(keylookup[c]));
        }
    }
}

void keyhandler_refresh_elkkeys()
{
    // Nothing to do in allegro5 (keys always have the correct state)
}

bool keyhandler_elk_key_state(elk_key_id_t elk_key_code)
{
    return(elk_key_state[elk_key_code]);
}


void key_down(uint8_t allegro_keycode)
{
    elk_key_id_t elkkey = keylookup[allegro_keycode];

    if(elkkey != ELK_KEY_MAX)
    {
        log_debug("keycode %d, elkkey %s", allegro_keycode, keyutils_get_elkkey_longname(elkkey));
        elk_key_state[keylookup[allegro_keycode]] = true;
    }
    else
    {
        log_debug("keycode=%d", allegro_keycode);
    }
}

void key_down_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
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
    elk_key_id_t elkkey = keylookup[allegro_keycode];

    // TODO: Allegro to elk key conversion.
    if(elkkey != ELK_KEY_MAX)
    {
        log_debug("keycode %d, elkkey %s", allegro_keycode, keyutils_get_elkkey_longname(elkkey));
        elk_key_state[keylookup[allegro_keycode]] = false;
    }
    else
    {
        log_debug("keycode=%d", allegro_keycode);
    }
}

void key_up_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
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
    log_debug("key char event, keycode=%d", keycode);
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
