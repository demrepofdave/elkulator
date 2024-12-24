#include <allegro5/allegro.h>
#include "callback_handlers.h"
#include "config_vars.h"
#include "elk.h"
#include "host_abstraction_layer/event_handler.h"
#include "host_abstraction_layer/keyutils.h"
#include "keyboard.h"
#include "keyboard_internal.h"
#include "logger.h"
#include "video_internal.h"

// HOST_KEY get mapped to ALLEGRO_KEY.
// 
// ALLEGRO KEY pressed.. need to map to HOST_KEY or ELK_KEY.
static const uint8_t allegro_key_to_host_key_mapping[ALLEGRO_KEY_MAX] = 
{
    0,
    HOST_KEY_A,              // ALLEGRO_KEY_A
    HOST_KEY_B,              // ALLEGRO_KEY_B
    HOST_KEY_C,              // ALLEGRO_KEY_C
    HOST_KEY_D,              // ALLEGRO_KEY_D
    HOST_KEY_E,              // ALLEGRO_KEY_E
    HOST_KEY_F,              // ALLEGRO_KEY_F
    HOST_KEY_G,              // ALLEGRO_KEY_G
    HOST_KEY_H,              // ALLEGRO_KEY_H
    HOST_KEY_I,              // ALLEGRO_KEY_I
    HOST_KEY_J,              // ALLEGRO_KEY_J
    HOST_KEY_K,              // ALLEGRO_KEY_K
    HOST_KEY_L,              // ALLEGRO_KEY_L
    HOST_KEY_M,              // ALLEGRO_KEY_M
    HOST_KEY_N,              // ALLEGRO_KEY_N
    HOST_KEY_O,              // ALLEGRO_KEY_O
    HOST_KEY_P,              // ALLEGRO_KEY_P
    HOST_KEY_Q,              // ALLEGRO_KEY_Q
    HOST_KEY_R,              // ALLEGRO_KEY_R
    HOST_KEY_S,              // ALLEGRO_KEY_S
    HOST_KEY_T,              // ALLEGRO_KEY_T
    HOST_KEY_U,              // ALLEGRO_KEY_U
    HOST_KEY_V,              // ALLEGRO_KEY_V
    HOST_KEY_W,              // ALLEGRO_KEY_W
    HOST_KEY_X,              // ALLEGRO_KEY_X
    HOST_KEY_Y,              // ALLEGRO_KEY_Y
    HOST_KEY_Z,              // ALLEGRO_KEY_Z

    HOST_KEY_0,              // ALLEGRO_KEY_0
    HOST_KEY_1,              // ALLEGRO_KEY_1
    HOST_KEY_2,              // ALLEGRO_KEY_2
    HOST_KEY_3,              // ALLEGRO_KEY_3
    HOST_KEY_4,              // ALLEGRO_KEY_4
    HOST_KEY_5,              // ALLEGRO_KEY_5
    HOST_KEY_6,              // ALLEGRO_KEY_6
    HOST_KEY_7,              // ALLEGRO_KEY_7
    HOST_KEY_8,              // ALLEGRO_KEY_8
    HOST_KEY_9,              // ALLEGRO_KEY_9

    HOST_KEY_PAD_0,          // ALLEGRO_KEY_PAD_0
    HOST_KEY_PAD_1,          // ALLEGRO_KEY_PAD_1
    HOST_KEY_PAD_2,          // ALLEGRO_KEY_PAD_2
    HOST_KEY_PAD_3,          // ALLEGRO_KEY_PAD_3
    HOST_KEY_PAD_4,          // ALLEGRO_KEY_PAD_4
    HOST_KEY_PAD_5,          // ALLEGRO_KEY_PAD_5
    HOST_KEY_PAD_6,          // ALLEGRO_KEY_PAD_6
    HOST_KEY_PAD_7,          // ALLEGRO_KEY_PAD_7
    HOST_KEY_PAD_8,          // ALLEGRO_KEY_PAD_8
    HOST_KEY_PAD_9,          // ALLEGRO_KEY_PAD_9


    HOST_KEY_F1,             // ALLEGRO_KEY_F1
    HOST_KEY_F2,             // ALLEGRO_KEY_F2
    HOST_KEY_F3,             // ALLEGRO_KEY_F3
    HOST_KEY_F4,             // ALLEGRO_KEY_F4
    HOST_KEY_F5,             // ALLEGRO_KEY_F5
    HOST_KEY_F6,             // ALLEGRO_KEY_F6
    HOST_KEY_F7,             // ALLEGRO_KEY_F7
    HOST_KEY_F8,             // ALLEGRO_KEY_F8
    HOST_KEY_F9,             // ALLEGRO_KEY_F9
    HOST_KEY_F10,            // ALLEGRO_KEY_F10
    HOST_KEY_F11,            // ALLEGRO_KEY_F11
    HOST_KEY_F12,            // ALLEGRO_KEY_F12

    HOST_KEY_ESCAPE,         // ALLEGRO_KEY_ESCAPE
    HOST_KEY_TILDE,          // ALLEGRO_KEY_TILDE
    HOST_KEY_MINUS,          // ALLEGRO_KEY_MINUS
    HOST_KEY_EQUALS,         // ALLEGRO_KEY_EQUALS
    HOST_KEY_BACKSPACE,      // ALLEGRO_KEY_BACKSPACE
    HOST_KEY_TAB,            // ALLEGRO_KEY_TAB
    HOST_KEY_OPENBRACE,      // ALLEGRO_KEY_OPENBRACE
    HOST_KEY_CLOSEBRACE,     // ALLEGRO_KEY_CLOSEBRACE
    HOST_KEY_ENTER,          // ALLEGRO_KEY_ENTER
    HOST_KEY_SEMICOLON,      // ALLEGRO_KEY_SEMICOLON
    HOST_KEY_APOSTROPHE,     // ALLEGRO_KEY_QUOTE
    HOST_KEY_BACKSLASH,      // ALLEGRO_KEY_BACKSLASH,
    HOST_KEY_BACKSLASH2,     // ALLEGRO_KEY_BACKSLASH2
    HOST_KEY_COMMA,          // ALLEGRO_KEY_COMMA
    HOST_KEY_FULLSTOP,       // ALLEGRO_KEY_FULLSTOP
    HOST_KEY_SLASH,          // ALLEGRO_KEY_SLASH
    HOST_KEY_SPACE,          // ALLEGRO_KEY_SPACE
    HOST_KEY_INSERT,         // ALLEGRO_KEY_INSERT,
    HOST_KEY_DELETE,         // ALLEGRO_KEY_DELETE
    HOST_KEY_HOME,           // ALLEGRO_KEY_HOME
    HOST_KEY_END,            // ALLEGRO_KEY_END
    HOST_KEY_PAGEUP,         // ALLEGRO_KEY_PGUP
    HOST_KEY_PAGEDOWN,       // ALLEGRO_KEY_PGDN

    HOST_KEY_LEFT,           // ALLEGRO_KEY_LEFT
    HOST_KEY_RIGHT,          // ALLEGRO_KEY_RIGHT
    HOST_KEY_UP,             // ALLEGRO_KEY_UP
    HOST_KEY_DOWN,           // ALLEGRO_KEY_DOWN

    HOST_KEY_PAD_SLASH,      // ALLEGRO_KEY_PAD_SLASH
    HOST_KEY_PAD_ASTERISK,   // ALLEGRO_KEY_PAD_ASTERISK
    HOST_KEY_PAD_MINUS,      // ALLEGRO_KEY_PAD_MINUS
    HOST_KEY_PAD_PLUS,       // ALLEGRO_KEY_PAD_PLUS
    HOST_KEY_PAD_DELETE,     // ALLEGRO_KEY_PAD_DELETE
    HOST_KEY_PAD_ENTER,      // ALLEGRO_KEY_PAD_ENTER

    HOST_KEY_PRINT_SCREEN,   // ALLEGRO_KEY_PRINTSCREEN
    HOST_KEY_PAUSE,          // ALLEGRO_KEY_PAUSE

    HOST_KEY_ABNT_C1,        // ALLEGRO_KEY_ABNT_C1
    HOST_KEY_YEN,            // ALLEGRO_KEY_YEN
    HOST_KEY_KANA,           // ALLEGRO_KEY_KANA
    HOST_KEY_CONVERT,        // ALLEGRO_KEY_CONVERT,
    HOST_KEY_NOCONVERT,      // ALLEGRO_KEY_NOCONVERT,
    HOST_KEY_AT,             // ALLEGRO_KEY_AT  
    HOST_KEY_CIRCUMFLEX,     // ALLEGRO_KEY_CIRCUMFLEX
    HOST_KEY_COLON2,         // ALLEGRO_KEY_COLON2
    HOST_KEY_KANJI,          // ALLEGRO_KEY_KANJI
    HOST_KEY_PAD_EQUALS,     // ALLEGRO_KEY_PAD_EQUALS,
    HOST_KEY_BACKQUOTE,      // ALLEGRO_KEY_BACKQUOTE
    HOST_KEY_SEMICOLON2,     // ALLEGRO_KEY_SEMICOLON2
    HOST_KEY_COMMAND,        // ALLEGRO_KEY_COMMAND

    HOST_KEY_NONE,           // ALLEGRO_KEY_BACK
    HOST_KEY_NONE,           // ALLEGRO_KEY_VOLUME_UP
    HOST_KEY_NONE,           // ALLEGRO_KEY_VOLUME_DOWN
                        
    HOST_KEY_NONE,           // ALLEGRO_KEY_SEARCH
    HOST_KEY_NONE,           // ALLEGRO_KEY_DPAD_CENTER
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_X
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_Y
    HOST_KEY_NONE,           // ALLEGRO_KEY_DPAD_UP
    HOST_KEY_NONE,           // ALLEGRO_KEY_DPAD_DOWN
    HOST_KEY_NONE,           // ALLEGRO_KEY_DPAD_LEFT
    HOST_KEY_NONE,           // ALLEGRO_KEY_DPAD_RIGHT
    HOST_KEY_NONE,           // ALLEGRO_KEY_SELECT
    HOST_KEY_NONE,           // ALLEGRO_KEY_START
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_L1
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_R1
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_L2
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_R2
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_A
    HOST_KEY_NONE,           // ALLEGRO_KEY_BUTTON_B
    HOST_KEY_NONE,           // ALLEGRO_KEY_THUMBL
    HOST_KEY_NONE,           // ALLEGRO_KEY_THUMBR
    
    HOST_KEY_NONE,           // ALLEGRO_KEY_UNKNOWN
  
    HOST_KEY_NONE,           // No allegro key code (129)
    HOST_KEY_NONE,           // No allegro key code (130)
    HOST_KEY_NONE,           // No allegro key code (131)
    HOST_KEY_NONE,           // No allegro key code (132)
    HOST_KEY_NONE,           // No allegro key code (133)
    HOST_KEY_NONE,           // No allegro key code (134)
    HOST_KEY_NONE,           // No allegro key code (135)
    HOST_KEY_NONE,           // No allegro key code (136)
    HOST_KEY_NONE,           // No allegro key code (137)
    HOST_KEY_NONE,           // No allegro key code (138)
    HOST_KEY_NONE,           // No allegro key code (139)

    HOST_KEY_NONE,           // No allegro key code (140)
    HOST_KEY_NONE,           // No allegro key code (141)
    HOST_KEY_NONE,           // No allegro key code (142)
    HOST_KEY_NONE,           // No allegro key code (143)
    HOST_KEY_NONE,           // No allegro key code (144)
    HOST_KEY_NONE,           // No allegro key code (145)
    HOST_KEY_NONE,           // No allegro key code (146)
    HOST_KEY_NONE,           // No allegro key code (147)
    HOST_KEY_NONE,           // No allegro key code (148)
    HOST_KEY_NONE,           // No allegro key code (149)

    HOST_KEY_NONE,           // No allegro key code (150)
    HOST_KEY_NONE,           // No allegro key code (151)
    HOST_KEY_NONE,           // No allegro key code (152)
    HOST_KEY_NONE,           // No allegro key code (153)
    HOST_KEY_NONE,           // No allegro key code (154)
    HOST_KEY_NONE,           // No allegro key code (155)
    HOST_KEY_NONE,           // No allegro key code (156)
    HOST_KEY_NONE,           // No allegro key code (157)
    HOST_KEY_NONE,           // No allegro key code (158)
    HOST_KEY_NONE,           // No allegro key code (159)

    HOST_KEY_NONE,           // No allegro key code (160)
    HOST_KEY_NONE,           // No allegro key code (161)
    HOST_KEY_NONE,           // No allegro key code (162)
    HOST_KEY_NONE,           // No allegro key code (163)
    HOST_KEY_NONE,           // No allegro key code (164)
    HOST_KEY_NONE,           // No allegro key code (165)
    HOST_KEY_NONE,           // No allegro key code (166)
    HOST_KEY_NONE,           // No allegro key code (167)
    HOST_KEY_NONE,           // No allegro key code (168)
    HOST_KEY_NONE,           // No allegro key code (169)

    HOST_KEY_NONE,           // No allegro key code (170)
    HOST_KEY_NONE,           // No allegro key code (171)
    HOST_KEY_NONE,           // No allegro key code (172)
    HOST_KEY_NONE,           // No allegro key code (173)
    HOST_KEY_NONE,           // No allegro key code (174)
    HOST_KEY_NONE,           // No allegro key code (175)
    HOST_KEY_NONE,           // No allegro key code (176)
    HOST_KEY_NONE,           // No allegro key code (177)
    HOST_KEY_NONE,           // No allegro key code (178)
    HOST_KEY_NONE,           // No allegro key code (179)


    HOST_KEY_NONE,           // No allegro key code (180)
    HOST_KEY_NONE,           // No allegro key code (181)
    HOST_KEY_NONE,           // No allegro key code (182)
    HOST_KEY_NONE,           // No allegro key code (183)
    HOST_KEY_NONE,           // No allegro key code (184)
    HOST_KEY_NONE,           // No allegro key code (185)
    HOST_KEY_NONE,           // No allegro key code (186)
    HOST_KEY_NONE,           // No allegro key code (187)
    HOST_KEY_NONE,           // No allegro key code (188)
    HOST_KEY_NONE,           // No allegro key code (189)

    HOST_KEY_NONE,           // No allegro key code (190)
    HOST_KEY_NONE,           // No allegro key code (191)
    HOST_KEY_NONE,           // No allegro key code (192)
    HOST_KEY_NONE,           // No allegro key code (193)
    HOST_KEY_NONE,           // No allegro key code (194)
    HOST_KEY_NONE,           // No allegro key code (195)
    HOST_KEY_NONE,           // No allegro key code (196)
    HOST_KEY_NONE,           // No allegro key code (197)
    HOST_KEY_NONE,           // No allegro key code (198)
    HOST_KEY_NONE,           // No allegro key code (199)

    HOST_KEY_NONE,           // No allegro key code (200)
    HOST_KEY_NONE,           // No allegro key code (201)
    HOST_KEY_NONE,           // No allegro key code (202)
    HOST_KEY_NONE,           // No allegro key code (203)
    HOST_KEY_NONE,           // No allegro key code (204)
    HOST_KEY_NONE,           // No allegro key code (205)
    HOST_KEY_NONE,           // No allegro key code (206)
    HOST_KEY_NONE,           // No allegro key code (207)
    HOST_KEY_NONE,           // No allegro key code (208)
    HOST_KEY_NONE,           // No allegro key code (209)

    HOST_KEY_NONE,           // No allegro key code (210)
    HOST_KEY_NONE,           // No allegro key code (211)
    HOST_KEY_NONE,           // No allegro key code (212)
    HOST_KEY_NONE,           // No allegro key code (213)
    HOST_KEY_NONE,           // No allegro key code (214)

    HOST_KEY_LSHIFT,         // ALLEGRO_KEY_LSHIFT
    HOST_KEY_RSHIFT,         // ALLEGRO_KEY_RSHIFT
    HOST_KEY_LCTRL,          // ALLEGRO_KEY_LCTRL
    HOST_KEY_RCTRL,          // ALLEGRO_KEY_RCTRL
    HOST_KEY_ALT,            // ALLEGRO_KEY_ALT
    HOST_KEY_ALTGR,          // ALLEGRO_KEY_ALTGR

    HOST_KEY_LWIN,           // ALLEGRO_KEY_LWIN
    HOST_KEY_RWIN,           // ALLEGRO_KEY_RWIN
    HOST_KEY_MENU,           // ALLEGRO_KEY_MENU
    HOST_KEY_SCROLLLOCK,     // ALLEGRO_KEY_SCROLLLOCK
    HOST_KEY_NUMLOCK,        // ALLEGRO_KEY_NUMLOCK
    HOST_KEY_CAPSLOCK        // ALLEGRO_KEY_CAPSLOCK
};

