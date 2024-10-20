/*Elkulator v1.0 by Sarah Walker
  Linux main loop*/

#ifdef HAL_ALLEGRO_4
#include <allegro.h>
#else
#include "common/event_handler.h"
#endif

#include <stdlib.h>
#include "elk.h"
#include "config.h"
#include "logger.h"
#include "callback_handlers.h"
#include "config_vars.h"
#include "common/video.h"
#include "ula.h"

char ssname[260];
char scrshotname[260];
char moviename[260];

int fullscreen=0;
int gotofullscreen=0;
int wantloadstate=0,wantsavestate=0;

int quited=0;
int infocus=1;

extern int drawit;

void native_window_close_button_handler(void)
{
       quited = 1;
}

int ms_tick = 0;

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
                        runelk();
                        if (menu_pressed()) entergui();
                }
        #else       
                video_start_timer();
                elk_event_t elkEvent = 0;
                while (!(elkEvent & ELK_EVENT_EXIT))
                {
                        elkEvent = event_await();
                        //log_debug("elkEvent=%04x", elkEvent);
                        if(elkEvent & ELK_EVENT_TIMER_TRIGGERED) 
                        {
                                //ms_tick +=2;
                                //if(ms_tick >= 20)
                                //{
                                    drawit++;
                                //    ms_tick = 0;
                                //}
                        }
                        if(elkEvent & ELK_EVENT_RESET)
                        {
                                resetit = 1;
                                log_config_vars();
                        }

                        runelk();

                        // If tape is running and its speed is fast or really fast
                        // We need to runelk another 19 times (or until tape is
                        // stopped, this maintains the fast loading that allegro4
                        // does as it runs the function every 1 millisecond with
                        // drawing every normal 20ms).
                        count = 19;
                        while(count && tapeon && elkConfig.tape.speed)
                        {
                                runelk();
                                count--;
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

