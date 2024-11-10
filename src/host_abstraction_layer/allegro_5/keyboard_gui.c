#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <limits.h>
#include "keyboard_internal.h"
#include "logger.h"

typedef enum {
    COL_BLACK,
    COL_GREY,
    COL_RED,
    COL_GREEN
} key_col_t;

typedef struct {
    uint16_t x, y, w, h;
    key_col_t col;
    char cap[12];
    char name[12];
    uint8_t keycode;
} key_cap_t;

typedef struct {
    const key_cap_t *captab;
    const key_cap_t *capend;
    uint16_t  disp_x, disp_y;
} key_dlg_t;

typedef enum {
    ST_ELK_KEY,
    ST_PC_KEY,
    ST_DONE
} state_t;

#define ELK_NKEY 56

static const key_cap_t kcaps_elk[ELK_NKEY] = {
        /* x,y,w,h,        Colour,  key,  fullname, key code */

        { 26,56,28,28,    COL_GREY,  "ESC",    "Escape",      0x70},
        { 58,56,28,28,    COL_GREY,  "1",      "1",      0x30},
        { 90,56,28,28,    COL_GREY,  "2",      "2",      0x31},
        { 122,56,28,28,   COL_GREY,  "3",      "3",      0x11},
        { 154,56,28,28,   COL_GREY,  "4",      "4",      0x12},
        { 186,56,28,28,   COL_GREY,  "5",      "5",      0x13},
        { 218,56,28,28,   COL_GREY,  "6",      "6",      0x34},
        { 250,56,28,28,   COL_GREY,  "7",      "7",      0x24},
        { 282,56,28,28,   COL_GREY,  "8",      "8",      0x15},
        { 314,56,28,28,   COL_GREY,  "9",      "9",      0x26},
        { 346,56,28,28,   COL_GREY,  "0",      "0",      0x27},
        { 378,56,28,28,   COL_GREY,  "=",      "=",      0x17},
        { 410,56,28,28,   COL_GREY,  "LFT",    "Left",    0x19},
        { 442,56,28,28,   COL_GREY,  "RGT",    "Right",   0x79},
        { 474,56,28,28,   COL_GREY,  "BRK",    "Break",   0xff},  // 15 keys - top row.

        { 42,88,28,28,    COL_GREY,  "FN",     "Function",      0x60},
        { 74,88,28,28,    COL_GREY,  "Q",      "Q",      0x10},
        { 106,88,28,28,   COL_GREY,  "W",      "W",      0x21},
        { 138,88,28,28,   COL_GREY,  "E",      "E",      0x22},
        { 170,88,28,28,   COL_GREY,  "R",      "R",      0x33},
        { 202,88,28,28,   COL_GREY,  "T",      "T",      0x23},
        { 234,88,28,28,   COL_GREY,  "Y",      "Y",      0x44},
        { 266,88,28,28,   COL_GREY,  "U",      "U",      0x35},
        { 298,88,28,28,   COL_GREY,  "I",      "I",      0x25},
        { 330,88,28,28,   COL_GREY,  "O",      "O",      0x36},
        { 362,88,28,28,   COL_GREY,  "P",      "P",      0x37},
        { 394,88,28,28,   COL_GREY,  "UP",     "Up",      0x39},
        { 426,88,28,28,   COL_GREY,  "DWN",    "Down",    0x29},
        { 458,88,28,28,   COL_GREY,  "CPY",    "Copy",    0x69}, // TODO: Reposition - BBC position. - 14 keys 2nd row

        { 50,120,28,28,   COL_GREY,  "CTL",    "Ctrl",   0x01},
        { 82,120,28,28,   COL_GREY,  "A",      "A",      0x41},
        { 114,120,28,28,  COL_GREY,  "S",      "S",      0x51},
        { 146,120,28,28,  COL_GREY,  "D",      "D",      0x32},
        { 178,120,28,28,  COL_GREY,  "F",      "F",      0x43},
        { 210,120,28,28,  COL_GREY,  "G",      "G",      0x53},
        { 242,120,28,28,  COL_GREY,  "H",      "H",      0x54},
        { 274,120,28,28,  COL_GREY,  "J",      "J",      0x45},
        { 306,120,28,28,  COL_GREY,  "K",      "K",      0x46},
        { 338,120,28,28,  COL_GREY,  "L",      "L",      0x56},
        { 370,120,28,28,  COL_GREY,  ";",      ";",      0x57},
        { 402,120,28,28,  COL_GREY,  ":",      ":",      0x48},
        { 434,120,44,28,  COL_GREY,  "RET",    "Return",  0x49}, // 13 keys 3rd row

        { 50,152,44,28,   COL_GREY,  "SHIFT",  "Shift",  0x00},
        { 98,152,28,28,   COL_GREY,  "Z",      "Z",      0x61},
        { 130,152,28,28,  COL_GREY,  "X",      "X",      0x42},
        { 162,152,28,28,  COL_GREY,  "C",      "C",      0x52},
        { 194,152,28,28,  COL_GREY,  "V",      "V",      0x63},
        { 226,152,28,28,  COL_GREY,  "B",      "B",      0x64},
        { 258,152,28,28,  COL_GREY,  "N",      "N",      0x55},
        { 290,152,28,28,  COL_GREY,  "M",      "M",      0x65},
        { 322,152,28,28,  COL_GREY,  ",",      ",",      0x66},
        { 354,152,28,28,  COL_GREY,  ".",      ".",      0x67},
        { 386,152,28,28,  COL_GREY,  "/",      "/",      0x68},
        { 418,152,44,28,  COL_GREY,  "SHIFT",  "Shift",  0x00},
        { 466,152,28,28,  COL_GREY,  "DEL",    "Delete", 0x59}, // TODO: Reposition - BBC position - 13 keys 4th row

        { 146,184,252,28, COL_GREY,  "SPACE",  "Space",  0x62}, // 1 key bottom row.
};