bool keydefining = false;

uint8_t keyhandler_allegro5_key_to_host_key(uint8_t allegro_key)
{
    uint8_t host_key = HOST_KEY_NONE;
    if(allegro_key < ALLEGRO_KEY_MAX)
    {
        host_key = allegro_key_to_host_key_mapping[allegro_key];
    }
    return host_key;
}

void keyhandler_refresh_elkkeys()
{
    // Nothing to do in allegro5 (keys always have the correct state)
}


void key_down_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
//    if (keycode == ALLEGRO_KEY_ALT || keycode == ALLEGRO_KEY_ALTGR)
//        hostalt = true;
//    else if (keycode == ALLEGRO_KEY_CAPSLOCK)
//            key_down(elkConfig.keyboard.host_key_mapping[keycode]);
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
                host_key_t hostkey = keyhandler_allegro5_key_to_host_key(keycode);
                if(hostkey != HOST_KEY_NONE)
                {
                    callback_handlers.handle_key_up(hostkey);
                }
//        }
//    }
}

void key_up_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
    if (keycode < ALLEGRO_KEY_MAX)
    { 
        host_key_t hostkey = keyhandler_allegro5_key_to_host_key(keycode);
        if(hostkey != HOST_KEY_NONE)
        {
            callback_handlers.handle_key_up(hostkey);
        }
//        if (keycode == ALLEGRO_KEY_ALT || keycode == ALLEGRO_KEY_ALTGR)
//            hostalt = false;
//        else if (keycode == ALLEGRO_KEY_CAPSLOCK)
//            key_up(elkConfig.keyboard.host_key_mapping[keycode]);
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
//                key_up(elkConfig.keyboard.host_key_mapping[keycode]);
//        }
    }
}

//void key_char_event(ALLEGRO_EVENT *event)
//{
//    int keycode = event->keyboard.keycode;
//    log_debug("key char event, keycode=%d", keycode);
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
//            key_down(elkConfig.keyboard.host_key_mapping[keycode]);
//    }
//}


// Main event handling Code
elk_event_t key_handler_handle_event(ALLEGRO_EVENT *event)
{
    elk_event_t elkEvent = 0;

    // Was the key pressed in our main screen?
    // If not it was on the keyboard redefining screen
    // so we ignore.
    if (!keydefining)
    {
        switch(event->type)
        {
            // Keyboard handling.
            case ALLEGRO_EVENT_KEY_DOWN:
                key_down_event(event);
                break;

            // Don't seem to need this for the moment.
//            case ALLEGRO_EVENT_KEY_CHAR:
//                key_char_event(event);
//                break;

            case ALLEGRO_EVENT_KEY_UP:
                key_up_event(event);
                break;
        }
    }
    return elkEvent;
}
