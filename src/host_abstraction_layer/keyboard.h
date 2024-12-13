#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include <stdint.h>
#include <stdbool.h>

// There are 56 keys in the Elk keyboard, here we defined them in
// order as laid out on the electron from top to bottom, left to right
typedef enum {
    ELK_KEY_NONE,
    ELK_KEY_0,
    ELK_KEY_1,
    ELK_KEY_2,
    ELK_KEY_3,
    ELK_KEY_4,
    ELK_KEY_5,
    ELK_KEY_6,
    ELK_KEY_7,
    ELK_KEY_8,
    ELK_KEY_9,
    ELK_KEY_A,
    ELK_KEY_B,
    ELK_KEY_C,
    ELK_KEY_D,
    ELK_KEY_E,
    ELK_KEY_F,
    ELK_KEY_G,
    ELK_KEY_H,
    ELK_KEY_I,
    ELK_KEY_J,
    ELK_KEY_K,
    ELK_KEY_L,
    ELK_KEY_M,
    ELK_KEY_N,
    ELK_KEY_O,
    ELK_KEY_P,
    ELK_KEY_Q,
    ELK_KEY_R,
    ELK_KEY_S,
    ELK_KEY_T,
    ELK_KEY_U,
    ELK_KEY_V,
    ELK_KEY_W,
    ELK_KEY_X,
    ELK_KEY_Y,
    ELK_KEY_Z,
    ELK_KEY_EQUALS,
    ELK_KEY_COMMA,
    ELK_KEY_FULLSTOP,
    ELK_KEY_SLASH,
    ELK_KEY_SEMICOLON,
    ELK_KEY_COLON,
    ELK_KEY_LEFT,
    ELK_KEY_RIGHT,
    ELK_KEY_UP,
    ELK_KEY_DOWN,
    ELK_KEY_FUNCTION,
    ELK_KEY_COPY,
    ELK_KEY_CONTROL,
    ELK_KEY_SHIFT,
    ELK_KEY_DEL,
    ELK_KEY_SPACE,
    ELK_KEY_RETURN,
    ELK_KEY_ESCAPE,
    ELK_KEY_BREAK,
    ELK_SPECIAL_KEY_MENU,  // NOTE: This is not a actual elektron key, but provides a way for the
                           //       elkulator menu key to be utilized (for libraries such as allegro 4 
                           //       that do not support native menus on their windows).
    ELK_KEY_MAX
} elk_key_id_t;

