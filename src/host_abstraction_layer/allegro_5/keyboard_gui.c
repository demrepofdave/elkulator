#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <limits.h>
#include "keyboard_internal.h"
#include "host_abstraction_layer/keyboard.h"
#include "host_abstraction_layer/keyutils.h"
#include "logger.h"
#include "config_vars.h"

typedef struct {
    uint16_t x, y, w, h;
    char cap[12];
    elk_key_id_t elkkeyid;
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
        /* x,y,w,h,       key,     elk hal keycode */

        { 26,56,28,28,    "ESC",   ELK_KEY_ESCAPE    },
        { 58,56,28,28,    "1",     ELK_KEY_1         },
        { 90,56,28,28,    "2",     ELK_KEY_2         },
        { 122,56,28,28,   "3",     ELK_KEY_3         },
        { 154,56,28,28,   "4",     ELK_KEY_4         },
        { 186,56,28,28,   "5",     ELK_KEY_5         },
        { 218,56,28,28,   "6",     ELK_KEY_6         },
        { 250,56,28,28,   "7",     ELK_KEY_7         },
        { 282,56,28,28,   "8",     ELK_KEY_8         },
        { 314,56,28,28,   "9",     ELK_KEY_9         },
        { 346,56,28,28,   "0",     ELK_KEY_0         },
        { 378,56,28,28,   "=",     ELK_KEY_EQUALS    },
        { 410,56,28,28,   "LFT",   ELK_KEY_LEFT      },
        { 442,56,28,28,   "RGT",   ELK_KEY_RIGHT     },
        { 474,56,28,28,   "BRK",   ELK_KEY_BREAK     },  // 15 keys - top row.

        { 42,88,28,28,    "FN",    ELK_KEY_FUNCTION  },
        { 74,88,28,28,    "Q",     ELK_KEY_Q         },
        { 106,88,28,28,   "W",     ELK_KEY_W         },
        { 138,88,28,28,   "E",     ELK_KEY_E         },
        { 170,88,28,28,   "R",     ELK_KEY_R         },
        { 202,88,28,28,   "T",     ELK_KEY_T         },
        { 234,88,28,28,   "Y",     ELK_KEY_Y         },
        { 266,88,28,28,   "U",     ELK_KEY_U         },
        { 298,88,28,28,   "I",     ELK_KEY_I         },
        { 330,88,28,28,   "O",     ELK_KEY_O         },
        { 362,88,28,28,   "P",     ELK_KEY_P         },
        { 394,88,28,28,   "UP",    ELK_KEY_UP        },
        { 426,88,28,28,   "DWN",   ELK_KEY_DOWN      },
        { 458,88,28,28,   "CPY",   ELK_KEY_COPY      }, // TODO: Reposition - BBC position. - 14 keys 2nd row

        { 50,120,28,28,   "CTL",   ELK_KEY_CONTROL   },
        { 82,120,28,28,   "A",     ELK_KEY_A         },
        { 114,120,28,28,  "S",     ELK_KEY_S         },
        { 146,120,28,28,  "D",     ELK_KEY_D         },
        { 178,120,28,28,  "F",     ELK_KEY_F         },
        { 210,120,28,28,  "G",     ELK_KEY_G         },
        { 242,120,28,28,  "H",     ELK_KEY_H         },
        { 274,120,28,28,  "J",     ELK_KEY_J         },
        { 306,120,28,28,  "K",     ELK_KEY_K         },
        { 338,120,28,28,  "L",     ELK_KEY_L         },
        { 370,120,28,28,  ";",     ELK_KEY_SEMICOLON },
        { 402,120,28,28,  ":",     ELK_KEY_COLON     },
        { 434,120,44,28,  "RET",   ELK_KEY_RETURN    }, // 13 keys 3rd row

        { 50,152,44,28,   "SHIFT", ELK_KEY_SHIFT     },
        { 98,152,28,28,   "Z",     ELK_KEY_Z         },
        { 130,152,28,28,  "X",     ELK_KEY_X         },
        { 162,152,28,28,  "C",     ELK_KEY_C         },
        { 194,152,28,28,  "V",     ELK_KEY_V         },
        { 226,152,28,28,  "B",     ELK_KEY_B         },
        { 258,152,28,28,  "N",     ELK_KEY_N         },
        { 290,152,28,28,  "M",     ELK_KEY_M         },
        { 322,152,28,28,  ",",     ELK_KEY_COMMA     },
        { 354,152,28,28,  ".",     ELK_KEY_FULLSTOP  },
        { 386,152,28,28,  "/",     ELK_KEY_SLASH     },
        { 418,152,44,28,  "SHIFT", ELK_KEY_SHIFT     },
        { 466,152,28,28,  "DEL",   ELK_KEY_DEL       },

        { 146,184,252,28, "SPACE", ELK_KEY_SPACE     }, // 1 key bottom row.
};

