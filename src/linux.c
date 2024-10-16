/*Elkulator v1.0 by Sarah Walker
  Linux main loop*/

#ifdef HAL_ALLEGRO_4
#include <allegro.h>
#else
#include "common/event_handler.h"
#endif

#include <stdlib.h>
#include "elk.h"
#include "logger.h"
#include "callback_handlers.h"
#include "common/video.h"

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

int main(int argc, char *argv[])
{
        //init_config(); TODO: May need this not sure.
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
                        if(elkEvent & ELK_EVENT_TIMER_TRIGGERED) 
                        {
                                drawit++;
                        }
                        if(elkEvent & ELK_EVENT_RESET)
                        {
                                resetit = 1;
                                log_config_vars();
                        }
                        runelk();
                }
        #endif // HAL_ALLEGRO_4
        closeelk();

        return 0;
}

#ifdef HAL_ALLEGRO_4
END_OF_MAIN();
#endif // HAL_ALLEGRO_4