// HOST_KEY order is as per allegro4 key order as this
// simplifies the code to update elk.cfg key definition
// from the old key_define_999=999 to the new more human
// readable format (e.g. HOST_KEY_F12=ELK_KEY_BREAK).
typedef enum {
  
    HOST_KEY_NONE,
    HOST_KEY_A,
    HOST_KEY_B,
    HOST_KEY_C,
    HOST_KEY_D,
    HOST_KEY_E,
    HOST_KEY_F,
    HOST_KEY_G,
    HOST_KEY_H,
    HOST_KEY_I,
    HOST_KEY_J,
    HOST_KEY_K,
    HOST_KEY_L,
    HOST_KEY_M,
    HOST_KEY_N,
    HOST_KEY_O,
    HOST_KEY_P,
    HOST_KEY_Q,
    HOST_KEY_R,
    HOST_KEY_S,
    HOST_KEY_T,
    HOST_KEY_U,
    HOST_KEY_V,
    HOST_KEY_W,
    HOST_KEY_X,
    HOST_KEY_Y,
    HOST_KEY_Z,

    HOST_KEY_0,
    HOST_KEY_1,
    HOST_KEY_2,
    HOST_KEY_3,
    HOST_KEY_4,
    HOST_KEY_5,
    HOST_KEY_6,
    HOST_KEY_7,
    HOST_KEY_8,
    HOST_KEY_9,

    HOST_KEY_PAD_0,
    HOST_KEY_PAD_1,
    HOST_KEY_PAD_2,
    HOST_KEY_PAD_3,
    HOST_KEY_PAD_4,
    HOST_KEY_PAD_5,
    HOST_KEY_PAD_6,
    HOST_KEY_PAD_7,
    HOST_KEY_PAD_8,
    HOST_KEY_PAD_9,

    HOST_KEY_F1,
    HOST_KEY_F2,
    HOST_KEY_F3,
    HOST_KEY_F4,
    HOST_KEY_F5,
    HOST_KEY_F6,
    HOST_KEY_F7,
    HOST_KEY_F8,
    HOST_KEY_F9,
    HOST_KEY_F10,
    HOST_KEY_F11,
    HOST_KEY_F12,

    HOST_KEY_ESCAPE,
    HOST_KEY_TILDE,
    HOST_KEY_MINUS,
    HOST_KEY_EQUALS,
    HOST_KEY_BACKSPACE,
    HOST_KEY_TAB,
    HOST_KEY_OPENBRACE,
    HOST_KEY_CLOSEBRACE,
    HOST_KEY_ENTER,
    HOST_KEY_SEMICOLON,
    HOST_KEY_APOSTROPHE,
    HOST_KEY_BACKSLASH,
    HOST_KEY_BACKSLASH2,
    HOST_KEY_COMMA,
    HOST_KEY_FULLSTOP,
    HOST_KEY_SLASH,
    HOST_KEY_SPACE,
    HOST_KEY_INSERT,
    HOST_KEY_DELETE,
    HOST_KEY_HOME,
    HOST_KEY_END,
    HOST_KEY_PAGEUP,
    HOST_KEY_PAGEDOWN,

    HOST_KEY_LEFT,
    HOST_KEY_RIGHT,
    HOST_KEY_UP,
    HOST_KEY_DOWN, 
    
    HOST_KEY_PAD_SLASH,
    HOST_KEY_PAD_ASTERISK,
    HOST_KEY_PAD_MINUS,
    HOST_KEY_PAD_PLUS,
    HOST_KEY_PAD_DELETE,
    HOST_KEY_PAD_ENTER,

    HOST_KEY_PRINT_SCREEN,
    HOST_KEY_PAUSE,

    HOST_KEY_ABNT_C1,
    HOST_KEY_YEN,
    HOST_KEY_KANA,
    HOST_KEY_CONVERT,
    HOST_KEY_NOCONVERT,
    HOST_KEY_AT,
    HOST_KEY_CIRCUMFLEX,
    HOST_KEY_COLON2,
    HOST_KEY_KANJI,
    HOST_KEY_PAD_EQUALS,
    HOST_KEY_BACKQUOTE,
    HOST_KEY_SEMICOLON2,
    HOST_KEY_COMMAND,

    HOST_KEY_LSHIFT,
    HOST_KEY_RSHIFT,
    HOST_KEY_LCTRL,
    HOST_KEY_RCTRL,
    HOST_KEY_ALT,
    HOST_KEY_ALTGR,

    HOST_KEY_LWIN,
    HOST_KEY_RWIN,
    HOST_KEY_MENU,
    HOST_KEY_SCROLLLOCK,
    HOST_KEY_NUMLOCK,
    HOST_KEY_CAPSLOCK,
    HOST_KEY_MAX

} host_key_t;

void keyboard_makelayout();
uint8_t keyboard_read(uint16_t addr);
void keyboard_debug_dump();
void keyhandler_refresh_elkkeys();
bool keyhandler_elk_key_state(elk_key_id_t elk_key_code);
elk_key_id_t get_elk_key_from_host_key(host_key_t host_key);

host_key_t keyboard_get_host_key_from_old_config_id(uint8_t old_key_id);
const char * keyboard_hostkey_to_config_str(host_key_t host_key);
char * keyboard_elkkey_to_config_str(elk_key_id_t elk_key);

elk_key_id_t keyboard_str_to_elk_key_id(const char * host_config_str);

#endif // _KEYBOARD_H