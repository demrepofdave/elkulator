/*Elkulator v1.0 by Tom walker
  Initialisation/Closing/Main loop*/
#ifdef HAL_ALLEGRO_5
#include <allegro5/allegro.h>
#else
#include <allegro.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elk.h"
#include "common/video.h"
#include "common/keyboard.h"
#include "common/fileutils.h"
#undef printf
int autoboot;
FILE *rlog;
void rpclog(char *format, ...)
{
   char buf[256];
   return;
   if (!rlog) rlog=fopen("e:/devcpp/cycleelk/rlog.txt","wt");
//turn;
   va_list ap;
   va_start(ap, format);
   vsprintf(buf, format, ap);
   va_end(ap);
   fputs(buf,rlog);
   fflush(rlog);
}

int drawit=0;
void drawitint()
{
        drawit++;
}

void cleardrawit()
{
        drawit=0;
}

char exedir[MAX_PATH_FILENAME_BUFFER_SIZE];
char tapename[512];
char parallelname[512];
char serialname[512];
extern int serial_debug;
char romnames[16][1024];

void initelk(int argc, char *argv[])
{
        int c;
        char *p;
        int tapenext=0;
        int discnext=0;
        int romnext=-2;
        int parallelnext=0;
        int serialnext=0;
        int serialdebugnext=0;
        fileutils_get_executable_name(exedir,MAX_PATH_FILENAME_BUFFER_SIZE - 1);
        p = fileutils_get_filename(exedir);
        p[0]=0;
        discname[0]=discname2[0]=tapename[0]=0;
        parallelname[0]=0;serialname[0]=0;
        for (int i = 0; i < 16; i++)
        {
            romnames[i][0] = 0;
        }
        loadconfig();

        for (c=1;c<argc;c++)
        {
#ifndef WIN32
                if (!strcasecmp(argv[c],"--help"))
                {
                        printf("Elkulator v1.0 command line options :\n\n");
                        printf("-disc disc.ssd  - load disc.ssd into drives :0/:2\n");
                        printf("-disc1 disc.ssd - load disc.ssd into drives :1/:3\n");
                        printf("-tape tape.uef  - load tape.uef\n");
                        printf("-parallel file  - use file as a socket for parallel output\n");
                        printf("-serial file    - use file as a socket for serial communications\n");
                        printf("-serialdebug n  - set serial debugging output level to n\n");
                        printf("-rom number rom - load rom into the numbered bank\n");
                        printf("-debug          - start debugger\n");
                        exit(-1);
                }
                else
#endif
                if (!strcasecmp(argv[c],"-tape"))
                {
                        tapenext=2;
                }
                else if (!strcasecmp(argv[c],"-disc") || !strcasecmp(argv[c],"-disk"))
                {
                        discnext=1;
                }
                else if (!strcasecmp(argv[c],"-disc1"))
                {
                        discnext=2;
                }
                else if (!strcasecmp(argv[c],"-rom"))
                {
                        romnext=-1;
                }
                else if (!strcasecmp(argv[c],"-parallel"))
                {
                        parallelnext=1;
                }
                else if (!strcasecmp(argv[c],"-serial"))
                {
                        serialnext=1;
                }
                else if (!strcasecmp(argv[c],"-serialdebug"))
                {
                        serialdebugnext=1;
                }
                else if (!strcasecmp(argv[c],"-debug"))
                {
                        debug=debugon=1;
                }
                else if (tapenext)
                {
                   strcpy(tapename,argv[c]);
                }
                else if (discnext)
                {
                        if (discnext==2) strcpy(discname2,argv[c]);
                        else             strcpy(discname,argv[c]);
                        discnext=0;
                }
                else if (romnext > -2)
                {
                    if (romnext == -1)
                    {
                        romnext = atoi(argv[c]);
                    }
                    else if (romnext < 16) 
                    {
                        fprintf(stderr, "Loading %s in bank %d\n", argv[c], romnext);
                        strcpy(romnames[romnext],argv[c]);
                        romnext = -2;
                    }
                }
                else if (parallelnext)
                {
                        strcpy(parallelname,argv[c]);
                        parallelnext=0;
                }
                else if (serialnext)
                {
                        strcpy(serialname,argv[c]);
                        serialnext=0;
                }
                else if (serialdebugnext)
                {
                        serial_debug = atoi(argv[c]);
                        serialdebugnext=0;
                }
                if (tapenext) tapenext--;
        }

        initalmain(0,NULL);
        loadroms();
        reset6502();
        initula();
        resetula();
        reset1770();
        resetparallel();
        resetserial();
        
        loadtape(tapename);
        loaddisc(0,discname);
        loaddisc(1,discname2);
        /* For temporary compatibility: */
        if (romnames[0][0] != 0) loadcart(romnames[0]);
        if (romnames[1][0] != 0) loadcart2(romnames[1]);
        /* Load ROMs normally. */
        for (int i = 0; i < 16; i++) {
            if (romnames[i][0] != 0) loadrom_n(i, romnames[i]);
        }
        if (defaultwriteprot) writeprot[0]=writeprot[1]=1;

        video_init_part3(drawitint);

        inital();
        initsound();
        loaddiscsamps();
        maketapenoise();

        keyboard_makelayout();
        
        video_set_display_switch_mode_background();
}

int ddnoiseframes=0;
int oldbreak=0;
int resetit=0;
int runelkframe=0;

void runelk()
{
        int c;
        if (drawit || (tapeon && tapespeed))
        {
                if (drawit) drawit--;
                if (drawit>8 || drawit<0) drawit=0;
                for (c=0;c<312;c++) exec6502();
                if (runelkframe) exec6502();
                runelkframe=!runelkframe;
                if (resetit)
                {
                        memset(ram,0,32768);
                        resetula();
                        resetserial();
                        reset6502();
                        resetit=0;
                }
                if (break_pressed() && !oldbreak)
                {
                        reset6502();
                }
                oldbreak = break_pressed();
                if (wantloadstate) doloadstate();
                if (wantsavestate) dosavestate();
                if (infocus) video_poll_joystick();
                if (autoboot) autoboot--;
                ddnoiseframes++;
                if (ddnoiseframes>=5)
                {
                        ddnoiseframes=0;
                        mixddnoise();
                }
        }
        else
           video_rest(1);
}

void closeelk()
{
        stopmovie();
        saveconfig();
}