static const key_dlg_t elk_kbd_dlg = { kcaps_elk, kcaps_elk + ELK_NKEY, 538, 304 };

#define BTNS_Y    266
#define BTNS_W     60
#define BTNS_H     28
#define BTN_OK_X  201
#define BTN_CAN_X 267

static ALLEGRO_FONT *font;
static ALLEGRO_EVENT_SOURCE uevsrc;

static void draw_button(int x, int y, int w, int h, ALLEGRO_COLOR bcol, ALLEGRO_COLOR tcol, const char *text)
{
    al_draw_filled_rectangle(x, y, x+w, y+h, bcol);
    al_draw_text(font, tcol, x+(w/2), y+(h/2)-2, ALLEGRO_ALIGN_CENTRE, text);
}

static void draw_keyboard(const key_dlg_t *key_dlg, int ok_x, int can_x)
{
    ALLEGRO_COLOR black, grey, white, red, brown, navy, green;
    const key_cap_t *kptr;

    black = al_map_rgb(  0,   0,   0);
    grey  = al_map_rgb(127, 127, 127);
    white = al_map_rgb(255, 255, 255);
    brown = al_map_rgb( 64,  32,  32);
    red   = al_map_rgb(255,  64,  64);
    navy  = al_map_rgb( 32,  32,  64);
    green = al_map_rgb(  0,  127,  0);
    al_clear_to_color(brown);
    for (kptr = key_dlg->captab; kptr < key_dlg->capend; kptr++) {
        switch(kptr->col) {
            case COL_RED:
                draw_button(kptr->x, kptr->y, kptr->w, kptr->h, red, white, kptr->cap);
                break;
            case COL_GREY:
                draw_button(kptr->x, kptr->y, kptr->w, kptr->h, grey, white, kptr->cap);
                break;
            case COL_BLACK:
                draw_button(kptr->x, kptr->y, kptr->w, kptr->h, black, white, kptr->cap);
                break;
            case COL_GREEN:
                draw_button(kptr->x, kptr->y, kptr->w, kptr->h, green, white, kptr->cap);
                break;
        }
    }
    draw_button(ok_x, BTNS_Y, BTNS_W, BTNS_H, navy, white, "OK");
    draw_button(can_x, BTNS_Y, BTNS_W, BTNS_H, navy, white, "Cancel");
    al_flip_display();
}

static void redef_message(const key_dlg_t *key_dlg, const key_cap_t *kptr, uint8_t *keylookcpy)
{
    int mid_x  = key_dlg->disp_x/2;
    int left_x = mid_x-200;
    int mid_y  = key_dlg->disp_y/2;
    int top_y  = mid_y - 36;
    ALLEGRO_COLOR navy = al_map_rgb( 32,  32,  64);
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    char s[1024], *p;
    int size, remain, count;

    log_debug("keydef-allegro: BBC key %s (%s), code %d clicked", kptr->cap, kptr->name, kptr->keycode);

    al_draw_filled_rectangle(left_x, top_y, left_x + 400, top_y + 72, navy);
    snprintf(s, sizeof s, "Redefining %s", kptr->name);
    al_draw_text(font, white, left_x+24, top_y+16, ALLEGRO_ALIGN_LEFT, s);
    size = snprintf(s, sizeof s, "Assigned to PC key(s):");
    p = s + size;
    remain = sizeof s - size;
    count = 0;
    int actcode = 0xff - kptr->keycode;

    for (int code = 0; remain > 0 && code < ALLEGRO_KEY_MAX; code++) {
        if (keylookcpy[code] == kptr->keycode) {
            const char *fmt = count == 0 ? " %s" : ", %s";
            size = snprintf(p, remain, fmt, al_keycode_to_name(code));
            p += size;
            remain -= size;
        }
    }
    al_draw_text(font, white, left_x+24, top_y+32, ALLEGRO_ALIGN_LEFT, s);
    al_draw_text(font, white, left_x+24, top_y+48, ALLEGRO_ALIGN_LEFT, "Please press new key...");
    al_flip_display();
}

