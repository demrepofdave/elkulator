/*Elkulator v1.0 by Tom walker
  Initialisation/Closing/Main loop*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "elk.h"
#include "hal/hal.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

//#undef printf

int autoboot;
int quited=0;
bool autopause = false;
bool autoskip = true;

extern int fullscreen;

static ALLEGRO_TIMER *timer;
ALLEGRO_EVENT_QUEUE *queue;
static ALLEGRO_EVENT_SOURCE evsrc;

ALLEGRO_DISPLAY *video_init(void);

ALLEGRO_DISPLAY *tmp_display;

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

char exedir[MAX_PATH_FILENAME_BUFFER_SIZE];
//ALLEGRO_PATH *exedir;
char tapename[512];
char parallelname[512];
char serialname[512];
extern int serial_debug;
char romnames[16][1024];

#define log_debug
#define log_fatal



static double main_calc_timer(int speed)
{
    double secs = ((double)speed / 2000000.0);
    printf("main: main_calc_timer is %gs\n", secs);
    return secs;
}

void initelk(int argc, char *argv[])
{
        ALLEGRO_DISPLAY *display;
        const char *ext;
        
        int c;
        char *p;
        int tapenext=0;
        int discnext=0;
        int romnext=-2;
        int parallelnext=0;
        int serialnext=0;
        int serialdebugnext=0;

        // Initialise allegro
        bool allegro_ok = al_init();

        if(allegro_ok)
        {
                allegro_ok = al_init_native_dialog_addon();
        }

        if(allegro_ok)
        {
                allegro_ok = al_init_primitives_addon();
        }

        if(allegro_ok)
        {
                allegro_ok = al_install_keyboard();
        }

        allegro_ok = al_init_image_addon();

        if(allegro_ok)
        {
                printf("Allegro initialisation is ok\n");
        }
        else
        {
                printf("Allegro initialisation failed!\n");
        }


        //hal_get_executable_name(exedir,MAX_PATH_FILENAME_BUFFER_SIZE - 1);

        ALLEGRO_PATH *alpath = al_get_standard_path(ALLEGRO_EXENAME_PATH);
        const char *tmpstrdir = al_path_cstr(alpath, '/');
        strcpy(exedir, "./");
        printf("Allegro path = %s\n", (char*) exedir);
        fflush(stdout);

        display = video_init();
        tmp_display = display;
    
        if (!(queue = al_create_event_queue())) 
        {
                log_fatal("main: unable to create event queue");
                exit(1);
        }
        al_register_event_source(queue, al_get_display_event_source(display));

        //p=hal_get_filename(exedir);

        //p[0]=0;
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
                        if (discnext==2)
                        {
                                strcpy(discname2,argv[c]);
                        } 
                        else             
                        {
                                strcpy(discname,argv[c]);
                        }
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
                if (tapenext)
                {
                        tapenext--;
                }
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
        if (romnames[0][0] != 0) 
        {
                loadcart(romnames[0]);
        }
        if (romnames[1][0] != 0) 
        {
                loadcart2(romnames[1]);
        }

        /* Load ROMs normally. */
        for (int i = 0; i < 16; i++)
        {
            if (romnames[i][0] != 0) loadrom_n(i, romnames[i]);
        }
        if (defaultwriteprot)
        {
                writeprot[0]=writeprot[1]=1;
        }
#ifndef WIN32
        hal_install_keyboard();
#endif
        inital();
        initsound();
        loaddiscsamps();
        maketapenoise();

        //makekeyl();
        
 //   if (!al_install_audio()) {
 //       log_fatal("main: unable to initialise audio");
 //       exit(1);
 //   }
 //   if (!al_reserve_samples(3)) {
 //       log_fatal("main: unable to reserve audio samples");
 //       exit(1);
 //   }
 //   if (!al_init_acodec_addon()) {
 //       log_fatal("main: unable to initialise audio codecs");
 //       exit(1);
 //   }

        //tapenoise_init(queue);

        //main_reset();

        //joystick_init(queue);

        tmp_display = display;

        printf("display=%p\n", (void*) display);
        gui_allegro_init(queue, display);

        if (!(timer = al_create_timer(main_calc_timer(10000)))) {
            log_fatal("main: unable to create timer");
            exit(1);
        }
        al_register_event_source(queue, al_get_timer_event_source(timer));
        al_init_user_event_source(&evsrc);
        al_register_event_source(queue, &evsrc);

        al_register_event_source(queue, al_get_keyboard_event_source());

        al_install_mouse();
        al_register_event_source(queue, al_get_mouse_event_source());

        // lovebug
        //if (fullscreen)
        //    video_enterfullscreen();
        // lovebug end
      
}

