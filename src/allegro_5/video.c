/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * video.c
 * 
 * Video abstration layer.
 * 
 * Allows actual graphics libraries used for the emulation to be abstracted 
 * from the actual electron code.
 * 
 * This allows easier porting to different graphics and sound libraries in 
 * future in order to allow maximum cross platform support and long term
 * durability.
 *
 * This is the allegro 5 implementation of the abstraction layer.
 *
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "common/video.h"
#include "allegro_5/menu_internal.h"
#include "logger.h"
#include "video_internal.h"
#include "event_handler_internal.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

#define VERSION_STR "Elkulator v2.09a"

/******************************************************************************
* Typedefs
*******************************************************************************/

typedef uint32_t elk_pallete_t;

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

ALLEGRO_BITMAP *b             = NULL;    // Main bitmap used before blitting to window screen.
ALLEGRO_BITMAP *b16           = NULL;  // Intermediate bitmap 1
ALLEGRO_BITMAP *b162          = NULL; // Intermediate bitmap 2
ALLEGRO_BITMAP *vidb          = NULL; // Windows bitmap
ALLEGRO_BITMAP *vp1           = NULL;  // Windows bitmap 1?
ALLEGRO_BITMAP *vp2           = NULL;  // Windows bitmap 2?
ALLEGRO_BITMAP *bm_screenshot = NULL; // USed for screenshots.

static ALLEGRO_DISPLAY *display;

ALLEGRO_LOCKED_REGION *region = NULL; // Region lock on bitmap b (to allow writing of pixels)

static ALLEGRO_TIMER *timer;
static ALLEGRO_EVENT_SOURCE evsrc;

elk_pallete_t elkpal[8] =
{
    0xff000000,
    0xffff0000,
    0xff00ff00,
    0xffffff00,
    0xff0000ff,
    0xffff00ff,
    0xffffff00,
    0xffffffff
};

windowCoords_t main_window;

/******************************************************************************
* Function Prototypes
*******************************************************************************/


