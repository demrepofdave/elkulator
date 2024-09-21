/*Elkulator v1.0 by Sarah Walker
  Linux keyboard redefinition GUI*/

#ifndef WIN32
#include <allegro.h>
#include "elk.h"

int keytemp[128];

/* Convert widget positions to or from screen positions. */

static void move_widgets(DIALOG *d, int show)
{
        int x = 0, adjustment;

        for (x = 0; d[x].proc; x++)
        {
                adjustment = (SCREEN_W/2) - (d[0].w/2);
                adjustment = show ? adjustment : -adjustment;
                d[x].x += adjustment;

                adjustment = (SCREEN_H/2) - (d[0].h/2);
                adjustment = show ? adjustment : -adjustment;
                d[x].y += adjustment;
        }
}

static BITMAP *open_dialog(DIALOG *d)
{
        BITMAP *b;
        int x, y;

        move_widgets(d, 1);
        x = (SCREEN_W/2) - (d[0].w/2);
        y = (SCREEN_H/2) - (d[0].h/2);
        b=create_bitmap(d[0].w, d[0].h);
        blit(screen, b, x, y, 0, 0, d[0].w, d[0].h);

        return b;
}

static void close_dialog(DIALOG *d, BITMAP *b)
{
        int x, y;

        x = (SCREEN_W/2) - (d[0].w/2);
        y = (SCREEN_H/2) - (d[0].h/2);
        blit(b, screen, 0, 0, x, y, d[0].w, d[0].h);
        move_widgets(d, 0);
        destroy_bitmap(b);
}

/* Constant colours. */

static const int FG = 0xffffff;
static const int BG = 0x000000;

char *key_names[] =
{
   "",           "A",          "B",          "C",
   "D",          "E",          "F",          "G",
   "H",          "I",          "J",          "K",
   "L",          "M",          "N",          "O",
   "P",          "Q",          "R",          "S",
   "T",          "U",          "V",          "W",
   "X",          "Y",          "Z",          "0",
   "1",          "2",          "3",          "4",
   "5",          "6",          "7",          "8",
   "9",          "0_PAD",      "1_PAD",      "2_PAD",
   "3_PAD",      "4_PAD",      "5_PAD",      "6_PAD",
   "7_PAD",      "8_PAD",      "9_PAD",      "F1",
   "F2",         "F3",         "F4",         "F5",
   "F6",         "F7",         "F8",         "F9",
   "F10",        "F11",        "F12",        "ESC",
   "TILDE",      "MINUS",      "EQUALS",     "BACKSPACE",
   "TAB",        "OPENBRACE",  "CLOSEBRACE", "ENTER",
   "COLON",      "QUOTE",      "BACKSLASH",  "BACKSLASH2",
   "COMMA",      "STOP",       "SLASH",      "SPACE",
   "INSERT",     "DEL",        "HOME",       "END",
   "PGUP",       "PGDN",       "LEFT",       "RIGHT",
   "UP",         "DOWN",       "SLASH_PAD",  "ASTERISK",
   "MINUS_PAD",  "PLUS_PAD",   "DEL_PAD",    "ENTER_PAD",
   "PRTSCR",     "PAUSE",      "ABNT_C1",    "YEN",
   "KANA",       "CONVERT",    "NOCONVERT",  "AT",
   "CIRCUMFLEX", "COLON2",     "KANJI",      "EQUALS_PAD",
   "BACKQUOTE",  "SEMICOLON",  "COMMAND",    "UNKNOWN1",
   "UNKNOWN2",   "UNKNOWN3",   "UNKNOWN4",   "UNKNOWN5",
   "UNKNOWN6",   "UNKNOWN7",   "UNKNOWN8",   "LSHIFT",
   "RSHIFT",     "LCONTROL",   "RCONTROL",   "ALT",
   "ALTGR",      "LWIN",       "RWIN",       "MENU",
   "SCRLOCK",    "NUMLOCK",    "CAPSLOCK",   "MAX"
};

/* Key reading control. */

#define MAX_KEYS 5

static int key_to_define;
static int current_keys[MAX_KEYS];
static char keysel[MAX_KEYS];
static int break_keys[MAX_KEYS];
static int menu_keys[MAX_KEYS];

static void populate_current_keys()
{
        int i, count;

        /* Clear current keys. */

        for (i = 0; i < MAX_KEYS; i++)
                current_keys[i] = -1;

        /* Populate with registered keys. */

        for (i = 0, count = 0; (i < 128) && (count < MAX_KEYS); i++)
        {
                if (keytemp[i] == key_to_define)
                {
                        current_keys[count] = i;
                        count++;
                }
        }
}

