/*Elkulator v1.0 by Tom walker
  Initialisation/Closing/Main loop*/
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elk.h"
#include "hal/hal.h"

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

/*int waiting,waiting2;

void waitforthread()
{
        return;
        waiting=1;
        while (!waiting2)
              sleep(10);
}

void stopwaiting()
{
        waiting=0;
}*/

int drawit=0;
void drawitint()
{
        drawit++;
}

void cleardrawit()
{
        drawit=0;
}

/*void wait50()
{
//        if (!infocus) drawit=1;
        if (tapeon && tapespeed) drawit=1;
        while (!drawit || !infocus)
        {
                sleep(1);
//                if (waiting) return;
        }
        drawit--;
}*/

char exedir[512];
char tapename[512];
char parallelname[512];
char serialname[512];
extern int serial_debug;
char romnames[16][1024];

void initelk(int argc, char *argv[])
{
        int c;
        char *p;
        int tapenext=0,discnext=0,romnext=-2,parallelnext=0,serialnext=0,serialdebugnext=0;
        get_executable_name(exedir,511);
        p=get_filename(exedir);
        p[0]=0;
        discname[0]=discname2[0]=tapename[0]=0;
        parallelname[0]=0;serialname[0]=0;
        for (int i = 0; i < 16; i++)
            romnames[i][0] = 0;
//        printf("Load config\n");
        loadconfig();
//printf("commandline\n");

        for (c=1;c<argc;c++)
        {
//printf("%i\n",c); fflush(stdout);
//                printf("%i : %s\n",c,argv[c]);
/*                if (!strcasecmp(argv[c],"-1770"))
                {
                        I8271=0;
                        WD1770=1;
                }
                else*/
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
                   strcpy(tapename,argv[c]);
                else if (discnext)
                {
                        if (discnext==2) strcpy(discname2,argv[c]);
                        else             strcpy(discname,argv[c]);
                        discnext=0;
                }
                else if (romnext > -2)
                {
                    if (romnext == -1)
                        romnext = atoi(argv[c]);
                    else if (romnext < 16) {
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
//printf("initalmain\n"); fflush(stdout);

        initalmain(0,NULL);
//printf("loadroms\n"); fflush(stdout);
        loadroms();
//printf("reset6502\n");
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
#ifndef WIN32
        install_keyboard();
#endif
        install_timer();
        install_int_ex(drawitint,MSEC_TO_TIMER(20));
        install_joystick(JOY_TYPE_AUTODETECT);
        inital();
        initsound();
        loaddiscsamps();
        maketapenoise();

        makekeyl();
        
        set_display_switch_mode(SWITCH_BACKGROUND);
        
//        initresid();
//        resetsid();
//        setsidtype(0,0);
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
                if (infocus) poll_joystick();
                if (autoboot) autoboot--;
                ddnoiseframes++;
                if (ddnoiseframes>=5)
                {
                        ddnoiseframes=0;
                        mixddnoise();
                }
        }
        else
           rest(1);
}

void closeelk()
{
        stopmovie();
        saveconfig();
//        dumpram();
//        dumpregs();
//        printf("Closing!\n");
}