static bool mouse_within(ALLEGRO_EVENT *event, int x, int y, int w, int h)
{
    return event->mouse.x >= x && event->mouse.x <= x+w && event->mouse.y >= y && event->mouse.y <= y+h;
}

static void *keydef_thread(ALLEGRO_THREAD *thread, void *tdata)
{
    const key_dlg_t *key_dlg;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_EVENT event;
    state_t state;
    uint8_t keylookcpy[ALLEGRO_KEY_MAX];
    const key_cap_t *kptr = NULL;
    int mid_x, ok_x, can_x;
    bool alt_down = false;

    if (!font) {
        al_init_font_addon();
        font = al_create_builtin_font();
        if (!font)
            return NULL;
    }

    key_dlg = &elk_kbd_dlg;
    if ((display = al_create_display(key_dlg->disp_x, key_dlg->disp_y))) {
        if ((queue = al_create_event_queue())) {
            al_init_user_event_source(&uevsrc);
            al_register_event_source(queue, &uevsrc);
            al_register_event_source(queue, al_get_display_event_source(display));
            al_register_event_source(queue, al_get_mouse_event_source());
            al_register_event_source(queue, al_get_keyboard_event_source());
            mid_x = key_dlg->disp_x/2;
            ok_x = mid_x-3-BTNS_W;
            can_x = mid_x+3;
            state = ST_ELK_KEY;
            memcpy(keylookcpy, keylookup, ALLEGRO_KEY_MAX);
            draw_keyboard(key_dlg, ok_x, can_x);
            while (state != ST_DONE) {
                al_wait_for_event(queue, &event);
                switch(event.type) {
                    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                        if (mouse_within(&event, ok_x, BTNS_Y, BTNS_W, BTNS_H)) {
                            // Ok button clicked.
                            memcpy(keylookup, keylookcpy, ALLEGRO_KEY_MAX);
                            state = ST_DONE;
                        }
                        else if (mouse_within(&event, can_x, BTNS_Y, BTNS_W, BTNS_H)) {
                            // Cancel button clicked.
                            if (state == ST_PC_KEY) {
                                state = ST_ELK_KEY;
                                draw_keyboard(key_dlg, ok_x, can_x);
                            }
                            else
                                state = ST_DONE;
                        }
                        else if (state == ST_ELK_KEY) {
                            // Search the keyboard buttons.
                            for (kptr = key_dlg->captab; kptr < key_dlg->capend; kptr++) {
                                if (mouse_within(&event, kptr->x, kptr->y, kptr->w, kptr->h)) {
                                    redef_message(key_dlg, kptr, keylookcpy);
                                    state = ST_PC_KEY;
                                    break;
                                }
                            }
                        }
                        break;
                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        memcpy(keylookup, keylookcpy, ALLEGRO_KEY_MAX);
                        state = ST_DONE;
                        break;
                    case ALLEGRO_EVENT_KEY_DOWN:
                        if (event.keyboard.keycode == ALLEGRO_KEY_ALT || event.keyboard.keycode == ALLEGRO_KEY_ALTGR) {
                            log_debug("keydef-allegro: alt down");
                            alt_down = true;
                        }
                        break;
                    case ALLEGRO_EVENT_KEY_CHAR:
                        if (state == ST_PC_KEY) {
                            int keycode = event.keyboard.keycode;
                            int actcode = kptr->keycode;
                            log_debug("keydef-allegro: mapping allegro code %d:%s to BBC code %02x", keycode, al_keycode_to_name(keycode), actcode);
                            keylookcpy[event.keyboard.keycode] = actcode;
                            state = ST_ELK_KEY;
                            draw_keyboard(key_dlg, ok_x, can_x);
                        }
                        break;
                    case ALLEGRO_EVENT_KEY_UP:
                        if (event.keyboard.keycode == ALLEGRO_KEY_ALT || event.keyboard.keycode == ALLEGRO_KEY_ALTGR) {
                            log_debug("keydef-allegro: alt up");
                            alt_down = false;
                        }
                }
            }
            al_destroy_event_queue(queue);
        } else
            log_debug("keydef-allegro: unable to create event queue");
        al_destroy_display(display);
    } else
        log_debug("keydef-allegro: unable to create display");
    keydefining = false;
    return NULL;
}

void gui_keydefine_open(void)
{
    ALLEGRO_THREAD *thread;

    if ((thread = al_create_thread(keydef_thread, NULL))) {
        keydefining = true;
        al_start_thread(thread);
    }
}

void gui_keydefine_close(void)
{
    ALLEGRO_EVENT event;

    event.type = ALLEGRO_EVENT_DISPLAY_CLOSE;
    al_emit_user_event(&uevsrc, &event, NULL);
}

int keydef_lookup_name(const char *name)
{
    const key_cap_t *ptr = kcaps_elk;
    const key_cap_t *end = ptr + ELK_NKEY;

    while (ptr < end) {
        if (strcasecmp(name, ptr->cap) == 0 || strcasecmp(name, ptr->name) == 0)
            return ptr->keycode;
        ptr++;
    }
    return 0;
}