static void update_defined_keys()
{
        int i;

        /* Reset to defaults. */

        for (i = 0; i < 128; i++)
                if (keytemp[i] == key_to_define)
                        keytemp[i] = -1;

        /* Apply changes. */

        for (i = 0; (i < MAX_KEYS) && (current_keys[i] != -1); i++)
                keytemp[current_keys[i]] = key_to_define;
}

static char *get_current_keys(int index, int *list_size)
{
        int i;

        /* Return the number of defined keys. */

        if (index < 0)
        {
                for (i = 0; i < MAX_KEYS; i++)
                        if (current_keys[i] == -1)
                                break;

                *list_size = i;
                return NULL;
        }

        /* Otherwise, return the name of the indicated key. */

        else
                return key_names[current_keys[index]];
}

static int d_assign_key(int msg, DIALOG *d, int c)
{
        int ret = d_button_proc(msg, d, c);
        int i, code, assigned = 0, have_code;

        if (ret == D_EXIT)
        {
                do
                {
                        if (keyboard_needs_poll())
                                poll_keyboard();

                        /* Manually scan the keys to obtain the one being
                           pressed. This includes modifier keys that cannot be
                           easily detected otherwise. */

                        have_code = 0;

                        for (code = 0; code < 128; code++)
                        {
                                if (key[code])
                                {
                                        have_code = 1;

                                        if (assigned)
                                                break;

                                        /* Find a slot to record the key. */

                                        for (i = 0; (i < MAX_KEYS) &&
                                                    (current_keys[i] != -1) &&
                                                    (current_keys[i] != code); i++);

                                        /* Record any new key and update the
                                           display. */

                                        if ((i < MAX_KEYS) && (current_keys[i] != code))
                                        {
                                                current_keys[i] = code;

                                                if (d->dp2 != NULL)
                                                        object_message(d->dp2, MSG_DRAW, 0);
                                        }

                                        assigned = 1;
                                }
                        }

                        /* Exit only when no more keys are being pressed, also
                           clearing any keypresses which seems to be necessary
                           to deal with Escape. */

                        if (assigned && !have_code)
                        {
                                while (keypressed()) readkey();

                                return D_O_K;
                        }
                }
                while (1);
        }

        return ret;
}

static int d_remove_key(int msg, DIALOG *d, int c)
{
        int ret = d_button_proc(msg, d, c);
        int size, i, j;

        get_current_keys(-1, &size);

        if (ret == D_EXIT)
        {
                /* Traverse the current keys. */

                for (i = 0, j = i; i < size; i++)
                {
                        /* Obtain the next unselected key. */

                        for (; (j < size) && keysel[j]; j++);

                        /* Copy unselected keys to the start of the list. */

                        if (j < size)
                        {
                                if (i != j)
                                        current_keys[i] = current_keys[j];
                                j++;
                        }

                        /* Erase any selected keys at the end of the list. */

                        else
                                current_keys[i] = -1;
                }

                if (d->dp2 != NULL)
                        object_message(d->dp2, MSG_DRAW, 0);

                return D_O_K;
        }

        return ret;
}

/* Key definition dialogue. */

static char msg_redefine[] = "Redefining XXXXXXXXXXXX";
static char msg_assigned[] = "Assigned to keys:";
static char msg_assign[]   = "Assign new key";
static char msg_remove[]   = "Remove key";

DIALOG bemdefinekeygui[] =
{
        /* proc,        x,y,w,h,        fg,bg,  key, flags,   d1, d2, label,            dp2,  dp3 */

        {d_box_proc,    0,0,200,236,    FG,BG,  0,   0,       0,  0,  NULL,             NULL, NULL},

        {d_ctext_proc,  100,8,0,0,      FG,BG,  0,   0,       0,  0,  msg_redefine,     NULL, NULL},
        {d_ctext_proc,  100,24,0,0,     FG,BG,  0,   0,       0,  0,  msg_assigned,     NULL, NULL},
        {d_list_proc,   20,40,160,80,   FG,BG,  0,   0,       0,  0,  get_current_keys, keysel, NULL},
        {d_assign_key,  20,128,160,28,  FG,BG,  0,   D_EXIT,  0,  0,  msg_assign,       NULL, NULL},
        {d_remove_key,  20,164,160,28,  FG,BG,  0,   D_EXIT,  0,  0,  msg_remove,       NULL, NULL},

        {d_button_proc, 36,200,60,28,   FG,BG,  0,   D_CLOSE, 1,  0,  "OK",             NULL, NULL},
        {d_button_proc, 104,200,60,28,  FG,BG,  0,   D_CLOSE, 0,  0,  "Cancel",         NULL, NULL},

        {d_yield_proc},
        {0,             0,0,0,0,        0,0,    0,   0,       0,  0,  NULL,             NULL, NULL}
};

