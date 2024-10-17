#include <allegro5/allegro.h>
#include "common/keyboard.h"
#include "elk.h"
#include "common/event_handler.h"
#include "logger.h"


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

int keyl[ALLEGRO_KEY_MAX];

int keylookup[ALLEGRO_KEY_MAX];

void keyboard_makelayout()
{
        int c,d,e;
        memset(keyl,0,sizeof(keyl));

        /* Establish a mapping from emulated key presses to keyboard matrix values. */

        for (c=0;c<14;c++)
        {
                for (d=0;d<4;d++)
                {
                        for (e=0;e<2;e++)
                        {
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
                    //log_debug("keyboard_read - key detected %d\n", d);
                    //log_debug("keylookup[%d] is %d\n,", d, keylookup[d]);
                    if(keyl[keylookup[d]]&0x80 && !(addr&(1<<(keyl[keylookup[d]]&15))))
                    {
                        temp|=1<<((keyl[keylookup[d]]&0x30)>>4);
                    }
                }
        }

        return temp;
}


void key_down(uint8_t allegro_keycode)
{
    log_debug("keydown %d\n", allegro_keycode);
    key[allegro_keycode] = true;
}

void key_down_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
    log_debug("keyboard: key down event, keycode=%d:%s, modifiers=%04X\n", keycode, al_keycode_to_name(keycode), event->keyboard.modifiers);
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
    log_debug("keyup %d\n", allegro_keycode);
    key[allegro_keycode] = false;
}

void key_up_event(ALLEGRO_EVENT *event)
{
    int keycode = event->keyboard.keycode;
    log_debug("keyboard: key up event, keycode=%d:%s, modifiers=%04X\n", keycode, al_keycode_to_name(keycode), event->keyboard.modifiers);
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
//            for (int act = 0; act < KEY_ACTION_MAX; act++) {
//                log_debug("keyboard: checking key action %d:%s codes %d<>%d, alt %d<>%d", act, keyact_const[act].name, keycode, keyactions[act].keycode, hostalt, keyactions[act].altstate);
//                if (keycode == keyactions[act].keycode && keyactions[act].altstate == hostalt) {
//                    keyact_const[act].upfunc();
//                    return;keylookup[keycode]
//                }
//            }
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
    log_debug("keyboard: key char event, keycode=%d:%s, unichar=%d\n", keycode, al_keycode_to_name(keycode), unichar);
//    if ((!event->keyboard.repeat || unichar != last_unichar[keycode]) && keycode < ALLEGRO_KEY_MAX) {
//        last_unichar[keycode] = unichar;
//        keycode = map_keypad_intern(keycode, unichar);
//        if (keycode == ALLEGRO_KEY_A && keyas && !keylogical)
//            keycode = ALLEGRO_KEY_CAPSLOCK;
//        else if (keycode == ALLEGRO_KEY_S && keyas && !keylogical)
//            keycode = ALLEGRO_KEY_LCTRL;
//        for (int act = 0; act < KEY_ACTION_MAX; act++) {
//            log_debug("keyboard: checking key action %d:%s codes %d<>%d, alt %d<>%d", act, keyact_const[act].name, keycode, keyactions[act].keycode, hostalt, keyactions[act].altstate);
//            if (keycode == keyactions[act].keycode && keyactions[act].altstate == hostalt) {
//                keyact_const[act].downfunc();
//                return;
//            }
//        }
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
    switch(event->type) 
    {
        // Keyboard handling.
        case ALLEGRO_EVENT_KEY_DOWN:
//            if (!keydefining)
                key_down_event(event);
            break;

        case ALLEGRO_EVENT_KEY_CHAR:
//            if (!keydefining)
                key_char_event(event);
            break;

        case ALLEGRO_EVENT_KEY_UP:
//           if (!keydefining)
                key_up_event(event);
            break;
    }
    return elkEvent;
}
