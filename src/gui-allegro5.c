/*Elkulator v1.0 by Sarah Walker
  Linux GUI*/

#ifndef WIN32
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "hal/hal_allegro_5/gui-allegro.h"
#include "hal/hal.h"
#include "elk.h"

#undef printf

static ALLEGRO_MENU *disc_menu;
static ALLEGRO_MENU *rom_menu;


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

void setejecttext(int d, char *s)
{
}
void setquit()
{
}

extern int fullscreen;
extern int quited;
int windx=640,windy=512;
extern int dcol;
extern int ddtype,ddvol,sndddnoise;

//MENU filemenu[6];
//MENU discmenu[8];
//MENU rommenu[5];
//MENU tapespdmenu[4];
//MENU tapemenu[5];
//MENU displaymenu[7];
//MENU bordersmenu[4];
//MENU videomenu[3];
//MENU soundmenu[7];
//MENU keymenu[2];
//MENU dischmenu[4];
//MENU memmenu[6];
//MENU mrbmenu[4];
//MENU ulamenu[4];
//MENU settingsmenu[7];
//MENU miscmenu[5];
//MENU mainmenu[7];
//MENU joymenu[3];
//MENU ddtypemenu[3];
//MENU ddvolmenu[4];
//MENU romcartsmenu[3];

// Helper functions (private)

static void add_checkbox_item(ALLEGRO_MENU *parent, char const *title, uint16_t id, bool checked)
{
    int flags = ALLEGRO_MENU_ITEM_CHECKBOX;
    if (checked)
        flags |= ALLEGRO_MENU_ITEM_CHECKED;
    al_append_menu_item(parent, title, id, flags, NULL, NULL);
}

int gui_keydefine();

//MENU filemenu[6]=
//{
//        {"&Return",gui_return,NULL,0,NULL},
//        {"&Hard reset",gui_reset,NULL,0,NULL},
//        {"&Load state",gui_loads,NULL,0,NULL},
//        {"&Save state",gui_saves,NULL,0,NULL},
//        {"&Exit",gui_exit,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_file_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Hard Reset", IDM_FILE_HARD_RESET, 0, NULL, NULL);
    al_append_menu_item(menu, "Soft Reset", IDM_FILE_SOFT_RESET, 0, NULL, NULL);
    al_append_menu_item(menu, "Load state...", IDM_FILE_LOAD_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Save State...", IDM_FILE_SAVE_STATE, 0, NULL, NULL);
    al_append_menu_item(menu, "Exit", IDM_FILE_EXIT, 0, NULL, NULL);
    return menu;
}


//MENU tapespdmenu[4]=
//{
//        {"Normal",gui_normal,NULL,0,NULL},
//        {"Fast",gui_fast,NULL,0,NULL},
//        {"Really Fast",gui_rfast,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_tapespeed_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Normal",      menu_id_num(IDM_TAPE_SPEED, 0), !tapespeed);
    add_checkbox_item(menu, "Fast",        menu_id_num(IDM_TAPE_SPEED, 1), tapespeed == 1);
    add_checkbox_item(menu, "Really Fast", menu_id_num(IDM_TAPE_SPEED, 2), tapespeed == 2);
    return menu;
}

//MENU tapemenu[]=
//{
//        {"Load tape...",gui_loadt,NULL,0,NULL},
//        {"Rewind tape",gui_rewind,NULL,0,NULL},
//        {"Eject tape",gui_ejectt,NULL,0,NULL},
//        {"Tape speed",NULL,tapespdmenu,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_tape_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Load tape",   IDM_TAPE_LOAD, 0, NULL, NULL);
    al_append_menu_item(menu, "Rewind tape", IDM_TAPE_REWIND, 0, NULL, NULL);
    al_append_menu_item(menu, "Eject tape",  IDM_TAPE_EJECT, 0, NULL, NULL);
    al_append_menu_item(menu, "Tape speed", 0, 0, NULL, create_tapespeed_menu());
    return menu;
}

