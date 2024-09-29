/*Elkulator v1.0 by Sarah Walker
  Linux keyboard redefinition GUI*/

#include <allegro5/allegro.h>
#include "elk.h"
#include "common/keyboard.h"

extern int keylookup[128];

int keytemp[128];

/* Convert widget positions to or from screen positions. */

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



/* Key definition dialogue. */

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
}

void update_menu_keys()
{
}


int break_pressed()
{
        return 0;
}

int menu_pressed()
{
        return 0;
}
