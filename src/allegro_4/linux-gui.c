/*Elkulator v1.0 by Sarah Walker
  Linux GUI*/

#ifndef WIN32
#include <allegro.h>
//#include <alleggl.h>
#include "elk.h"

#undef printf

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

MENU filemenu[6];
MENU discmenu[8];
MENU rommenu[5];
MENU tapespdmenu[4];
MENU tapemenu[5];
MENU displaymenu[7];
MENU bordersmenu[4];
MENU videomenu[3];
MENU soundmenu[7];
MENU keymenu[2];
MENU dischmenu[4];
MENU memmenu[6];
MENU mrbmenu[4];
MENU ulamenu[4];
MENU settingsmenu[7];
MENU miscmenu[5];
MENU mainmenu[7];
MENU joymenu[3];
MENU ddtypemenu[3];
MENU ddvolmenu[4];
MENU romcartsmenu[3];

void updatelinuxgui()
{
        int x;
        discmenu[4].flags=(writeprot[0])?D_SELECTED:0;
        discmenu[5].flags=(writeprot[1])?D_SELECTED:0;
        discmenu[6].flags=(defaultwriteprot)?D_SELECTED:0;
        tapespdmenu[0].flags=(!tapespeed)?D_SELECTED:0;
        tapespdmenu[1].flags=(tapespeed==1)?D_SELECTED:0;
        tapespdmenu[2].flags=(tapespeed==2)?D_SELECTED:0;
        for (x=0;x<6;x++)  displaymenu[x].flags=(drawmode==(intptr_t)displaymenu[x].dp)?D_SELECTED:0;
        videomenu[1].flags=(fullscreen)?D_SELECTED:0;
        soundmenu[0].flags=(sndint)?D_SELECTED:0;
        soundmenu[1].flags=(sndex)?D_SELECTED:0;
        soundmenu[2].flags=(sndddnoise)?D_SELECTED:0;
        dischmenu[0].flags=(plus3)?D_SELECTED:0;
        dischmenu[1].flags=(adfsena)?D_SELECTED:0;
        dischmenu[2].flags=(dfsena)?D_SELECTED:0;
        memmenu[0].flags=(turbo)?D_SELECTED:0;
        memmenu[1].flags=(mrb)?D_SELECTED:0;
        for (x=0;x<3;x++)  mrbmenu[x].flags=(mrbmode==(intptr_t)mrbmenu[x].dp)?D_SELECTED:0;
        for (x=0;x<3;x++)  ulamenu[x].flags=(ulamode==(intptr_t)ulamenu[x].dp)?D_SELECTED:0;
        memmenu[4].flags=(enable_jim)?D_SELECTED:0;
        joymenu[0].flags=(plus1)?D_SELECTED:0;
        joymenu[1].flags=(firstbyte)?D_SELECTED:0;
//        for (x=0;x<5;x++)  waveformmenu[x].flags=(curwave==(intptr_t)waveformmenu[x].dp)?D_SELECTED:0;
        ddtypemenu[0].flags=(!ddtype)?D_SELECTED:0;
        ddtypemenu[1].flags=(ddtype)?D_SELECTED:0;
        for (x=0;x<3;x++)  ddvolmenu[x].flags=(ddvol==(intptr_t)ddvolmenu[x].dp)?D_SELECTED:0;
//        keymenu[1].flags=(keyas)?D_SELECTED:0;
        romcartsmenu[0].flags = (enable_mgc)?D_SELECTED:0;
        romcartsmenu[1].flags = (enable_db_flash_cartridge)?D_SELECTED:0;
}

int gui_keydefine();

int gui_return()
{
        return D_CLOSE;
}

int gui_reset()
{
        resetit=1;
        return D_O_K;
}

int gui_exit()
{
        quited=1;
        return D_CLOSE;
}