//MENU discmenu[8]=
//{
//       {"Load disc :&0/2...",gui_load0,NULL,0,NULL},
//       {"Load disc :&1/3...",gui_load1,NULL,0,NULL},
//       {"Eject disc :0/2",gui_eject0,NULL,0,NULL},
//       {"Eject disc :1/3",gui_eject1,NULL,0,NULL},
//       {"Write protect disc :0/2",gui_wprot0,NULL,0,NULL},
//       {"Write protect disc :1/3",gui_wprot1,NULL,0,NULL},
//       {"Default write protect",gui_wprotd,NULL,0,NULL},
//       {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_disc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();

    //al_append_menu_item(menu, "Autoboot disc in 0/2...", IDM_DISC_AUTOBOOT, 0, NULL, NULL);
    al_append_menu_item(menu, "Load disc :0/2...", menu_id_num(IDM_DISC_LOAD, 0), 0, NULL, NULL);
    al_append_menu_item(menu, "Load disc :1/3...", menu_id_num(IDM_DISC_LOAD, 1), 0, NULL, NULL);
    al_append_menu_item(menu, "Eject disc :0/2", menu_id_num(IDM_DISC_EJECT, 0), 0, NULL, NULL);
    al_append_menu_item(menu, "Eject disc :1/3", menu_id_num(IDM_DISC_EJECT, 1), 0, NULL, NULL);
    add_checkbox_item(menu, "Write protect disc :0/2", menu_id_num(IDM_DISC_WPROT, 0), writeprot[0]);
    add_checkbox_item(menu, "Write protect disc :1/3", menu_id_num(IDM_DISC_WPROT, 1), writeprot[1]);
    add_checkbox_item(menu, "Default write protect", IDM_DISC_WPROT_D, defaultwriteprot);
    disc_menu = menu;
    return menu;
}

//MENU romcartsmenu[3]=
//{
//        {"Mega Games Cartridge",gui_mgc,0,0,NULL},
//        {"David's Flash ROM Cartridge",gui_db_flash_cartridge,0,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
//
static ALLEGRO_MENU *create_romcarts_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Mega Games Cartridge",        menu_id_num(IDM_ROMCARTS_MEGA, 0),      enable_mgc);
    add_checkbox_item(menu, "David's Flash ROM Cartridge", menu_id_num(IDM_ROMCARTS_FLASH_ROM, 1), enable_db_flash_cartridge);
    return menu;
}

//MENU rommenu[5]=
//{
//        {"Load ROM cartridge &1...",gui_romload0,NULL,0,NULL},
//        {"Load ROM cartridge &2...",gui_romload1,NULL,0,NULL},
//        {"&Unload ROM cartridges...",gui_romeject0,NULL,0,NULL},
//        {"Multi-ROM expansions",NULL,romcartsmenu,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_rom_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Load ROM cartridge 1...", menu_id_num(IDM_ROM_LOAD_CART, 0), 0, NULL, NULL);
    al_append_menu_item(menu, "Load ROM cartridge 2...", menu_id_num(IDM_ROM_LOAD_CART, 1), 0, NULL, NULL);
    al_append_menu_item(menu, "Unload ROM cartridges...", IDM_ROM_UNLOAD_CARTS, 0, NULL, NULL);
    al_append_menu_item(menu, "Multi-ROM-expansions", 0, 0, NULL, create_romcarts_menu());
    return menu;
}


/******************************************************************************/
/* Settings menu and all sub-menus                                            */
/******************************************************************************/

//MENU displaymenu[7]=
//{
//        {"Scanlines",gui_disp,NULL,0,(void *)0},
//        {"Line doubling",gui_disp,NULL,0,(void *)1},
//        {"2xSaI",gui_disp,NULL,0,(void *)2},
//        {"Scale2X",gui_disp,NULL,0,(void *)3},
//        {"Super Eagle",gui_disp,NULL,0,(void *)4},
//        {"PAL Filter",gui_disp,NULL,0,(void *)5},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_display_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Scanlines",     menu_id_num(IDM_DISPLAY_FILTER, 0), drawmode == 0);
    add_checkbox_item(menu, "Line doubling", menu_id_num(IDM_DISPLAY_FILTER, 0), drawmode == 1);
    add_checkbox_item(menu, "2xSaI",         menu_id_num(IDM_DISPLAY_FILTER, 0), drawmode == 2);
    add_checkbox_item(menu, "Scale2X",       menu_id_num(IDM_DISPLAY_FILTER, 0), drawmode == 3);
    add_checkbox_item(menu, "Super Eagle",   menu_id_num(IDM_DISPLAY_FILTER, 0), drawmode == 4);
    add_checkbox_item(menu, "PAL Filter",    menu_id_num(IDM_DISPLAY_FILTER, 0), drawmode == 5);
    return menu;
}


//MENU videomenu[3]=
//{
//        {"Display type",NULL,displaymenu,0,NULL},
//        {"Fullscreen",gui_fullscreen,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_video_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Display Type", 0, 0, NULL, create_display_menu());
    add_checkbox_item(menu, "Fullscreen", IDM_SETTINGS_FULLSCREEN, fullscreen); // TODO: fullscreen?
    return menu;
}

//MENU ddtypemenu[3]=
//{
//        {"5.25",gui_ddtype,NULL,0,(void *)0},
//        {"3.5",gui_ddtype,NULL,0,(void *)1},
//        {NULL,NULL,NULL,0,NULL}
//};
//
static ALLEGRO_MENU *create_ddtype_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "5.25", menu_id_num(IDM_DDTYPE_SIZE, 0), !ddtype);
    add_checkbox_item(menu, "3.5",  menu_id_num(IDM_DDTYPE_SIZE, 1),  ddtype);
    return menu;
}

