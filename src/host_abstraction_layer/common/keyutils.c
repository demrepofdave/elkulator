
#include <string.h>
#include "keyboard.h"
#include "logger.h"

// see https://planet.racket-lang.org/package-source/kazzmir/allegro.plt/1/6/allegro-4.2.0/examples/exkeys.c
static const int old_config_keys_to_new_host_keys_table[128] =
{
    HOST_KEY_NONE, // key_define_000
    HOST_KEY_A,  // key_define_001
    HOST_KEY_B,  // key_define_002
    HOST_KEY_C,  // key_define_003
    HOST_KEY_D,  // key_define_004
    HOST_KEY_E,  // key_define_005
    HOST_KEY_F,  // key_define_006
    HOST_KEY_G,  // key_define_007
    HOST_KEY_H,  // key_define_008
    HOST_KEY_I,  // key_define_009
    HOST_KEY_J,  // key_define_010
    HOST_KEY_K,  // key_define_011
    HOST_KEY_L,  // key_define_012
    HOST_KEY_M,  // key_define_013
    HOST_KEY_N,  // key_define_014
    HOST_KEY_O,  // key_define_015
    HOST_KEY_P,  // key_define_016
    HOST_KEY_Q,  // key_define_017
    HOST_KEY_R,  // key_define_018
    HOST_KEY_S,  // key_define_019
    HOST_KEY_T,  // key_define_020
    HOST_KEY_U,  // key_define_021
    HOST_KEY_V,  // key_define_022
    HOST_KEY_W,  // key_define_023
    HOST_KEY_X,  // key_define_024
    HOST_KEY_Y,  // key_define_025
    HOST_KEY_Z,  // key_define_026


    HOST_KEY_0,		// key_define_027,
    HOST_KEY_1,		// key_define_028,
    HOST_KEY_2,		// key_define_029,
    HOST_KEY_3,		// key_define_030,
    HOST_KEY_4,		// key_define_031,
    HOST_KEY_5,		// key_define_032,
    HOST_KEY_6,		// key_define_033,
    HOST_KEY_7,		// key_define_034,
    HOST_KEY_8,		// key_define_035,
    HOST_KEY_9,		// key_define_036,

    HOST_KEY_PAD_0,		// key_define_037,
    HOST_KEY_PAD_1,		// key_define_038,
    HOST_KEY_PAD_2,		// key_define_039,
    HOST_KEY_PAD_3,		// key_define_040,
    HOST_KEY_PAD_4,		// key_define_041,
    HOST_KEY_PAD_5,		// key_define_042,
    HOST_KEY_PAD_6,		// key_define_043,
    HOST_KEY_PAD_7,		// key_define_044,
    HOST_KEY_PAD_8,		// key_define_045,
    HOST_KEY_PAD_9,		// key_define0_46,

    HOST_KEY_F1,	    	// key_define_047,
    HOST_KEY_F2,		    // key_define_048,
    HOST_KEY_F3,		    // key_define_049,
    HOST_KEY_F4,	    	// key_define_050,
    HOST_KEY_F5,	    	// key_define_051,
    HOST_KEY_F6,	    	// key_define_052,
    HOST_KEY_F7,	    	// key_define_053,
    HOST_KEY_F8,	    	// key_define_054,
    HOST_KEY_F9,	    	// key_define_055,
    HOST_KEY_F10,	        // key_define_056,
    HOST_KEY_F11,	        // key_define_057,
    HOST_KEY_F12,	        // key_define_058,

    HOST_KEY_ESCAPE,	    // key_define_059,
    HOST_KEY_TILDE,	        // key_define_060,
    HOST_KEY_MINUS,		    // key_define_061,
    HOST_KEY_EQUALS,	    // key_define_062,
    HOST_KEY_BACKSPACE,	    // key_define_063,
    HOST_KEY_TAB,		    // key_define_064,
    HOST_KEY_OPENBRACE,	    // key_define_065,
    HOST_KEY_CLOSEBRACE,	// key_define_066,
    HOST_KEY_ENTER,		    // key_define_067,
    HOST_KEY_SEMICOLON,	    // key_define_068,
    HOST_KEY_APOSTROPHE,    // key_define_069,
    HOST_KEY_BACKSLASH,	    // key_define_070,
    HOST_KEY_BACKSLASH2,	// key_define_071, /* DirectInput calls this DIK_OEM_102: "< > | on UK/Germany keyboards" */
    HOST_KEY_COMMA,		    // key_define_072,
    HOST_KEY_FULLSTOP,	    // key_define_073,
    HOST_KEY_SLASH,		    // key_define_074,
    HOST_KEY_SPACE,		    // key_define_075,

    HOST_KEY_INSERT,	    // key_define_076,
    HOST_KEY_DELETE,	    // key_define_077,
    HOST_KEY_HOME,		    // key_define_078,
    HOST_KEY_END,		    // key_define_079,
    HOST_KEY_PAGEUP,		// key_define_080,
    HOST_KEY_PAGEDOWN,	    // key_define_081,
    HOST_KEY_LEFT,		    // key_define_082,
    HOST_KEY_RIGHT,		    // key_define_083,
    HOST_KEY_UP,		    // key_define_084,
    HOST_KEY_DOWN,		    // key_define_085,

    HOST_KEY_PAD_SLASH,	    // key_define_086
    HOST_KEY_PAD_ASTERISK,	// key_define_087
    HOST_KEY_PAD_MINUS,	    // key_define_088
    HOST_KEY_PAD_PLUS,	    // key_define_089
    HOST_KEY_PAD_DELETE,	// key_define_090
    HOST_KEY_PAD_ENTER,	    // key_define_091

    HOST_KEY_PRINT_SCREEN,	// key_define_092
    HOST_KEY_PAUSE,		    // key_define_093

    HOST_KEY_ABNT_C1,	    // key_define_094
    HOST_KEY_YEN,		    // key_define_095
    HOST_KEY_KANA,		    // key_define_096
    HOST_KEY_CONVERT,	    // key_define_097
    HOST_KEY_NOCONVERT,	    // key_define_098
    HOST_KEY_AT,		    // key_define_099
    HOST_KEY_CIRCUMFLEX,	// key_define_100
    HOST_KEY_COLON2,	    // key_define_101
    HOST_KEY_KANJI,		    // key_define_102

    HOST_KEY_PAD_EQUALS,	// key_define_103	MacOS X
    HOST_KEY_BACKQUOTE,	    // key_define_104	MacOS X
    HOST_KEY_SEMICOLON2,	// key_define_105	MacOS X
    HOST_KEY_COMMAND,	    // key_define_106	MacOS X

    HOST_KEY_NONE,      // key_define_107
    HOST_KEY_NONE,      // key_define_108
    HOST_KEY_NONE,      // key_define_109
    HOST_KEY_NONE,      // key_define_110
    HOST_KEY_NONE,      // key_define_111
    HOST_KEY_NONE,      // key_define_112
    HOST_KEY_NONE,      // key_define_113
    HOST_KEY_NONE,      // key_define_114

    HOST_KEY_LSHIFT,        // key_define_115
    HOST_KEY_RSHIFT,        // key_define_116
    HOST_KEY_LCTRL,         // key_define_117
    HOST_KEY_RCTRL,         // key_define_118
    HOST_KEY_ALT,           // key_define_119
    HOST_KEY_ALTGR,         // key_define_120

    HOST_KEY_LWIN,          // key_define_121
    HOST_KEY_RWIN,          // key_define_122
    HOST_KEY_MENU,          // key_define_123
    HOST_KEY_SCROLLLOCK,    // key_define_124
    HOST_KEY_NUMLOCK,       // key_define_125
    HOST_KEY_CAPSLOCK,      // key_define_126

    HOST_KEY_MAX,           // key_define_127,

};

host_key_t keyutils_get_hostkey_from_legacy_keyid(uint8_t old_key_id)
{
    host_key_t host_key = HOST_KEY_NONE;

    if(old_key_id < HOST_KEY_MAX)
    {
        host_key = old_config_keys_to_new_host_keys_table[old_key_id];
    }
    log_debug("old_key_id = %d, returns host_key %d", old_key_id, host_key);
    return(host_key);
}


