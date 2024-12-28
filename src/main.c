/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * main.c
 * 
 * Main elkulator runtime initialisation and loop routines.
 * 
 */

/******************************************************************************
* Include files
*******************************************************************************/

#ifdef HAL_ALLEGRO_4
#include <allegro.h>
#else
#include <allegro5/allegro.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "1770.h"
#include "6502.h"
#include "callback_handlers.h"
#include "config.h"
#include "config_vars.h"
#include "csw.h"
#include "disc.h"
#include "ddnoise.h"
#include "debugger.h"
#include "elk.h"
#include "keyboard.h"
#include "logger.h"
#include "mem.h"
#include "tapenoise.h"
#include "ula.h"
#include "uef.h"

#include "host_abstraction_layer/event_handler.h"
#include "host_abstraction_layer/fileutils.h"
#include "host_abstraction_layer/sound.h"
#include "host_abstraction_layer/video.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

int autoboot;
FILE *rlog;

int drawit=0;

char exedir[MAX_PATH_FILENAME_BUFFER_SIZE];
char tapename[512];
char parallelname[512];
char serialname[512];
extern int serial_debug;
char romnames[16][1024];

callback_handlers_t callback_handlers;

int quited=0;
int infocus=1;

char ssname[260];
int fullscreen=0;

extern int wantloadstate;
extern int wantsavestate;

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

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

void drawitint()
{
    drawit++;
}

void initHandlers()
{
    log_debug("initHandlers");
    callback_handlers.handler_save_state    = dosavestate;
    callback_handlers.handler_load_state    = doloadstate;
    callback_handlers.handler_load_tape     = loadtape;
    callback_handlers.handler_load_disc0_2  = load_disc_0_2;
    callback_handlers.handler_load_disc1_3  = load_disc_1_3;
    callback_handlers.handler_load_cart1    = loadcart;
    callback_handlers.handler_load_cart2    = loadcart2;
    callback_handlers.handle_unload_carts   = unloadcart;
    callback_handlers.eject_tape            = handle_eject_tape;
    callback_handlers.rewind_tape           = handle_rewind_tape;
    callback_handlers.handle_screenshot     = savescrshot;
    callback_handlers.handle_startmovie     = startmovie;
    callback_handlers.handle_stopmovie      = stopmovie;
    callback_handlers.handle_enable_debugger= startdebug;
    callback_handlers.handle_key_down       = keyboard_keydown;
    callback_handlers.handle_key_up         = keyboard_keyup;
}

void initelk(int argc, char *argv[])
{
    int c;
    int tapenext=0;
    int discnext=0;
    int romnext=-2;
    int parallelnext=0;
    int serialnext=0;
    int serialdebugnext=0;
    fileutils_get_executable_name(exedir,MAX_PATH_FILENAME_BUFFER_SIZE - 1);
    #ifdef HAL_ALLEGRO_4
        // TODO: Tidy-up.
        char *p = fileutils_get_filename(exedir);
        p[0] = 0;
    #endif
    elkConfig.disc.discname[0]  = 0;
    elkConfig.disc.discname2[0] = 0;
    tapename[0] = 0;
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
            printf("Elkulator v2.0 command line options :\n\n");
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
            if (discnext==2) strcpy(elkConfig.disc.discname2,argv[c]);
            else             strcpy(elkConfig.disc.discname,argv[c]);
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

    sound_init_part1(0,NULL);
    loadroms();
    reset6502();
    initula();
    resetula();
    reset1770();
    #ifndef WIN32
        resetparallel();
        resetserial();
    #endif // WIN32
    
    loadtape(tapename);
    loaddisc(0,elkConfig.disc.discname);
    loaddisc(1,elkConfig.disc.discname2);
    /* For temporary compatibility: */
    if (romnames[0][0] != 0) loadcart(romnames[0]);
    if (romnames[1][0] != 0) loadcart2(romnames[1]);
    /* Load ROMs normally. */
    for (int i = 0; i < 16; i++) {
        if (romnames[i][0] != 0) loadrom_n(i, romnames[i]);
    }
    if (elkConfig.disc.defaultwriteprot) writeprot[0]=writeprot[1]=1;

    video_init_part3(drawitint);

    sound_init_part2();
    initsound();
    loaddiscsamps();
    maketapenoise();

    makekeyl();
    
    video_set_display_switch_mode_background();
}

int ddnoiseframes=0;
bool oldbreak=false;
int resetit=0;
int runelkframe=0;
elkstate_t elk_state = ELK_STATE_INITIALIZING;