//MENU ddvolmenu[4]=
//{
//        {"33%",gui_ddvol,NULL,0,(void *)1},
//        {"66%",gui_ddvol,NULL,0,(void *)2},
//        {"100%",gui_ddvol,NULL,0,(void *)3},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_ddvol_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "33%",  menu_id_num(IDM_DDVOL_LEVEL, 0), ddvol == 0);
    add_checkbox_item(menu, "66%",  menu_id_num(IDM_DDVOL_LEVEL, 1), ddvol == 1);
    add_checkbox_item(menu, "100%", menu_id_num(IDM_DDVOL_LEVEL, 2), ddvol == 2);
    return menu;
}

//MENU soundmenu[7]=
//{
//        {"Internal speaker",gui_internalsnd,NULL,0,NULL},
//        {"CSS Sound Expansion",gui_sndex,NULL,0,NULL},
//        {"Disc drive noise",gui_ddnoise,NULL,0,NULL},
//        {"Tape noise",gui_tnoise,NULL,0,NULL},
//        {"Disc drive type",NULL,ddtypemenu,0,NULL},
//        {"Disc drive volume",NULL,ddvolmenu,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_sound_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Internal speaker",    IDM_SOUND_INTERNAL_SPEAKER, sndint);
    add_checkbox_item(menu, "CSS Sound Expansion", IDM_SOUND_CSS_SOUND_EXPANSION, sndex);
    add_checkbox_item(menu, "Disc drive noise",    IDM_SOUND_DD_NOISE, sndddnoise);
    add_checkbox_item(menu, "Tape noise",          IDM_SOUND_TAPE_NOISE, sndtape);
    al_append_menu_item(menu, "Disc drive type", 0, 0, NULL, create_ddtype_menu());
    al_append_menu_item(menu, "Disc drive volume", 0, 0, NULL, create_ddvol_menu());
    return menu;
}


//MENU mrbmenu[4]=
//{
//        {"&Off",gui_mrbmode,NULL,0,(void *)0},
//        {"&Turbo",gui_mrbmode,NULL,0,(void *)1},
//        {"&Shadow",gui_mrbmode,NULL,0,(void *)2},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_mrb_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Off",    menu_id_num(IDM_MRB_MODE, 0),  mrbmode == 0);
    add_checkbox_item(menu, "Turbo",  menu_id_num(IDM_MRB_MODE, 1),  mrbmode == 1);
    add_checkbox_item(menu, "Shadow", menu_id_num(IDM_MRB_MODE, 2), mrbmode == 2);
    return menu;
}

//MENU ulamenu[4]=
//{
//        {"&Standard",gui_ulamode,NULL,0,(void *)0},
//        {"&Enhanced (8-bit, dual access)",gui_ulamode,NULL,0,(void *)ULA_RAM_8BIT_DUAL_ACCESS},
//        {"&Enhanced (8-bit, single access)",gui_ulamode,NULL,0,(void *)ULA_RAM_8BIT_SINGLE_ACCESS},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_ula_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu,   "Standard",                        menu_id_num(IDM_ULA_MODE, 0), ulamode == 0);
    add_checkbox_item(menu,   "Enhanced (8-bit, dual access)",   menu_id_num(IDM_ULA_MODE, 1), ulamode == 1);
    add_checkbox_item(menu,   "Enhanced (8-bit, single access)", menu_id_num(IDM_ULA_MODE, 2), ulamode == 2);
    return menu;
}

//MENU memmenu[6]=
//{
//        {"&Elektuur/Slogger turbo board",gui_turbo,NULL,0,NULL},
//        {"&Slogger/Jafa Master RAM board",gui_mrb,NULL,0,NULL},
//        {"&Master RAM board mode",NULL,mrbmenu,0,NULL},
//        {"Enhanced &ULA mode",NULL,ulamenu,0,NULL},
//        {"&Paged RAM in FD (JIM)",gui_jim,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_memory_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu,   "Elektuur/Slogger turbo board", IDM_MEMORY_TURBO, turbo);
    add_checkbox_item(menu,   "Slogger/Jafa Master RAM board", IDM_MEMORY_MASTER_RAM, mrb);
    al_append_menu_item(menu, "Master RAM board mode", 0, 0, NULL, create_mrb_menu());
    al_append_menu_item(menu, "Enhanced ULA mode", 0, 0, NULL, create_ula_menu());
    add_checkbox_item(menu,   "Paged RAM in FD (JIM)", IDM_MEMORY_ENABLE_JIM, enable_jim);
    return menu;
}