static int gui_keydefine_input(DIALOG *kd)
{
        DIALOG_PLAYER *dp;
        DIALOG *d=bemdefinekeygui;
        BITMAP *b;
        int i;

        key_to_define = kd->d2;

        /* Wire up the buttons to the list. */

        d[4].dp2 = &d[3];
        d[5].dp2 = &d[3];

        sprintf(msg_redefine, "Redefining %s", kd->dp2 != NULL ? (char *)kd->dp2 : (char *)kd->dp);

        /* Initialise the current key list. */

        populate_current_keys();

        b = open_dialog(d);
        dp = init_dialog(d, 0);

        while (update_dialog(dp) && !menu_pressed() && !(mouse_b&2) && !key[KEY_ESC]);

        /* Determine which button caused the dialogue to close. */

        i = shutdown_dialog(dp);

        /* Update the temporary mapping if OK was pressed. */

        if (d[i].d1)
                update_defined_keys();

        close_dialog(d, b);
        return D_O_K;
}

static int d_getkey(int msg, DIALOG *d, int cd)
{
        int ret = d_button_proc(msg, d, cd);

        if (ret == D_EXIT)
        {
                gui_keydefine_input(d);
                return D_O_K;
        }
        else
                return ret;
}

/* Keyboard dialogue with emulated key values represented by Allegro keycodes.
   Note that keycodes from actual keypresses are converted to these key values
   via the keylookup table. */

DIALOG bemdefinegui[] =
{
        /* proc,        x,y,w,h,        fg,bg,  key, flags,   rc, emulated key,   label,    alt label, dp3 */

        {d_box_proc,    0,0,538,270,    FG,BG,  0,   0,       0,  0,              NULL,     NULL,      NULL},

        {d_button_proc, 205,232,60,28,  FG,BG,  0,   D_CLOSE, 1,  0,              "OK",     NULL,      NULL},
        {d_button_proc, 271,232,60,28,  FG,BG,  0,   D_CLOSE, 0,  0,              "Cancel", NULL,      NULL},

        {d_getkey,      8,8,522,28,     FG,BG,  0,   D_EXIT,  0,  KEY_MENU,       "Menu",   NULL,      NULL},

        {d_getkey,      26,56,28,28,    FG,BG,  0,   D_EXIT,  0,  KEY_ESC,        "ESC",    NULL,      NULL},
        {d_getkey,      58,56,28,28,    FG,BG,  0,   D_EXIT,  0,  KEY_1,          "1",      NULL,      NULL},
        {d_getkey,      90,56,28,28,    FG,BG,  0,   D_EXIT,  0,  KEY_2,          "2",      NULL,      NULL},
        {d_getkey,      122,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_3,          "3",      NULL,      NULL},
        {d_getkey,      154,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_4,          "4",      NULL,      NULL},
        {d_getkey,      186,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_5,          "5",      NULL,      NULL},
        {d_getkey,      218,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_6,          "6",      NULL,      NULL},
        {d_getkey,      250,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_7,          "7",      NULL,      NULL},
        {d_getkey,      282,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_8,          "8",      NULL,      NULL},
        {d_getkey,      314,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_9,          "9",      NULL,      NULL},
        {d_getkey,      346,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_0,          "0",      NULL,      NULL},
        {d_getkey,      378,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_MINUS,      "=",      NULL,      NULL},
        {d_getkey,      410,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_LEFT,       "LFT",    "LEFT",    NULL},
        {d_getkey,      442,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_RIGHT,      "RGT",    "RIGHT",   NULL},
        {d_getkey,      474,56,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_F12,        "BRK",    "BREAK",   NULL},

        {d_getkey,      42,88,28,28,    FG,BG,  0,   D_EXIT,  0,  KEY_TAB,        "FN",     NULL,      NULL},
        {d_getkey,      74,88,28,28,    FG,BG,  0,   D_EXIT,  0,  KEY_Q,          "Q",      NULL,      NULL},
        {d_getkey,      106,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_W,          "W",      NULL,      NULL},
        {d_getkey,      138,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_E,          "E",      NULL,      NULL},
        {d_getkey,      170,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_R,          "R",      NULL,      NULL},
        {d_getkey,      202,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_T,          "T",      NULL,      NULL},
        {d_getkey,      234,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_Y,          "Y",      NULL,      NULL},
        {d_getkey,      266,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_U,          "U",      NULL,      NULL},
        {d_getkey,      298,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_I,          "I",      NULL,      NULL},
        {d_getkey,      330,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_O,          "O",      NULL,      NULL},
        {d_getkey,      362,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_P,          "P",      NULL,      NULL},
        {d_getkey,      394,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_UP,         "UP",     NULL,      NULL},
        {d_getkey,      426,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_DOWN,       "DWN",    "DOWN",    NULL},
        {d_getkey,      458,88,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_END,        "CPY",    "COPY",    NULL},

        {d_getkey,      50,120,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_LCONTROL,   "CTL",    "CTRL",    NULL},
        {d_getkey,      82,120,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_A,          "A",      NULL,      NULL},
        {d_getkey,      114,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_S,          "S",      NULL,      NULL},
        {d_getkey,      146,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_D,          "D",      NULL,      NULL},
        {d_getkey,      178,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_F,          "F",      NULL,      NULL},
        {d_getkey,      210,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_G,          "G",      NULL,      NULL},
        {d_getkey,      242,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_H,          "H",      NULL,      NULL},
        {d_getkey,      274,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_J,          "J",      NULL,      NULL},
        {d_getkey,      306,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_K,          "K",      NULL,      NULL},
        {d_getkey,      338,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_L,          "L",      NULL,      NULL},
        {d_getkey,      370,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_SEMICOLON,  ";",      NULL,      NULL},
        {d_getkey,      402,120,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_QUOTE,      ":",      NULL,      NULL},
        {d_getkey,      434,120,44,28,  FG,BG,  0,   D_EXIT,  0,  KEY_ENTER,      "RET",    "RETURN",  NULL},

        {d_getkey,      50,152,44,28,   FG,BG,  0,   D_EXIT,  0,  KEY_LSHIFT,     "SHIFT",  NULL,      NULL},
        {d_getkey,      98,152,28,28,   FG,BG,  0,   D_EXIT,  0,  KEY_Z,          "Z",      NULL,      NULL},
        {d_getkey,      130,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_X,          "X",      NULL,      NULL},
        {d_getkey,      162,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_C,          "C",      NULL,      NULL},
        {d_getkey,      194,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_V,          "V",      NULL,      NULL},
        {d_getkey,      226,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_B,          "B",      NULL,      NULL},
        {d_getkey,      258,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_N,          "N",      NULL,      NULL},
        {d_getkey,      290,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_M,          "M",      NULL,      NULL},
        {d_getkey,      322,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_COMMA,      ",",      NULL,      NULL},
        {d_getkey,      354,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_STOP,       ".",      NULL,      NULL},
        {d_getkey,      386,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_SLASH,      "/",      NULL,      NULL},
        {d_getkey,      418,152,44,28,  FG,BG,  0,   D_EXIT,  0,  KEY_RSHIFT,     "SHIFT",  NULL,      NULL},
        {d_getkey,      466,152,28,28,  FG,BG,  0,   D_EXIT,  0,  KEY_DEL,        "DEL",    "DELETE",  NULL},

        {d_getkey,      146,184,252,28, FG,BG,  0,   D_EXIT,  0,  KEY_SPACE,      "SPACE",  NULL,      NULL},

        {d_yield_proc},
        {0,             0,0,0,0,        0,0,    0,   0,       0,  0,              NULL,     NULL,      NULL}
};

