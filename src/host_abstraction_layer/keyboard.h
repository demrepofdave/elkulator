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
} elk_key_id;

void keyboard_makelayout();
uint8_t keyboard_read(uint16_t addr);
void keyboard_debug_dump();
void keyhandler_refresh_elkkeys();
bool keyhandler_elk_key_state(elk_key_id elk_key_code);