//MENU dischmenu[4]=
//{
//        {"&Plus 3 enable",gui_plus3,NULL,0,NULL},
//        {"&ADFS enable",gui_adfs,NULL,0,NULL},
//        {"&DFS enable",gui_dfs,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_disch_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Plus 3 enable", IDM_DISCCH_PLUS_THREE, plus3);
    add_checkbox_item(menu, "ADFS enable",   IDM_DISCCH_ADFS,       adfsena);
    add_checkbox_item(menu, "DFS enable",    IDM_DISCCH_DFS,        dfsena);
    return menu;
}

//MENU joymenu[3]=
//{
//        {"&Plus 1 joystick interface",gui_plus1,NULL,0,NULL},
//        {"&First Byte joystick interface",gui_first,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_joystick_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    add_checkbox_item(menu, "Plus 1 joystick interface",     IDM_JOYSTICK_PLUS_ONE,   plus1);
    add_checkbox_item(menu, "First Byte joystick interface", IDM_JOYSTICK_FIRST_BYTE, firstbyte);
    return menu;
}

//MENU keymenu[2]=
//{
//        {"Redefine keyboard",gui_keydefine,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_keyboard_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Redefine keyboard", IDM_KEYBOARD_REDEFINE, 0, NULL, NULL);
    return menu;
}

//MENU settingsmenu[7]=
//{
//        {"&Video",NULL,videomenu,0,NULL},
//        {"&Sound",NULL,soundmenu,0,NULL},
//        {"&Memory",NULL,memmenu,0,NULL},
//        {"&Disc",NULL,dischmenu,0,NULL},
//        {"&Joystick",NULL,joymenu,0,NULL},
//        {"&Keyboard",NULL,keymenu,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_settings_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Video", 0, 0, NULL, create_video_menu());
    al_append_menu_item(menu, "Sound", 0, 0, NULL, create_sound_menu());
    al_append_menu_item(menu, "Memory", 0, 0, NULL, create_memory_menu());
    al_append_menu_item(menu, "Disc", 0, 0, NULL, create_disch_menu());
    al_append_menu_item(menu, "Joystick", 0, 0, NULL, create_joystick_menu());
    al_append_menu_item(menu, "Keyboard", 0, 0, NULL, create_keyboard_menu());
    return menu;
}

/******************************************************************************/
/* Misc menu and all sub-menus                                                */
/******************************************************************************/


//MENU miscmenu[5]=
//{
//        {"Save screenshot",gui_scrshot,NULL,0,NULL},
//        {"Start movie",gui_startmovie,NULL,0,NULL},
//        {"Stop movie",gui_stopmovie,NULL,0,NULL},
//        {"Start debugging",gui_startdebugging,NULL,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
static ALLEGRO_MENU *create_misc_menu(void)
{
    ALLEGRO_MENU *menu = al_create_menu();
    al_append_menu_item(menu, "Save screenshot", IDM_MISC_SAVE_SCREENSHOT, 0, NULL, NULL);
    al_append_menu_item(menu, "Start movie",     IDM_MISC_START_MOVIE,     0, NULL, NULL);
    al_append_menu_item(menu, "Stop movie",      IDM_MISC_STOP_MOVIE,      0, NULL, NULL);
    al_append_menu_item(menu, "Start debugging", IDM_MISC_START_DEBUGGING, 0, NULL, NULL);
    return menu;
}

//MENU mainmenu[7]=
//{
//        {"&File",NULL,filemenu,0,NULL},
//        {"&Tape",NULL,tapemenu,0,NULL},
//        {"&Disc",NULL,discmenu,0,NULL},
//        {"&ROM",NULL,rommenu,0,NULL},
//        {"&Settings",NULL,settingsmenu,0,NULL},
//        {"&Misc",NULL,miscmenu,0,NULL},
//        {NULL,NULL,NULL,0,NULL}
//};
void gui_allegro_init(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display)
{
    ALLEGRO_MENU *menu = al_create_menu();
    printf("Menu = %p\n");
    //int res = al_append_menu_item(menu, "File", 0, 0, NULL, create_file_menu());
    //printf("Res = %d", res);
    //res = al_append_menu_item(menu, "Tape", 0, 0, NULL, create_tape_menu());
    //printf("Res = %d", res);
    //res = al_append_menu_item(menu, "Disc", 0, 0, NULL, create_disc_menu());
    //printf("Res = %d", res);
    //res = al_append_menu_item(menu, "ROM", 0, 0, NULL, create_rom_menu());
    //printf("Res = %d", res);
    int res = al_append_menu_item(menu, "Settings", 0, 0, NULL, create_settings_menu());
    printf("Res = %d", res);
    //res = al_append_menu_item(menu, "Misc", 0, 0, NULL, create_misc_menu());
    //printf("Res = %d", res);
    if(al_set_display_menu(display, menu))
    {
        printf("al_set_display_menu ok\n");
    }
    else
    {
        printf("failed - errno=%d\n", al_get_errno());
    }
    al_register_event_source(queue, al_get_default_menu_event_source());
}

