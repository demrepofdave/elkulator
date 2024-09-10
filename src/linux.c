/*Elkulator v1.0 by Sarah Walker
  Linux main loop*/
#ifndef WIN32

#include <stdlib.h>
#include "elk.h"
#include "hal/hal.h"
#include "hal/hal_end_of_main.h"

char ssname[260];
char scrshotname[260];
char moviename[260];

int fullscreen=0;
int gotofullscreen=0;
int videoresize=0;
int wantloadstate=0,wantsavestate=0;
int winsizex=640,winsizey=512;

int plus3=0;
int dfsena=0,adfsena=0;
int turbo=0;
int mrb=0,mrbmode=0;
int ulamode=0;
int drawmode=0;

char discname[260];
char discname2[260];
int quited=0;
int infocus=1;

#ifndef HAL_ALLEGRO_5
void native_window_close_button_handler(void)
{
       quited = 1;
}
#endif

void startblit()
{
}
void endblit()
{
}

int keylookup[128];

#ifdef HAL_ALLEGRO_5

void main_init(int argc, char *argv[])
{
    ALLEGRO_DISPLAY *display;
    const char *ext;

    hal_set_new_window_title(VERSION_STR);

    key_init();
    config_load();

    display = video_init();
    
    if (!(queue = al_create_event_queue())) 
    {
        log_fatal("main: unable to create event queue");
        exit(1);
    }
    al_register_event_source(queue, al_get_display_event_source(display));

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

    tapenoise_init(queue);

    main_reset();

    joystick_init(queue);

    tmp_display = display;

    gui_allegro_init(queue, display);

    if (!(timer = al_create_timer(main_calc_timer(emu_speed_normal)))) {
        log_fatal("main: unable to create timer");
        exit(1);
    }
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_init_user_event_source(&evsrc);
    al_register_event_source(queue, &evsrc);

    al_register_event_source(queue, al_get_keyboard_event_source());

    oldmodel = curmodel;

    al_install_mouse();
    al_register_event_source(queue, al_get_mouse_event_source());

    // lovebug
    if (fullscreen)
        video_enterfullscreen();
    // lovebug end
}

void main_run()
{
    ALLEGRO_EVENT event;

    log_debug("main: about to start timer");
    al_start_timer(timer);

    log_debug("main: entering main loop");
    while (!quitting) {
        al_wait_for_event(queue, &event);
        switch(event.type) {
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
                quitting = true;
                break;
            case ALLEGRO_EVENT_TIMER:
                main_timer(&event);
                break;
            case ALLEGRO_EVENT_MENU_CLICK:
                main_pause("menu active");
                gui_allegro_event(&event);
                main_resume();
                break;
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
//                video_update_window_size(&event);
                break;
            case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
                /* bodge for when OUT events immediately follow an IN event */
                if ((event.any.timestamp - last_switch_in) > 0.01) {
                    key_lost_focus();
                    if (autopause && !debug_core)
                        main_pause("auto-paused");
                }
                break;
            case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
                last_switch_in = event.any.timestamp;
                if (autopause)
                    main_resume();
        }
    }
    log_debug("main: end loop");
}

int main(int argc, char **argv)
{
    main_init(argc, argv);
    main_run();
    main_close();
    return 0;
}
#else
int main(int argc, char *argv[])
{
        hal_result result = hal_init();

        if (result != HAL_OK)
        {
                fprintf(stderr, "Error %d initializing HAL (Allegro).\n", result); // TODO: Pass back actual allegro error information.
                exit(-1);
        }
        initelk(argc,argv);
        hal_set_window_title("Elkulator v1.0 (Press MENU, next to R-Ctrl, for menu)");
#ifndef HAL_ALLEGRO_5
        hal_set_close_button_callback(native_window_close_button_handler);
#endif
        hal_install_mouse();
        while (!quited)
        {
                runelk();
                if (menu_pressed()) entergui();
        }
        closeelk();
        return 0;
}

END_OF_MAIN();
#endif // HAL_ALLEGRO_5
#endif // WIN32
