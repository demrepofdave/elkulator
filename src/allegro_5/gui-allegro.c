#include <allegro5/allegro_native_dialog.h>
#include "common/video.h"
#include "gui-allegro.h"
#include "logger.h"

/* pclose() and popen() on Windows are _pclose() and _popen() */
#ifdef _WIN32
#  define pclose _pclose
#  define popen _popen
#endif

typedef struct {
    const char *label;
    int itemno;
} menu_map_t;

// Private functions.

static inline int menu_id_num(menu_id_t id, int num)
{
    return (num << 8) | id;
}

static inline menu_id_t menu_get_id(ALLEGRO_EVENT *event)
{
    return event->user.data1 & 0xff;
}

static inline int menu_get_num(ALLEGRO_EVENT *event)
{
    return event->user.data1 >> 8;
}

static void add_checkbox_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, bool checked)
{
    int flags = ALLEGRO_MENU_ITEM_CHECKBOX;
    if (checked)
        flags |= ALLEGRO_MENU_ITEM_CHECKED;
    al_append_menu_item(parent, title, id, flags, NULL, NULL);
}

static void add_radio_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, int this_value, int cur_value)
{
    add_checkbox_item(parent, title, menu_id_num(id, this_value), this_value == cur_value);
}

static void add_radio_set(ALLEGRO_MENU *parent, char const **labels, uint16_t id, int cur_value)
{
    int i;
    const char *label;

    for (i = 0; (label = *labels++); i++)
        add_checkbox_item(parent, label, menu_id_num(id, i), i == cur_value);
}

static int menu_cmp(const void *va, const void *vb)
{
    menu_map_t *a = (menu_map_t *)va;
    menu_map_t *b = (menu_map_t *)vb;
    return strcasecmp(a->label, b->label);
}

static void add_sorted_set(ALLEGRO_MENU *parent, menu_map_t *map, size_t items, uint16_t id, int cur_value)
{
    int i, ino;

    qsort(map, items, sizeof(menu_map_t), menu_cmp);
    for (i = 0; i < items; i++) {
        ino = map[i].itemno;
        add_checkbox_item(parent, map[i].label, menu_id_num(id, ino), ino == cur_value);
    }
}

static int radio_event_simple(ALLEGRO_EVENT *event, int current)
{
    int id = menu_get_id(event);
    int num = menu_get_num(event);
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);

    al_set_menu_item_flags(menu, menu_id_num(id, current), ALLEGRO_MENU_ITEM_CHECKBOX);
    return num;
}

static int radio_event_with_deselect(ALLEGRO_EVENT *event, int current)
{
    int id = menu_get_id(event);
    int num = menu_get_num(event);
    ALLEGRO_MENU *menu = (ALLEGRO_MENU *)(event->user.data3);

    if (num == current)
        num = -1;
    else
        al_set_menu_item_flags(menu, menu_id_num(id, current), ALLEGRO_MENU_ITEM_CHECKBOX);
    return num;
}

/*MENU filemenu[6]=
{
        {"&Return",gui_return,NULL,0,NULL},
        {"&Hard reset",gui_reset,NULL,0,NULL},
        {"&Load state",gui_loads,NULL,0,NULL},
        {"&Save state",gui_saves,NULL,0,NULL},
        {"&Exit",gui_exit,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};*/
static ALLEGRO_MENU *create_file_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    al_append_menu_item(menu, "Hard Reset", IDM_FILE_HARD_RESET, 0, NULL, NULL);
//    al_append_menu_item(menu, "Load state...", IDM_FILE_LOAD_STATE, 0, NULL, NULL);
//    al_append_menu_item(menu, "Save State...", IDM_FILE_SAVE_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Exit", IDM_FILE_EXIT, 0, NULL, NULL);
    return menu;
}

// Public functions.

void gui_allegro_init(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display)
{
    log_debug("gui_allegro_init\n");
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "File", 0, 0, NULL, create_file_menu());
    al_set_display_menu(display, menu);
    al_register_event_source(queue, al_get_default_menu_event_source());
}

void gui_allegro_destroy(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display)
{
    al_unregister_event_source(queue, al_get_default_menu_event_source());
    al_set_display_menu(display, NULL);
}

static const char all_dext[] = "*.ssd;*.dsd;*.img;*.adf;*.ads;*.adm;*.adl;*.sdd;*.ddd;*.fdi;*.imd;*.hfe;"
                               "*.SSD;*.DSD;*.IMG;*.ADF;*.ADS;*.ADM;*.ADL;*.SDD;*.DDD;*.FDI;*.IMD;*.HFE";

uint32_t gui_allegro_event(ALLEGRO_EVENT *event)
{
    uint32_t elkEvent = 0;
    switch(menu_get_id(event)) 
    {
        case IDM_ZERO:
            break;
        case IDM_FILE_EXIT:
            elkEvent = ELK_EVENT_EXIT;
            break;
        case IDM_FILE_HARD_RESET:
            elkEvent = ELK_EVENT_RESET;
            break;
    }
    log_debug("gui_allegro_event elkEvent = 0x%02x\n", elkEvent);
    return(elkEvent);
}

void entergui()
{
}

void setejecttext(int d, char *s)
{
}
void setquit()
{
}