void gui_allegro_destroy(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *display)
{
    al_unregister_event_source(queue, al_get_default_menu_event_source());
    al_set_display_menu(display, NULL);
}

//void gui_load0()
//{
//        char tempname[260];
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        memcpy(tempname,discname,260);
//        ret=file_select_ex("Please choose a disc image",tempname,"SSD;DSD;IMG;ADF;ADL;FDI",260,xsize,ysize);
//        if (ret)
//        {
//                closedisc(0);
//                memcpy(discname,tempname,260);
//                loaddisc(0,discname);
//                if (defaultwriteprot) writeprot[0]=1;
//        }
//        return;
//}

//void gui_romload0()
//{
//        char tempname[260];
//        tempname[0]=0;
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        ret=file_select_ex("Please choose a ROM image",tempname,"ROM",260,xsize,ysize);
//        if (ret)
//        {
//                                loadcart(tempname);
//                                reset6502e();
//                                resetula();
//        }
//        return;
//}

//void gui_romload1()
//{
//        char tempname[260];
//        tempname[0]=0;
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        ret=file_select_ex("Please choose a ROM image",tempname,"ROM",260,xsize,ysize);
//        if (ret)
//        {
//                                loadcart2(tempname);
//                                reset6502e();
//                                resetula();
//        }
//        return;
//}

void gui_romeject0()
{
                        unloadcart();
                        reset6502e();
                        resetula();
        return;
}

//void gui_load1()
//{
//        char tempname[260];
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        memcpy(tempname,discname2,260);
//        ret=file_select_ex("Please choose a disc image",tempname,"SSD;DSD;IMG;ADF;ADL;FDI",260,xsize,ysize);
//        if (ret)
//        {
//                closedisc(1);
//                memcpy(discname2,tempname,260);
//                loaddisc(1,discname2);
//                if (defaultwriteprot) writeprot[1]=1;
//        }
//        return;
//}

void gui_eject0()
{
        closedisc(0);
        discname[0]=0;
        return;
}
void gui_eject1()
{
        closedisc(1);
        discname2[0]=0;
        return;
}

void gui_wprotd()
{
        defaultwriteprot=!defaultwriteprot;
}

void gui_normal()
{
        tapespeed=0;
}
void gui_fast()
{
        tapespeed=1;
}
void gui_rfast()
{
        tapespeed=2;
}

//void gui_loadt()
//{
//        char tempname[260];
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        memcpy(tempname,tapename,260);
//        ret=file_select_ex("Please choose a tape image",tempname,"UEF;CSW",260,xsize,ysize);
//        if (ret)
//        {
//                closeuef();
//                closecsw();
//                memcpy(tapename,tempname,260);
//                loadtape(tapename);
//        }
//        return;
//}

void gui_rewind()
{
        closeuef();
        closecsw();
        loadtape(tapename);
}

void gui_ejectt()
{
        closeuef();
        closecsw();
}


void gui_disp()
{
//        drawmode=(intptr_t)active_menu->dp;
}

void gui_fullscreen()
{
        if (fullscreen)
        {
                fullscreen=0;
                leavefullscreen();
        }
        else
        {
                fullscreen=1;
                enterfullscreen();
        }
}

void gui_waveform()
{
//        curwave=(int)active_menu->dp;
}

//MENU waveformmenu[6]=
//{
//        {"Square",gui_waveform,NULL,0,(void *)0},
//        {"Saw",gui_waveform,NULL,0,(void *)1},
//        {"Sine",gui_waveform,NULL,0,(void *)2},
//        {"Triangle",gui_waveform,NULL,0,(void *)3},
//        {"SID",gui_waveform,NULL,0,(void *)4},
//        {NULL,NULL,NULL,0,NULL}
//};

void gui_ddtype()
{
 //       ddtype=(intptr_t)active_menu->dp;
        closeddnoise();
        loaddiscsamps();
}


void gui_ddvol()
{
 //       ddvol=(intptr_t)active_menu->dp;
}

void gui_internalsnd()
{
        sndint=!sndint;
}
void gui_sndex()
{
        sndex=!sndex;
        resetit=1;
}
void gui_ddnoise()
{
        sndddnoise=!sndddnoise;
        return;
}
void gui_tnoise()
{
        sndtape=!sndtape;
        return;
}
/*void gui_filter()
{
        soundfilter=!soundfilter;
        return;
}*/

void gui_plus3()
{
        plus3=!plus3;
        if (plus3) firstbyte=0;
        resetit=1;
        return;
}
void gui_adfs()
{
        adfsena=!adfsena;
        resetit=1;
        return;
}
void gui_dfs()
{
        dfsena=!dfsena;
        resetit=1;
        return;
}