int ddnoiseframes=0;
int oldbreak=0;
int resetit=0;
int runelkframe=0;

void runelk(ALLEGRO_EVENT *event)
{
        int c;
        drawit++;
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
//                if (break_pressed() && !oldbreak)
//                {
//                        reset6502();
//                }
                //oldbreak = break_pressed();
                if (wantloadstate) doloadstate();
                if (wantsavestate) dosavestate();
                if (infocus) hal_poll_joystick();
                if (autoboot) autoboot--;
                ddnoiseframes++;
                if (ddnoiseframes>=5)
                {
                        ddnoiseframes=0;
                        mixddnoise();
                }
        }

//        if (fullspeed == FSPEED_RUNNING) {
//        ALLEGRO_EVENT event;
//        event.type = ALLEGRO_EVENT_TIMER;
//        al_emit_user_event(&evsrc, &event, NULL);
//        }
}

void closeelk()
{
        stopmovie();
        saveconfig();
}

void main_init(int argc, char *argv[])
{

}

//static double last_switch_in = 0.0;

void main_run()
{
    ALLEGRO_EVENT event;

    log_debug("main: about to start timer");
    
    al_start_timer(timer);

    log_debug("main: entering main loop");
    while (!quited)
    {
        al_wait_for_event(queue, &event);

        //printf("Event %d\n", (int) event.type);
        switch(event.type)
        {
            case ALLEGRO_EVENT_KEY_DOWN:
//                if (!keydefining)
//                    key_down_event(&event);
                break;
            case ALLEGRO_EVENT_KEY_CHAR:
//                if (!keydefining)
//                    key_char_event(&event);
                break;
            case ALLEGRO_EVENT_KEY_UP:
//                if (!keydefining)
//                    key_up_event(&event);
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
//                mouse_axes(&event);
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
//                log_debug("main: mouse button down");
//                mouse_btn_down(&event);
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
//                log_debug("main: mouse button up");
//                mouse_btn_up(&event);
                break;
            case ALLEGRO_EVENT_JOYSTICK_AXIS:
//                joystick_axis(&event);
                break;
            case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
//                joystick_button_down(&event);
                break;
            case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
//                joystick_button_up(&event);
                break;
            case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
//                joystick_rescan_sticks();
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                log_debug("main: event display close - quitting");
                quited = true;
                break;
            case ALLEGRO_EVENT_TIMER:
                runelk(&event);
                break;
            case ALLEGRO_EVENT_MENU_CLICK:
                main_pause("menu active");
                gui_allegro_event(&event);
 //               main_resume();
                break;
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
//                video_update_window_size(&event);
                break;
            case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
                /* bodge for when OUT events immediately follow an IN event */
//                if ((event.any.timestamp - last_switch_in) > 0.01) {
//                    key_lost_focus();
//                    if (autopause) //  && !debug_core)
//                        main_pause("auto-paused");
//                }
                break;
            case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
//                last_switch_in = event.any.timestamp;
//                if (autopause)
//                    main_resume();
                break;
        }
    }
    log_debug("main: end loop");
}

void main_pause(const char *why)
{
    char buf[120];
    //snprintf(buf, sizeof(buf), "%s (%s)", VERSION_STR, why);
    //al_set_window_title(tmp_display, buf);
    al_stop_timer(timer);
}

void main_resume(void)
{
//    if (emuspeed != EMU_SPEED_PAUSED && emuspeed != EMU_SPEED_FULL)
        al_start_timer(timer);
}

//int main(int argc, char **argv)
//{
//    main_init(argc, argv);
//    main_run();
//    main_close();
//    return 0;
//}

int main(int argc, char *argv[])
{
        printf("Entering Elkulator!\n");
        hal_result result = hal_init();

        if (result != HAL_OK)
        {
                fprintf(stderr, "Error %d initializing HAL (Allegro).\n", result); // TODO: Pass back actual allegro error information.
                exit(-1);
        }
        initelk(argc, argv);
        hal_set_window_title("Elkulator v1.0 (Press MENU, next to R-Ctrl, for menu)");
        //hal_install_mouse();
        main_run();
        closeelk();
        return 0;
}

// End of file.