int gui_loads()
{
        char tempname[260];
        int ret;
        int xsize=windx-32,ysize=windy-16;
        memcpy(tempname,ssname,260);
        ret=file_select_ex("Please choose a save state",tempname,"SNP",260,xsize,ysize);
        if (ret)
        {
                memcpy(ssname,tempname,260);
                loadstate(ssname);
        }
        return D_O_K;
}
int gui_saves()
{
        char tempname[260];
        int ret;
        int xsize=windx-32,ysize=windy-16;
        memcpy(tempname,ssname,260);
        ret=file_select_ex("Please choose a save state",tempname,"SNP",260,xsize,ysize);
        if (ret)
        {
                memcpy(ssname,tempname,260);
                savestate(ssname);
        }
        return D_O_K;
}
MENU filemenu[6]=
{
        {"&Return",gui_return,NULL,0,NULL},
        {"&Hard reset",gui_reset,NULL,0,NULL},
        {"&Load state",gui_loads,NULL,0,NULL},
        {"&Save state",gui_saves,NULL,0,NULL},
        {"&Exit",gui_exit,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_load0()
{
        char tempname[260];
        int ret;
        int xsize=windx-32,ysize=windy-16;
        memcpy(tempname,discname,260);
        ret=file_select_ex("Please choose a disc image",tempname,"SSD;DSD;IMG;ADF;ADL;FDI",260,xsize,ysize);
        if (ret)
        {
                closedisc(0);
                memcpy(discname,tempname,260);
                loaddisc(0,discname);
                if (defaultwriteprot) writeprot[0]=1;
        }
        updatelinuxgui();
        return D_O_K;
}

int gui_romload0()
{
        char tempname[260];
        tempname[0]=0;
        int ret;
        int xsize=windx-32,ysize=windy-16;
        ret=file_select_ex("Please choose a ROM image",tempname,"ROM",260,xsize,ysize);
        if (ret)
        {
                                loadcart(tempname);
                                reset6502e();
                                resetula();
        }
        updatelinuxgui();
        return D_O_K;
}

int gui_romload1()
{
        char tempname[260];
        tempname[0]=0;
        int ret;
        int xsize=windx-32,ysize=windy-16;
        ret=file_select_ex("Please choose a ROM image",tempname,"ROM",260,xsize,ysize);
        if (ret)
        {
                                loadcart2(tempname);
                                reset6502e();
                                resetula();
        }
        updatelinuxgui();
        return D_O_K;
}

int gui_romeject0()
{
                        unloadcart();
                        reset6502e();
                        resetula();
        return D_O_K;
}

int gui_load1()
{
        char tempname[260];
        int ret;
        int xsize=windx-32,ysize=windy-16;
        memcpy(tempname,discname2,260);
        ret=file_select_ex("Please choose a disc image",tempname,"SSD;DSD;IMG;ADF;ADL;FDI",260,xsize,ysize);
        if (ret)
        {
                closedisc(1);
                memcpy(discname2,tempname,260);
                loaddisc(1,discname2);
                if (defaultwriteprot) writeprot[1]=1;
        }
        updatelinuxgui();
        return D_O_K;
}

int gui_eject0()
{
        closedisc(0);
        discname[0]=0;
        return D_O_K;
}
int gui_eject1()
{
        closedisc(1);
        discname2[0]=0;
        return D_O_K;
}

int gui_wprot0()
{
        writeprot[0]=!writeprot[0];
        if (fwriteprot[0]) fwriteprot[0]=1;
        updatelinuxgui();
        return D_O_K;
}
int gui_wprot1()
{
        writeprot[1]=!writeprot[1];
        if (fwriteprot[1]) fwriteprot[1]=1;
        updatelinuxgui();
        return D_O_K;
}
int gui_wprotd()
{
        defaultwriteprot=!defaultwriteprot;
        updatelinuxgui();
        return D_O_K;
}

MENU discmenu[8]=
{
        {"Load disc :&0/2...",gui_load0,NULL,0,NULL},
        {"Load disc :&1/3...",gui_load1,NULL,0,NULL},
        {"Eject disc :0/2",gui_eject0,NULL,0,NULL},
        {"Eject disc :1/3",gui_eject1,NULL,0,NULL},
        {"Write protect disc :0/2",gui_wprot0,NULL,0,NULL},
        {"Write protect disc :1/3",gui_wprot1,NULL,0,NULL},
        {"Default write protect",gui_wprotd,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

MENU rommenu[5]=
{
        {"Load ROM cartridge &1...",gui_romload0,NULL,0,NULL},
        {"Load ROM cartridge &2...",gui_romload1,NULL,0,NULL},
        {"&Unload ROM cartridges...",gui_romeject0,NULL,0,NULL},
        {"Multi-ROM expansions",NULL,romcartsmenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_normal()
{
        tapespeed=0;
        updatelinuxgui();
        return D_O_K;
}
int gui_fast()
{
        tapespeed=1;
        updatelinuxgui();
        return D_O_K;
}
int gui_rfast()
{
        tapespeed=2;
        updatelinuxgui();
        return D_O_K;
}

MENU tapespdmenu[4]=
{
        {"Normal",gui_normal,NULL,0,NULL},
        {"Fast",gui_fast,NULL,0,NULL},
        {"Really Fast",gui_rfast,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_loadt()
{
        char tempname[260];
        int ret;
        int xsize=windx-32,ysize=windy-16;
        memcpy(tempname,tapename,260);
        ret=file_select_ex("Please choose a tape image",tempname,"UEF;CSW",260,xsize,ysize);
        if (ret)
        {
                closeuef();
                closecsw();
                memcpy(tapename,tempname,260);
                loadtape(tapename);
//                tapeloaded=1;
        }
        return D_O_K;
}

int gui_rewind()
{
        closeuef();
        closecsw();
        loadtape(tapename);
        return D_O_K;
}

int gui_ejectt()
{
        closeuef();
        closecsw();
//        tapeloaded=0;
        return D_O_K;
}

MENU tapemenu[]=
{
        {"Load tape...",gui_loadt,NULL,0,NULL},
        {"Rewind tape",gui_rewind,NULL,0,NULL},
        {"Eject tape",gui_ejectt,NULL,0,NULL},
        {"Tape speed",NULL,tapespdmenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_disp()
{
        drawmode=(intptr_t)active_menu->dp;
        updatelinuxgui();
        return D_O_K;
}

MENU displaymenu[7]=
{
        {"Scanlines",gui_disp,NULL,0,(void *)0},
        {"Line doubling",gui_disp,NULL,0,(void *)1},
        {"2xSaI",gui_disp,NULL,0,(void *)2},
        {"Scale2X",gui_disp,NULL,0,(void *)3},
        {"Super Eagle",gui_disp,NULL,0,(void *)4},
        {"PAL Filter",gui_disp,NULL,0,(void *)5},
        {NULL,NULL,NULL,0,NULL}
};

int gui_fullscreen()
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
        return D_EXIT;
}

MENU videomenu[3]=
{
        {"Display type",NULL,displaymenu,0,NULL},
        {"Fullscreen",gui_fullscreen,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_waveform()
{
//        curwave=(int)active_menu->dp;
        updatelinuxgui();
        return D_O_K;
}

MENU waveformmenu[6]=
{
        {"Square",gui_waveform,NULL,0,(void *)0},
        {"Saw",gui_waveform,NULL,0,(void *)1},
        {"Sine",gui_waveform,NULL,0,(void *)2},
        {"Triangle",gui_waveform,NULL,0,(void *)3},
        {"SID",gui_waveform,NULL,0,(void *)4},
        {NULL,NULL,NULL,0,NULL}
};

int gui_ddtype()
{
        ddtype=(intptr_t)active_menu->dp;
        closeddnoise();
        loaddiscsamps();
        updatelinuxgui();
        return D_O_K;
}

MENU ddtypemenu[3]=
{
        {"5.25",gui_ddtype,NULL,0,(void *)0},
        {"3.5",gui_ddtype,NULL,0,(void *)1},
        {NULL,NULL,NULL,0,NULL}
};

int gui_ddvol()
{
        ddvol=(intptr_t)active_menu->dp;
        updatelinuxgui();
        return D_O_K;
}

MENU ddvolmenu[4]=
{
        {"33%",gui_ddvol,NULL,0,(void *)1},
        {"66%",gui_ddvol,NULL,0,(void *)2},
        {"100%",gui_ddvol,NULL,0,(void *)3},
        {NULL,NULL,NULL,0,NULL}
};

int gui_internalsnd()
{
        sndint=!sndint;
        updatelinuxgui();
        return D_O_K;
}
int gui_sndex()
{
        sndex=!sndex;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}
int gui_ddnoise()
{
        sndddnoise=!sndddnoise;
        updatelinuxgui();
        return D_O_K;
}
int gui_tnoise()
{
        sndtape=!sndtape;
        updatelinuxgui();
        return D_O_K;
}
/*int gui_filter()
{
        soundfilter=!soundfilter;
        updatelinuxgui();
        return D_O_K;
}*/

MENU soundmenu[7]=
{
        {"Internal speaker",gui_internalsnd,NULL,0,NULL},
        {"CSS Sound Expansion",gui_sndex,NULL,0,NULL},
        {"Disc drive noise",gui_ddnoise,NULL,0,NULL},
        {"Tape noise",gui_tnoise,NULL,0,NULL},
        {"Disc drive type",NULL,ddtypemenu,0,NULL},
        {"Disc drive volume",NULL,ddvolmenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

MENU keymenu[2]=
{
        {"Redefine keyboard",gui_keydefine,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_plus3()
{
        plus3=!plus3;
        if (plus3) firstbyte=0;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}
int gui_adfs()
{
        adfsena=!adfsena;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}
int gui_dfs()
{
        dfsena=!dfsena;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

MENU dischmenu[4]=
{
        {"&Plus 3 enable",gui_plus3,NULL,0,NULL},
        {"&ADFS enable",gui_adfs,NULL,0,NULL},
        {"&DFS enable",gui_dfs,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_mrbmode()
{
        mrbmode=(intptr_t)active_menu->dp;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

MENU mrbmenu[4]=
{
        {"&Off",gui_mrbmode,NULL,0,(void *)0},
        {"&Turbo",gui_mrbmode,NULL,0,(void *)1},
        {"&Shadow",gui_mrbmode,NULL,0,(void *)2},
        {NULL,NULL,NULL,0,NULL}
};

int gui_ulamode()
{
        ulamode=(intptr_t)active_menu->dp;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

MENU ulamenu[4]=
{
        {"&Standard",gui_ulamode,NULL,0,(void *)0},
        {"&Enhanced (8-bit, dual access)",gui_ulamode,NULL,0,(void *)ULA_RAM_8BIT_DUAL_ACCESS},
        {"&Enhanced (8-bit, single access)",gui_ulamode,NULL,0,(void *)ULA_RAM_8BIT_SINGLE_ACCESS},
        {NULL,NULL,NULL,0,NULL}
};

int gui_turbo()
{
        turbo=!turbo;
        if (turbo) mrb=0;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

int gui_mrb()
{
        mrb=!mrb;
        if (mrb) turbo=0;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

int gui_jim()
{
        enable_jim=!enable_jim;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

MENU memmenu[6]=
{
        {"&Elektuur/Slogger turbo board",gui_turbo,NULL,0,NULL},
        {"&Slogger/Jafa Master RAM board",gui_mrb,NULL,0,NULL},
        {"&Master RAM board mode",NULL,mrbmenu,0,NULL},
        {"Enhanced &ULA mode",NULL,ulamenu,0,NULL},
        {"&Paged RAM in FD (JIM)",gui_jim,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_plus1()
{
        plus1=!plus1;
        if (plus1) firstbyte=0;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

int gui_first()
{
        firstbyte=!firstbyte;
        if (firstbyte) plus1=plus3=0;
        resetit=1;
        updatelinuxgui();
        return D_O_K;
}

MENU joymenu[3]=
{
        {"&Plus 1 joystick interface",gui_plus1,NULL,0,NULL},
        {"&First Byte joystick interface",gui_first,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

MENU settingsmenu[7]=
{
        {"&Video",NULL,videomenu,0,NULL},
        {"&Sound",NULL,soundmenu,0,NULL},
        {"&Memory",NULL,memmenu,0,NULL},
        {"&Disc",NULL,dischmenu,0,NULL},
        {"&Joystick",NULL,joymenu,0,NULL},
        {"&Keyboard",NULL,keymenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_scrshot()
{
        char tempname[260];
        int ret;
        int xsize=windx-32,ysize=windy-16;
        tempname[0]=0;
        ret=file_select_ex("Please enter filename",tempname,"BMP",260,xsize,ysize);
        if (ret)
        {
                memcpy(scrshotname,tempname,260);
                savescrshot(scrshotname);
        }
        return D_O_K;
}

int gui_startmovie()
{
        char tempname[260];
        int ret;
        int xsize=windx-32,ysize=windy-16;
        tempname[0]=0;
        ret=file_select_ex("Please enter filename",tempname,"VID",260,xsize,ysize);
        if (ret)
        {
                memcpy(moviename,tempname,260);
                startmovie(moviename);
        }
        return D_O_K;
}

int gui_stopmovie()
{
    stopmovie(moviename);
    return D_O_K;
}

int gui_startdebugging()
{
    debug=debugon=1;
    startdebug();
}

MENU miscmenu[5]=
{
        {"Save screenshot",gui_scrshot,NULL,0,NULL},
        {"Start movie",gui_startmovie,NULL,0,NULL},
        {"Stop movie",gui_stopmovie,NULL,0,NULL},
        {"Start debugging",gui_startdebugging,NULL,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

int gui_mgc()
{
        enable_mgc=!enable_mgc;
        updatelinuxgui();
        return D_O_K;
}
int gui_db_flash_cartridge()
{
        enable_db_flash_cartridge=!enable_db_flash_cartridge;
        updatelinuxgui();
        return D_O_K;
}

MENU romcartsmenu[3]=
{
        {"Mega Games Cartridge",gui_mgc,0,0,NULL},
        {"David's Flash ROM Cartridge",gui_db_flash_cartridge,0,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

MENU mainmenu[7]=
{
        {"&File",NULL,filemenu,0,NULL},
        {"&Tape",NULL,tapemenu,0,NULL},
        {"&Disc",NULL,discmenu,0,NULL},
        {"&ROM",NULL,rommenu,0,NULL},
        {"&Settings",NULL,settingsmenu,0,NULL},
        {"&Misc",NULL,miscmenu,0,NULL},
        {NULL,NULL,NULL,0,NULL}
};

DIALOG bemgui[]=
{
        {d_ctext_proc, 200, 260, 0,  0, 15,0,0,0,     0,0,"Elkulator v1.0"},
        {d_menu_proc,  0,   0,   0,  0, 15,0,0,0,     0,0,mainmenu},
        {d_yield_proc},
        {0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL}
};

BITMAP *mouse,*_mouse_sprite;

void entergui()
{
        int x=1;
        DIALOG_PLAYER *dp;
        
        //BITMAP *guib;
        
        while (keypressed()) readkey();
        while (menu_pressed()) rest(100);

        updatelinuxgui();


        set_color_depth(desktop_color_depth());
        show_mouse(screen);
        bemgui[0].x=(windx/2)-36;
        bemgui[0].y=windy-8;
        bemgui[0].fg=makecol(255,255,255);
/*        if (opengl)
        {
                guib=create_bitmap(SCREEN_W,SCREEN_H);
                clear(guib);
                gui_set_screen(guib);
                allegro_gl_set_allegro_mode();
        }*/
        dp=init_dialog(bemgui,0);
        while (x && !menu_pressed() && !key[KEY_ESC] && !quited)
        {
/*                if (opengl)
                {
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }*/
                x=update_dialog(dp);
/*                if (opengl)
                {
//                        algl_draw_mouse();
                        blit(guib,screen,0,0,0,0,SCREEN_W,SCREEN_H);
                        putpixel(screen,mouse_x,mouse_y,makecol(255,255,255));
                        putpixel(screen,mouse_x+1,mouse_y,makecol(255,255,255));
                        putpixel(screen,mouse_x+2,mouse_y,makecol(255,255,255));
                        putpixel(screen,mouse_x+3,mouse_y,makecol(255,255,255));
                        putpixel(screen,mouse_x,mouse_y+1,makecol(255,255,255));
                        putpixel(screen,mouse_x,mouse_y+2,makecol(255,255,255));
                        putpixel(screen,mouse_x,mouse_y+3,makecol(255,255,255));
                        putpixel(screen,mouse_x+1,mouse_y+1,makecol(255,255,255));
                        putpixel(screen,mouse_x+2,mouse_y+2,makecol(255,255,255));
                        putpixel(screen,mouse_x+3,mouse_y+3,makecol(255,255,255));
//                        allegro_gl_unset_allegro_mode();
                        allegro_gl_flip();
                }*/
//              updatelinuxgui();
        }
        shutdown_dialog(dp);
        show_mouse(NULL);
/*        if (opengl)
        {
                destroy_bitmap(guib);
                allegro_gl_unset_allegro_mode();
        }*/
        set_color_depth(8);

        while (menu_pressed()) rest(100);

        clearscreen();
}
#endif