/******************************************************************************
* Private Function Definitions
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

// Called from linux.c (main)
int video_init_part1()
{
    if (!al_init())
    {
        fprintf(stderr, "Error initializing Allegro.\n");
        exit(-1);
    }

    main_window.winsizex = 800; // TODO: Will be configured in future
    main_window.winsizey = 600; // TODO: Will be configured in future
    main_window.maintain_aspect = true;

    al_init_native_dialog_addon();
    al_set_new_window_title(VERSION_STR);
    al_init_primitives_addon();

    if (!al_install_keyboard())
    {
        log_fatal("main: unable to install keyboard");
        exit(1);
    }

#ifdef ALLEGRO_GTK_TOPLEVEL
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_GTK_TOPLEVEL | ALLEGRO_RESIZABLE);
#else
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
#endif
    int vsync = -1; //get_config_int("video", "allegro_vsync", -1);
    if (vsync >= 0) 
    {
        int temp;
        al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
        log_debug("video: config vsync=%d, actual=%d", vsync, al_get_new_display_option(ALLEGRO_VSYNC, &temp));
    }
    //video_set_window_size(true);

    if ((display = al_create_display(main_window.winsizex, main_window.winsizey)) == NULL) {
        log_fatal("video: unable to create display");
        exit(1);
    }

    log_debug("Display = %p\n", display);
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP|ALLEGRO_NO_PRESERVE_TEXTURE);

    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    b16 = al_create_bitmap(800*2,600);
    b162= al_create_bitmap(640,256);
    al_set_target_bitmap(b16);
    al_clear_to_color(black);

    //colblack = 0xff000000;
    //colwhite = 0xffffffff;
    // border_col = al_map_rgb(0, 0, 0);

    b = al_create_bitmap(640, 616);
    al_set_target_bitmap(b);
    al_clear_to_color(al_map_rgb(0, 0,0));
    region = al_lock_bitmap(b, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

    al_init_image_addon();

    if (!event_init_queue())
    {
        log_fatal("main: unable to create event queue");
        exit(1);
    }

    event_register_event_source(al_get_display_event_source(display));

    if (!al_install_audio()) {
        log_fatal("main: unable to initialise audio");
        exit(1);
    }
    if (!al_reserve_samples(3)) {
        log_fatal("main: unable to reserve audio samples");
        exit(1);
    }
    if (!al_init_acodec_addon()) {
        log_fatal("main: unable to initialise audio codecs");
        exit(1);
    }


    return 0;
}

void video_init_part2()
{
    //ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    //b = al_create_bitmap(640,616);

    log_debug("display = %p\n", display);
    menu_init(display);

    if (!(timer = al_create_timer(0.02)))
    {
        log_fatal("main: unable to create timer");
        exit(1);
    }
    event_register_event_source(al_get_timer_event_source(timer));
    al_init_user_event_source(&evsrc);
    event_register_event_source(&evsrc);

    event_register_event_source(al_get_keyboard_event_source());

    al_install_mouse();
    event_register_event_source(al_get_mouse_event_source());
}

// Called from main.c (initelk)
void video_init_part3(void (*timer_function)(void))
{
    // Nothing to do.
}

void video_rest(unsigned int period)
{
    return;
}

void video_set_window_size(int w, int h, int v_w, int v_h)
{
    main_window.winsizex = w;
    main_window.winsizey = h;
}


void video_set_gfx_mode_windowed()
{
    ALLEGRO_DISPLAY *display;
    display = al_get_current_display();
    al_resize_display(display, main_window.winsizex,  main_window.winsizey);
    al_set_display_flag(display, ALLEGRO_MAXIMIZED, false);
    al_set_display_flag(display, ALLEGRO_FRAMELESS, false);
    al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, false);
//    al_set_display_flag
//    set_gfx_mode(GFX_AUTODETECT_WINDOWED, w, h, v_w, v_h);
}

void video_register_close_button_handler(void (*handler_function)(void))
{
    // Nothing to do with allegro 5.
}

int video_set_display_switch_mode_background()
{
    return 0;
}

int video_poll_joystick()
{
    return 0;
}

void video_set_gfx_mode_fullscreen()
{
    ALLEGRO_DISPLAY *display = al_get_current_display();
    //save_winsizex = al_get_display_width(display);
    //save_winsizey = al_get_display_height(display);
    if (al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, true)) 
    {
//#ifdef WIN32
//        al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, false);
//        al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, true);
//#endif
    }
}

void video_set_depth_and_elk_palette()
{
    //set_color_depth(8); // TODO: May not need this in allegro5.
    //set_palette(elkpal);  - Does not exist in allegro5
}

void video_set_desktop_color_depth()
{
    //set_color_depth(desktop_color_depth()); // TODO: may not need this in allegro5
}

int video_get_desktop_color_depth()
{
    return(8); // TODO: probably won't need this in allegro5
}

void video_put_pixel(int y, int x, uint8_t color)
{
    *((uint32_t *)((char *)region->data + region->pitch * y + x * region->pixel_size)) = elkpal[color];
}

void video_put_pixel_line(int y, int x, int width, uint8_t color)
{
    int count = width;

    char *ptr = (char *)region->data + region->pitch * y + x * region->pixel_size;
    while (count--) {
        *(uint32_t *)ptr = elkpal[color];
        ptr += region->pixel_size;
    }
}

//#ifdef WIN32
//CRITICAL_SECTION cs;
//#endif

void startblit()
{
//    #ifdef WIN32
//        EnterCriticalSection(&cs);
//    #endif
}

void endblit()
{
//    #ifdef WIN32
//        LeaveCriticalSection(&cs);
//    #endif
}

void video_blit_to_screen(int drawMode, int colDepth)
{
    int c;
    //int firstx = 0;
    //int firsty = 0;
    //int scr_x_start = 0;
    //int scr_y_start = 0;
    //int scr_x_size  = 640;
    //int scr_y_size  = 512;

    //int xsize = lastx - firstx;
    //int ysize = lasty - firsty + 1;
    //int xsize = 640;
    //int ysize = 512;

    startblit();
    switch (drawMode)
    {
        case SCANLINES:
            al_unlock_bitmap(b);
            al_set_target_backbuffer(al_get_current_display());
            al_draw_scaled_bitmap(b, 0,0,640,512, 0,0,640,512, 0);
            //blit(b,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            //al_draw_bitmap(b, (winsizeX-640)/2,(winsizeY-512)/2,0);
            //al_draw_bitmap(b, 0,0,0);
            break;

/*        case LINEDBL:
            #ifdef WIN32
                blit(b,vidb,0,0,0,0,640,256);
                if (elkConfig.display.videoresize)
                {
                    stretch_blit(vidb,screen,0,0,640,256,0,0,winsizex,winsizey);
                }
                else
                {
                    stretch_blit(vidb,screen,0,0,640,256,(winsizex-640)/2,(winsizey-512)/2,640,512);
                }
            #else
                for (c=0;c<512;c++)
                {
//                    blit(b,b16,0,c>>1,0,c,640,1);
                }
                al_set_target_backbuffer(al_get_current_display());
                //al_draw_scaled_bitmap(b16, firstx, firsty, xsize, ysize, scr_x_start, scr_y_start, scr_x_size, scr_y_size, 0);
                al_draw_bitmap(b16, (winsizeX-640)/2,(winsizeY-512)/2);
            #endif
            break;

        case _2XSAI:
            blit(b,b162,0,0,0,0,640,256);
            Super2xSaI(b162,b16,0,0,0,0,320,256);
            al_set_target_backbuffer(al_get_current_display());
            //al_draw_scaled_bitmap(b16, firstx, firsty, xsize, ysize, scr_x_start, scr_y_start, scr_x_size, scr_y_size, 0);
            al_draw_bitmap(b16, (winsizeX-640)/2,(winsizeY-512)/2);
            //blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case SCALE2X:
            blit(b,b162,0,0,0,0,640,256);
            scale2x(b162,b16,320,256);
            al_set_target_backbuffer(al_get_current_display());
            //al_draw_scaled_bitmap(b16, firstx, firsty, xsize, ysize, scr_x_start, scr_y_start, scr_x_size, scr_y_size, 0);
            al_draw_bitmap(b16, (winsizeX-640)/2,(winsizeY-512)/2);
            //blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case EAGLE:
            blit(b,b162,0,0,0,0,640,256);
            SuperEagle(b162,b16,0,0,0,0,320,256);
            al_set_target_backbuffer(al_get_current_display());
            //al_draw_scaled_bitmap(b16, firstx, firsty, xsize, ysize, scr_x_start, scr_y_start, scr_x_size, scr_y_size, 0);
            al_draw_bitmap(b16, (winsizeX-640)/2,(winsizeY-512)/2);
            //blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case PAL: // TODO: Not currently working (blank screen)
            palfilter(b,b16,colDepth);
            al_set_target_backbuffer(al_get_current_display());
            //al_draw_scaled_bitmap(b16, firstx, firsty, xsize, ysize, scr_x_start, scr_y_start, scr_x_size, scr_y_size, 0);
            //blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            al_draw_bitmap(b16, (winsizeX-640)/2,(winsizeY-512)/2);
            break;*/
    }
    region = al_lock_bitmap(b, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
    al_flip_display();
    endblit();
}