int gui_keydefine()
{
        DIALOG_PLAYER *dp;
        DIALOG *d=bemdefinegui;
        BITMAP *b;
        int i;

        /* Initialise the temporary mapping. */

        for (i = 0; i < 128; i++) keytemp[i] = keylookup[i];

        b = open_dialog(d);
        dp = init_dialog(d, 0);

        while (update_dialog(dp) && !menu_pressed() && !(mouse_b&2) && !key[KEY_ESC]);

        /* Determine which button caused the dialogue to close. */

        i = shutdown_dialog(dp);

        /* Update the saved mapping if OK was pressed. */

        if (d[i].d1)
        {
                for (i = 0; i < 128; i++) keylookup[i] = keytemp[i];

                update_break_keys();
                update_menu_keys();
        }

        close_dialog(d, b);
        return D_O_K;
}

static void update_special_keys(int special_keys[MAX_KEYS], int keycode)
{
        int i, j = 0;

        for (i = 0; i < 128; i++)
        {
                if (keylookup[i] == keycode)
                        special_keys[j++] = i;
        }

        while (j < MAX_KEYS) special_keys[j++] = -1;
}

void update_break_keys()
{
        update_special_keys(break_keys, KEY_F12);
}

void update_menu_keys()
{
        update_special_keys(menu_keys, KEY_MENU);
}

static int special_key_pressed(int special_keys[MAX_KEYS])
{
        int i;

        for (i = 0; i < MAX_KEYS; i++)
        {
                if (special_keys[i] == -1)
                        break;
                else if (key[special_keys[i]])
                        return 1;
        }

        return 0;
}

int break_pressed()
{
        return special_key_pressed(break_keys);
}

int menu_pressed()
{
        return special_key_pressed(menu_keys);
}

#endif