void gui_mrbmode()
{
  //      mrbmode=(intptr_t)active_menu->dp;
        resetit=1;
        return;
}

void gui_ulamode()
{
  //      ulamode=(intptr_t)active_menu->dp;
        resetit=1;
        return;
}

void gui_turbo()
{
        turbo=!turbo;
        if (turbo) mrb=0;
        resetit=1;
        return;
}

void gui_mrb()
{
        mrb=!mrb;
        if (mrb) turbo=0;
        resetit=1;
        return;
}

void gui_jim()
{
        enable_jim=!enable_jim;
        resetit=1;
        return;
}

void gui_plus1()
{
        plus1=!plus1;
        if (plus1) firstbyte=0;
        resetit=1;
        return;
}

void gui_first()
{
        firstbyte=!firstbyte;
        if (firstbyte) plus1=plus3=0;
        resetit=1;
        return;
}

//void gui_scrshot()
//{
//        char tempname[260];
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        tempname[0]=0;
//        ret=file_select_ex("Please enter filename",tempname,"BMP",260,xsize,ysize);
//        if (ret)
//        {
//                memcpy(scrshotname,tempname,260);
//                savescrshot(scrshotname);
//        }
//        return;
//}

//void gui_startmovie()
//{
//        char tempname[260];
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        tempname[0]=0;
//        ret=file_select_ex("Please enter filename",tempname,"VID",260,xsize,ysize);
//        if (ret)
//        {
//                memcpy(moviename,tempname,260);
//                startmovie(moviename);
//        }
//        return;
//}

void gui_stopmovie()
{
    stopmovie(moviename);
    return;
}

void gui_startdebugging()
{
    debug=debugon=1;
    startdebug();
}

void gui_mgc()
{
        enable_mgc=!enable_mgc;
        return;
}
void gui_db_flash_cartridge()
{
        enable_db_flash_cartridge=!enable_db_flash_cartridge;
        return;
}

//DIALOG bemgui[]=
//{
//        {d_ctext_proc, 200, 260, 0,  0, 15,0,0,0,     0,0,"Elkulator v1.0"},
//        {d_menu_proc,  0,   0,   0,  0, 15,0,0,0,     0,0,mainmenu},
//        {d_yield_proc},
//        {0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL}
//};

//BITMAP *mouse,*_mouse_sprite;

//void entergui()
//{
//        int x=1;
//        DIALOG_PLAYER *dp;
        
        //BITMAP *guib;
        
//        while (keypressed()) readkey();
//        while (menu_pressed()) rest(100);

//        hal_set_color_depth(hal_get_desktop_color_depth());
//        show_mouse(screen);
//        bemgui[0].x=(windx/2)-36;
//        bemgui[0].y=windy-8;
//        bemgui[0].fg=makecol(255,255,255);
/*        if (opengl)
        {
                guib=create_bitmap(SCREEN_W,SCREEN_H);
                clear(guib);
                gui_set_screen(guib);
                allegro_gl_set_allegro_mode();
        }*/
//        dp=init_dialog(bemgui,0);
//        while (x && !menu_pressed() && !key[KEY_ESC] && !quited)
//        {
//                x=update_dialog(dp);
//        }
//        shutdown_dialog(dp);
//        show_mouse(NULL);
//        hal_set_color_depth(8);
//
//        while (menu_pressed()) rest(100);
//
//        clearscreen();
//}
#endif

//void gui_loads()
//{
//        char tempname[260];
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        memcpy(tempname,ssname,260);
//        ret=file_select_ex("Please choose a save state",tempname,"SNP",260,xsize,ysize);
//        if (ret)
//        {
//                memcpy(ssname,tempname,260);
//                loadstate(ssname);
//        }
//        return;
//}

//static void file_load_state(ALLEGRO_EVENT *event)
//{
//    ALLEGRO_DISPLAY *display = (ALLEGRO_DISPLAY *)(event->user.data2);
//    ALLEGRO_FILECHOOSER *chooser = al_create_native_file_dialog(savestate_name, "Load state from file", "*.snp", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
//    if (chooser) {
//        if (al_show_native_file_dialog(display, chooser)) {
//            if (al_get_native_file_dialog_count(chooser) > 0)
//                loadstate(al_get_native_file_dialog_path(chooser, 0));
//        }
//        al_destroy_native_file_dialog(chooser);
//    }
//}

//void gui_saves()
//{
//        char tempname[260];
//        int ret;
//        int xsize=windx-32,ysize=windy-16;
//        memcpy(tempname,ssname,260);
//        ret=file_select_ex("Please choose a save state",tempname,"SNP",260,xsize,ysize);
//        if (ret)
//        {
//                memcpy(ssname,tempname,260);
//                savestate(ssname);
//        }
//        return;
//}

