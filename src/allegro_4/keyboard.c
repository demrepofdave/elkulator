#include "common/keyboard.h"
#include "elk.h"
#include <allegro.h>

int keys[2][14][4]=
{
        {
                {KEY_RIGHT,KEY_END,0,KEY_SPACE},
                {KEY_LEFT,KEY_DOWN,KEY_ENTER,KEY_DEL},
                {KEY_MINUS,KEY_UP,KEY_QUOTE,0},
                {KEY_0,KEY_P,KEY_COLON,KEY_SLASH},
                {KEY_9,KEY_O,KEY_L,KEY_STOP},
                {KEY_8,KEY_I,KEY_K,KEY_COMMA},
                {KEY_7,KEY_U,KEY_J,KEY_M},
                {KEY_6,KEY_Y,KEY_H,KEY_N},
                {KEY_5,KEY_T,KEY_G,KEY_B},
                {KEY_4,KEY_R,KEY_F,KEY_V},
                {KEY_3,KEY_E,KEY_D,KEY_C},
                {KEY_2,KEY_W,KEY_S,KEY_X},
                {KEY_1,KEY_Q,KEY_A,KEY_Z},
                {KEY_ESC,KEY_ALT,KEY_LCONTROL,KEY_LSHIFT}
        },
        {
                {KEY_RIGHT,KEY_END,0,KEY_SPACE},
                {KEY_LEFT,KEY_DOWN,KEY_ENTER,KEY_BACKSPACE},
                {KEY_MINUS,KEY_UP,KEY_QUOTE,0},
                {KEY_0,KEY_P,KEY_SEMICOLON,KEY_SLASH},
                {KEY_9,KEY_O,KEY_L,KEY_STOP},
                {KEY_8,KEY_I,KEY_K,KEY_COMMA},
                {KEY_7,KEY_U,KEY_J,KEY_M},
                {KEY_6,KEY_Y,KEY_H,KEY_N},
                {KEY_5,KEY_T,KEY_G,KEY_B},
                {KEY_4,KEY_R,KEY_F,KEY_V},
                {KEY_3,KEY_E,KEY_D,KEY_C},
                {KEY_2,KEY_W,KEY_S,KEY_X},
                {KEY_1,KEY_Q,KEY_A,KEY_Z},
                {KEY_ESC,KEY_TAB,KEY_RCONTROL,KEY_RSHIFT}
        }
};

int keyl[128];

int keylookup[128];

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

        /* Record the Break keys separately, along with configurable menu keys. */

        update_break_keys();
        update_menu_keys();
}

uint8_t keyboard_read(uint16_t addr)
{
        int d;
        uint8_t temp=0;
        for (d=0;d<128;d++)
        {
                if (key[d] && keyl[keylookup[d]]&0x80 && !(addr&(1<<(keyl[keylookup[d]]&15)))) temp|=1<<((keyl[keylookup[d]]&0x30)>>4);
        }

        return temp;
}