void video_capture_screenshot(int drawMode, int colDepth)
{
    int firstx = 0;
    int firsty = 0;
    int scr_x_start = 0;
    int scr_y_start = 0;
    int scr_x_size  = 640;
    int scr_y_size  = 512;

    //int xsize = lastx - firstx;
    //int ysize = lasty - firsty + 1;
    int xsize = 640;
    int ysize = 512;

    video_set_desktop_color_depth();
    bm_screenshot = al_create_bitmap(640,512);
    switch (drawMode)
    {
        case SCANLINES:
            al_unlock_bitmap(b);
            al_set_target_bitmap(bm_screenshot);
            al_draw_bitmap(b16, 0,0,0);
//            blit(b,bm_screenshot,0,0,0,0,640,512);
            break;

        case LINEDBL:
//            #ifdef WIN32
//                stretch_blit(vidb,tb,0,0,640,256,0,0,640,512);
//            #else
                al_unlock_bitmap(b);
                al_set_target_bitmap(bm_screenshot);
                al_draw_bitmap(b16, 0,0,0);
//                blit(b16,bm_screenshot,0,0,0,0,640,512);
//            #endif // WIN32
            break;

/*        case _2XSAI:
            blit(b,b162,0,0,0,0,640,256);
            Super2xSaI(b162,b16,0,0,0,0,320,256);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case SCALE2X:
            blit(b,b162,0,0,0,0,640,256);
            scale2x(b162,b16,320,256);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case EAGLE:
            blit(b,b162,0,0,0,0,640,256);
            SuperEagle(b162,b16,0,0,0,0,320,256);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case PAL:
            palfilter(b,b16,colDepth);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break; */
    }
    //set_color_depth(8); // TODO: May not need this in allegro5.
}

int video_save_bmp(const char * filename)
{
    return(al_save_bitmap(filename, bm_screenshot));
}

void video_destroy_screenshot()
{
    if(bm_screenshot)
    {
        al_destroy_bitmap(bm_screenshot);
        bm_screenshot = NULL;
    }
}
                                                        
void video_clearall()
{
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    al_set_target_bitmap(b);
    al_clear_to_color(black);
    al_set_target_bitmap(b16);
    al_clear_to_color(black);
    al_set_target_bitmap(b162);
    al_clear_to_color(black);
    //al_set_target_bitmap(al_get_current_display()); // TODO: do we need this?
    //al_clear_to_color(black);
}

void video_shutdown()
{
    //allegro_exit();
}

void video_start_timer()
{
    log_debug("video_start_timer: staring timer %p\n", timer);
    al_start_timer(timer);
}

void video_stop_timer()
{
    log_debug("video_start_timer: staring timer %p\n", timer);
    al_stop_timer(timer);   
}