//static void file_save_state(ALLEGRO_EVENT *event)
//{
//    ALLEGRO_FILECHOOSER *chooser;
//    ALLEGRO_DISPLAY *display;
//
//    if ((chooser = al_create_native_file_dialog(savestate_name, "Save state to file", "*.snp", ALLEGRO_FILECHOOSER_SAVE))) {
//        display = (ALLEGRO_DISPLAY *)(event->user.data2);
//        if (al_show_native_file_dialog(display, chooser)) {
//            if (al_get_native_file_dialog_count(chooser) > 0)
//                savestate(al_get_native_file_dialog_path(chooser, 0));
//        }
//        al_destroy_native_file_dialog(chooser);
//    }
//}

/* Menus event handler */
void gui_allegro_event(ALLEGRO_EVENT *event)
{
    switch(menu_get_id(event)) 
    {
        case IDM_ZERO:
            break;

        // File Menu handler
        case IDM_FILE_HARD_RESET:
            resetit=1;  // Set reset flag.
            break;

        case IDM_FILE_SOFT_RESET:
            resetit=1;  // Set reset flag.
            break;

        case IDM_FILE_LOAD_STATE:
//            file_load_state(event); // (gui_loads)
            break;

        case IDM_FILE_SAVE_STATE:
//            file_save_state(event); // gui_saves
            break;

        case IDM_FILE_EXIT:
            quited = true; //gui_exit
            break;

        // TODO: Cut an dpast is useful - implement.
        //case IDM_EDIT_PASTE:
        //    edit_paste_start(event);
        //    break;
        //case IDM_EDIT_COPY:
        //    edit_print_clip(event);
        //    break;

        // Main Disc Menu
//MENU discmenu[8]=
//{
//       {"Load disc :&0/2...",gui_load0,NULL,0,NULL},
//       {"Load disc :&1/3...",gui_load1,NULL,0,NULL},
//       {"Eject disc :0/2",gui_eject0,NULL,0,NULL},
//       {"Eject disc :1/3",gui_eject1,NULL,0,NULL},
//       {"Write protect disc :0/2",gui_wprot0,NULL,0,NULL},
//       {"Write protect disc :1/3",gui_wprot1,NULL,0,NULL},
//       {"Default write protect",gui_wprotd,NULL,0,NULL},
//       {NULL,NULL,NULL,0,NULL}
//};
        //case IDM_DISC_AUTOBOOT:
        //    disc_choose(event, "autoboot in", all_dext, ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
        //    break;
        case IDM_DISC_LOAD:  // gui_load0 / gui_load1
//            disc_choose(event, "load into", all_dext, ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
            break;
        case IDM_DISC_EJECT:  // gui_eject0 // gui_eject1
//            disc_eject(event);
            break;
        
        case IDM_DISC_WPROT:  // gui_wprot0 // gui_wprot1
        {
            int drive = menu_get_num(event);
            writeprot[drive]=!writeprot[drive];
            if (fwriteprot[drive])
            {
                fwriteprot[drive]=1;
            }
            break;
        }

        case IDM_DISC_WPROT_D:
            defaultwriteprot = !defaultwriteprot;
            break;

        //case IDM_DISC_NEW_ADFS_S:
        //    disc_choose_new(event, "*.ads");
        //    break;
        //case IDM_DISC_NEW_ADFS_M:
        //    disc_choose_new(event, "*.adm");
        //    break;
        //case IDM_DISC_NEW_ADFS_L:
        //    disc_choose_new(event, "*.adl");
        //    break;
        //case IDM_DISC_NEW_DFS_10S_SIN_40T:
        //case IDM_DISC_NEW_DFS_10S_SIN_80T:
        //    disc_choose_new(event, "*.ssd");
        //    break;
        //case IDM_DISC_NEW_DFS_10S_INT_40T:
        //case IDM_DISC_NEW_DFS_10S_INT_80T:
        //    disc_choose_new(event, "*.dsd");
        //    break;
        //case IDM_DISC_NEW_DFS_16S_SIN_40T:
        //case IDM_DISC_NEW_DFS_16S_SIN_80T:
        //case IDM_DISC_NEW_DFS_18S_SIN_40T:
        //case IDM_DISC_NEW_DFS_18S_SIN_80T:
        //    disc_choose_new(event, "*.sdd");
        //    break;
        //case IDM_DISC_NEW_DFS_16S_INT_80T:
        //case IDM_DISC_NEW_DFS_18S_INT_80T:
        //    disc_choose_new(event, "*.ddd");
        //    break;


//        case IDM_TAPE_LOAD:
//            tape_load_ui(event);
//            break;
//        case IDM_TAPE_REWIND:
//            tape_rewind();
//            break;
//        case IDM_TAPE_EJECT:
//            tape_eject();
//            break;
//        case IDM_TAPE_SPEED_NORMAL:
//            tape_normal(event);
//            break;
//        case IDM_TAPE_SPEED_FAST:
//            tape_fast(event);
//            break;
//        case IDM_TAPE_CAT:
//            gui_tapecat_start();
//            break;
//        case IDM_ROMS_LOAD:
//            rom_load(event);
//            break;
//        case IDM_ROMS_CLEAR:
//            rom_clear(event);
//            break;
//        case IDM_ROMS_RAM:
//            rom_ram_toggle(event);
//            break;
//        case IDM_MODEL:
//            change_model(event);
//            break;
//        case IDM_VIDEO_DISPTYPE:
//            video_set_disptype(radio_event_simple(event, vid_dtype_user));
//            break;
//        case IDM_VIDEO_BORDERS:
//            video_set_borders(radio_event_simple(event, vid_fullborders));
//            break;
//        case IDM_VIDEO_WIN_MULT:
//            video_set_multipier(radio_event_simple(event, vid_win_multiplier));
//            break;
//        case IDM_VIDEO_WINSIZE:
//            video_set_borders(vid_fullborders);
//            break;
//        case IDM_VIDEO_FULLSCR:
//            toggle_fullscreen();
//            break;
//        case IDM_VIDEO_PAL:
//            vid_pal = !vid_pal;
//            break;
//        case IDM_VIDEO_NULA:
//            nula_disable = !nula_disable;
//            break;
//        case IDM_VIDEO_LED_LOCATION:
//            video_set_led_location(radio_event_simple(event, vid_ledlocation));
//            break;
//        case IDM_VIDEO_LED_VISIBILITY:
//            video_set_led_visibility(radio_event_simple(event, vid_ledvisibility));
//            break;
//        case IDM_VIDEO_MODE7_FONT:
//            change_mode7_font(event);
//            break;
//        case IDM_SOUND_INTERNAL:
//            sound_internal = !sound_internal;
//            break;
//        case IDM_SOUND_BEEBSID:
//            sound_beebsid = !sound_beebsid;
//            break;
//        case IDM_SOUND_PAULA:
//            sound_paula = !sound_paula;
//            break;
//        case IDM_SOUND_DAC:
//            sound_dac = !sound_dac;
//            break;
//        case IDM_SOUND_DDNOISE:
//            sound_ddnoise = !sound_ddnoise;
//            break;
//        case IDM_SOUND_TAPE:
//            sound_tape = !sound_tape;
//            break;
//        case IDM_SOUND_FILTER:
//            sound_filter = !sound_filter;
//            break;
//        case IDM_WAVE:
//            curwave = radio_event_simple(event, curwave);
//            break;
//        case IDM_DISC_TYPE:
//            change_ddnoise_dtype(event);
//            break;
//        case IDM_DISC_VOL:
//            ddnoise_vol = radio_event_simple(event, ddnoise_vol);
//            break;
//        case IDM_SPEED:
//            main_setspeed(radio_event_simple(event, emuspeed));
//            break;
//        case IDM_AUTOSKIP:
//            autoskip = !autoskip;
//            break;
//        case IDM_DEBUGGER:
//            debug_toggle_core();
//            break;
//        case IDM_DEBUG_BREAK:
//            debug_step = 1;
//            break;
//        case IDM_KEY_REDEFINE:
//            gui_keydefine_open();
//            break;
//        case IDM_KEY_AS:
//            keyas = !keyas;
//            break;
//        case IDM_KEY_LOGICAL:
//            keylogical = !keylogical;
//            key_reset();
//            break;
//        case IDM_KEY_PAD:
//            keypad = !keypad;
//            break;
//        case IDM_JIM_SIZE:
//            mem_jim_setsize(radio_event_simple(event, mem_jim_size));
//            break;
//        case IDM_AUTO_PAUSE:
//            autopause = !autopause;
//            break;
//        case IDM_MOUSE_AMX:
//            mouse_amx = !mouse_amx;
//            break;
//        case IDM_TRIACK_SEGA_ADAPTER:
//            tricky_sega_adapter = !tricky_sega_adapter;
//            remap_joystick(0);
//            remap_joystick(1);
//            break;
//        case IDM_JOYSTICK:
//            change_joystick(0, radio_event_with_deselect(event, joystick_index[0]));
//            break;
//        case IDM_JOYSTICK2:
//            change_joystick(1, radio_event_with_deselect(event, joystick_index[1]));
//        case IDM_MOUSE_STICK:
//            mouse_stick = !mouse_stick;
//            break;
//        case IDM_JOYMAP:
//            joymap_index[0] = radio_event_simple(event, joymap_index[0]);
//            remap_joystick(0);
//        case IDM_JOYMAP2:
//            joymap_index[1] = radio_event_simple(event, joymap_index[1]);
//            remap_joystick(1);
//            break;
    }
}