static const key_dlg_t elk_kbd_dlg = { kcaps_elk, kcaps_elk + ELK_KEY_MAX, 538, 304 };

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
    ALLEGRO_COLOR grey, white, brown, navy;
    const key_cap_t *kptr;

    grey  = al_map_rgb(127, 127, 127);
    white = al_map_rgb(255, 255, 255);
    brown = al_map_rgb( 64,  32,  32);
    navy  = al_map_rgb( 32,  32,  64);
    al_clear_to_color(brown);
    for (kptr = key_dlg->captab; kptr < key_dlg->capend; kptr++) 
    {
        draw_button(kptr->x, kptr->y, kptr->w, kptr->h, grey, white, kptr->cap);
    }
    draw_button(ok_x, BTNS_Y, BTNS_W, BTNS_H, navy, white, "OK");
    draw_button(can_x, BTNS_Y, BTNS_W, BTNS_H, navy, white, "Cancel");
    al_flip_display();
}

static elk_key_id_t redef_message(const key_dlg_t *key_dlg, const key_cap_t *kptr, elk_key_id_t *keylookcpy)
{
    int mid_x  = key_dlg->disp_x/2;
    int left_x = mid_x-200;
    int mid_y  = key_dlg->disp_y/2;
    int top_y  = mid_y - 36;
    ALLEGRO_COLOR navy = al_map_rgb( 32,  32,  64);
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    char s[1024], *p;
    int size, remain, count;

    log_debug("keydef-allegro: Elk key %s clicked", keyutils_get_elkkey_longname(kptr->elkkeyid));

    al_draw_filled_rectangle(left_x, top_y, left_x + 400, top_y + 72, navy);
    snprintf(s, sizeof s, "Redefining %s", keyutils_get_elkkey_longname(kptr->elkkeyid));
    al_draw_text(font, white, left_x+24, top_y+16, ALLEGRO_ALIGN_LEFT, s);
    size = snprintf(s, sizeof s, "Assigned to PC key(s):");
    p = s + size;
    remain = sizeof s - size;
    count = 0;

    for(int code = 0; code < HOST_KEY_MAX; code++)
    {
        log_debug("%s=%s", keyutils_get_hostkey_longname(code), keyutils_get_elkkey_config_string(kptr->elkkeyid));
        if(keylookcpy[code] == kptr->elkkeyid)
        {
            const char *fmt = count == 0 ? " %s" : ", %s";
            log_debug("[[ %s=%s ]]", keyutils_get_hostkey_longname(code), keyutils_get_elkkey_config_string(kptr->elkkeyid));
            size = snprintf(p, remain, fmt, keyutils_get_hostkey_longname(code));
            p += size;
            remain -= size;
        }
    }

    al_draw_text(font, white, left_x+24, top_y+32, ALLEGRO_ALIGN_LEFT, s);
    al_draw_text(font, white, left_x+24, top_y+48, ALLEGRO_ALIGN_LEFT, "Please press new key, or click cancel to abort");
    al_flip_display();

    return (kptr->elkkeyid);
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
    elk_key_id_t keylookcpy[HOST_KEY_MAX];
    const key_cap_t *kptr = NULL;
    int mid_x, ok_x, can_x;
    bool alt_down = false;
    elk_key_id_t elkkeyid = ELK_KEY_MAX;

    keyboard_debug_dump();

    if (!font) {
        al_init_font_addon();
        font = al_create_builtin_font();
        if (!font)
            return NULL;
    }

    key_dlg = &elk_kbd_dlg;
    if ((display = al_create_display(key_dlg->disp_x, key_dlg->disp_y)))
    {
        if ((queue = al_create_event_queue()))
        {
            al_init_user_event_source(&uevsrc);
            al_register_event_source(queue, &uevsrc);
            al_register_event_source(queue, al_get_display_event_source(display));
            al_register_event_source(queue, al_get_mouse_event_source());
            al_register_event_source(queue, al_get_keyboard_event_source());
            mid_x = key_dlg->disp_x/2;
            ok_x = mid_x-3-BTNS_W;
            can_x = mid_x+3;
            state = ST_ELK_KEY;
            

            // Make a copy and modify that (just in case the user cancels the whole operation).
            memcpy(keylookcpy, elkConfig.keyboard.host_key_mapping, sizeof(keylookcpy));

            draw_keyboard(key_dlg, ok_x, can_x);
            while (state != ST_DONE)
            {
                al_wait_for_event(queue, &event);
                switch(event.type)
                {
                    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                        // Verify is mouse click is in our window.
                        if(event.mouse.display = display)
                        {
                            if (mouse_within(&event, ok_x, BTNS_Y, BTNS_W, BTNS_H)) 
                            {
                                // Ok button clicked (apply settings to main keyboard lookup table)
                                log_debug("ok");
                                memcpy(elkConfig.keyboard.host_key_mapping, keylookcpy, sizeof(elkConfig.keyboard.host_key_mapping));
                                state = ST_DONE;
                            }
                            else if (mouse_within(&event, can_x, BTNS_Y, BTNS_W, BTNS_H))
                            {
                                // Cancel button clicked.
                                if (state == ST_PC_KEY) 
                                {
                                    state = ST_ELK_KEY;
                                    draw_keyboard(key_dlg, ok_x, can_x);
                                }
                                else
                                {
                                    log_debug("Cancel");
                                    state = ST_DONE;
                                }
                            }
                            else if (state == ST_ELK_KEY) 
                            {
                                // Search the keyboard buttons.
                                for (kptr = key_dlg->captab; kptr < key_dlg->capend; kptr++) 
                                {
                                    if (mouse_within(&event, kptr->x, kptr->y, kptr->w, kptr->h))
                                    {
                                        elkkeyid = redef_message(key_dlg, kptr, keylookcpy);
                                        state = ST_PC_KEY;
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        memcpy(elkConfig.keyboard.host_key_mapping, keylookcpy, HOST_KEY_MAX);
                        state = ST_DONE;
                        break;
                    case ALLEGRO_EVENT_KEY_DOWN:
                        // Check if key was pressed in this window
                        if (event.keyboard.display == display)
                        {
                            if (event.keyboard.keycode == ALLEGRO_KEY_ALT || event.keyboard.keycode == ALLEGRO_KEY_ALTGR)
                            {
                                log_debug("keydef-allegro: alt down");
                                alt_down = true;
                            }
                        }
                        break;
                    case ALLEGRO_EVENT_KEY_CHAR:
                        // Check if key was pressed in this window
                        if (event.keyboard.display == display)
                        {
                            if (state == ST_PC_KEY) 
                            {
                                host_key_t hostkey = keyboard_allegro5_key_to_host_key(event.keyboard.keycode);
                                log_debug("keydef-allegro: mapping allegro code %d, hostkey %s to Elk key %s", event.keyboard.keycode, keyutils_get_hostkey_longname(hostkey), keyutils_get_elkkey_longname(elkkeyid));
                                keylookcpy[hostkey] = elkkeyid;
                                state = ST_ELK_KEY;
                                draw_keyboard(key_dlg, ok_x, can_x);
                            }
                        }
                        break;
                    case ALLEGRO_EVENT_KEY_UP:
                        // Check if key was pressed in this window
                        if (event.keyboard.display == display)
                        {
                            if (event.keyboard.keycode == ALLEGRO_KEY_ALT || event.keyboard.keycode == ALLEGRO_KEY_ALTGR) {
                                log_debug("keydef-allegro: alt up");
                                alt_down = false;
                            }
                        }
                        break;
                    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
                        if(event.display.source == display)
                        {
                            draw_keyboard(key_dlg, ok_x, can_x);
                            if (state == ST_PC_KEY)
                            {
                                // Redraw keyboard definition message as well.
                                redef_message(key_dlg, kptr, keylookcpy);
                            }
                        }
                        break;
                    default: 
                        if(event.type != ALLEGRO_EVENT_MOUSE_AXES)
                        {
                            log_debug("Allegro event %d", event.type);
                        }
                        break;
                }
            }
            al_destroy_event_queue(queue);
        } else
            log_debug("keydef-allegro: unable to create event queue");
        al_destroy_display(display);
    } else
        log_debug("keydef-allegro: unable to create display");
    keydefining = false;
    keyboard_debug_dump();
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

//int keydef_lookup_name(const char *name)
//{
//    const key_cap_t *ptr = kcaps_elk;
//    const key_cap_t *end = ptr + ELK_KEY_MAX;

//    while (ptr < end) {
//        if (strcasecmp(name, ptr->cap) == 0 || strcasecmp(name, ptr->name) == 0)
//            return ptr->keycode;
//        ptr++;
//    }
//    return 0;
//}