native_timediff_t runelk()
{       
    native_timestamp_t timestamp_start = log_get_timestamp();
    native_timediff_t  timestamp_diff  = 0;
    int c;
    //log_time_mark("=== runelk begin ===");

    if (drawit) drawit--;
    if (drawit>8 || drawit<0) drawit=0;
    for (c=0;c<312;c++) exec6502();
    if (runelkframe) exec6502();
    runelkframe=!runelkframe;
    if (resetit)
    {
        memset(ram,0,SIZE_32K);
        resetula();
        #ifndef WIN32
            resetserial();
        #endif // WIN32
        reset6502();
        resetit=0;
    }
    if (break_pressed() && !oldbreak)
    {
        reset6502();
    }
    oldbreak = break_pressed();
    if (wantloadstate) doloadstate(ssname);
    if (wantsavestate) dosavestate(ssname);
    if (infocus) video_poll_joystick();
    if (autoboot) autoboot--;
    ddnoiseframes++;
    if (ddnoiseframes>=5)
    {
        ddnoiseframes=0;
        mixddnoise();
    }

    // Record how long elkrun took (this will allow the calling
    // function to make adjustments if this function took too
    // long (e.g. rendering took longer than expected)
    timestamp_diff = log_get_timestamp() - timestamp_start;

    return timestamp_diff;
}

void closeelk()
{
    stopmovie();
    saveconfig();
}

void pauseelk()
{
    video_stop_timer();
    elk_state = ELK_STATE_PAUSED;
}

void resumeelk()
{
    video_start_timer();
    elk_state = ELK_STATE_RUNNING;
}

void native_window_close_button_handler(void)
{
    quited = 1;
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

int main(int argc, char *argv[])
{
    int count = 0;
    //init_config(); TODO: May need this not sure.
    log_msg(__FUNCTION__, "Elkulator has started");
    int ret = video_init_part1();
    if (ret != 0)
    {
        fprintf(stderr, "Error %d initializing Allegro.\n", ret);
        exit(-1);
    }
    initHandlers();
    initelk(argc,argv);
    video_register_close_button_handler(native_window_close_button_handler);
    
    log_config_vars();
    #ifdef HAL_ALLEGRO_4 
        while (!quited)
        {
            if (drawit || (is_tapeon() && elkConfig.tape.speed))
            {
                runelk();
            }
            else
            {
                video_rest(1);
            }

            if (menu_pressed())
            {
                entergui();
            } 
        }
    #else       
        resumeelk();
        char elk_timediff_str[28];
        char elk_cumulated_timediff_str[28];
        elk_event_t elkEvent = 0;
        native_timediff_t elk_runtime = 0;
        native_timediff_t native_timer_diff = 0;
        native_timediff_t native_cummulative_time_diff = 0;
        native_timestamp_t native_timestamp_last_trigger = log_get_timestamp();
        native_timestamp_t native_timestamp_current = 0;
        while (!(elkEvent & ELK_EVENT_EXIT))
        {
            elkEvent = event_await();

            if(elkEvent & ELK_EVENT_RESET)
            {
                resetit = 1;
                log_config_vars();
            }

            //log_debug("elkEvent=%04x", elkEvent);
            if(elkEvent & ELK_EVENT_TIMER_TRIGGERED) 
            {
                native_timestamp_current = log_get_timestamp();
                native_timer_diff = native_timestamp_current - native_timestamp_last_trigger;
                native_timestamp_last_trigger = native_timestamp_current;
                native_cummulative_time_diff = native_cumulative_time_adjust(20000, native_cummulative_time_diff, native_timer_diff);

                //drawit++;
                if(native_cummulative_time_diff > 20000)
                {
                    pause_video_blit();
                    // Print out some stats for debug purposes.
                    native_timediff_sprintf(elk_timediff_str, sizeof(elk_timediff_str), elk_runtime);
                    native_timediff_sprintf(elk_cumulated_timediff_str, sizeof(elk_cumulated_timediff_str), native_cummulative_time_diff);
                    log_debug("elkruntime = %s (%s)", elk_timediff_str, elk_cumulated_timediff_str);
                }
                elk_runtime = runelk();

                // If tape is running and its speed is fast or really fast
                // We need to runelk another 19 times (or until tape is
                // stopped, this maintains the fast loading that allegro4
                // does as it runs the function every 1 millisecond with
                // drawing every normal 20ms).
                count = 19;
                bool skip_video_refresh = false;
                while(count && is_tapeon() && (is_csw() || is_uef()) && elkConfig.tape.speed)
                {
                    if(elk_runtime > 20000 && !skip_video_refresh)
                    {
                        log_debug("!!tape skip video refresh disabled!!");
                        skip_video_refresh = true;
                        pause_video_blit(); // We don't need to update the screen for this (helps on slower machines).
                    }
                    elk_runtime += runelk(skip_video_refresh);
                    count--;
                }
                resume_video_blit();
            }
            else if(elkEvent & ELK_EVENT_HANDLED)
            {
                // Menu may have been accessed, reset timing.
                native_timestamp_last_trigger = log_get_timestamp();
            }
        }
    #endif // HAL_ALLEGRO_4
    closeelk();
    log_msg(__FUNCTION__, "Elkulator has ended");
    return 0;
}

#ifdef HAL_ALLEGRO_4
END_OF_MAIN();
#endif // HAL_ALLEGRO_4