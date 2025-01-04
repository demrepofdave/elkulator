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
#include "host_abstraction_layer/video.h"
#include "host_abstraction_layer/allegro_5//menu_internal.h"
#include "logger.h"
#include "host_abstraction_layer/native_time.h"
#include "config_vars.h"
#include "elk.h"
#include "video_internal.h"
#include "event_handler_internal.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/

typedef uint32_t elk_pallete_t;

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

ALLEGRO_BITMAP *b             = NULL;    // Main bitmap used before blitting to window screen.
ALLEGRO_BITMAP *b16           = NULL;  // Intermediate bitmap 1
ALLEGRO_BITMAP *bm_screenshot = NULL; // Used for screenshots.
ALLEGRO_BITMAP *moviebitmap   = NULL; // Used for capturing movies.

static ALLEGRO_DISPLAY *display;

//ALLEGRO_LOCKED_REGION *region = NULL; // Region lock on bitmap b (to allow writing of pixels)

elk_pallete_t elkpal[8] =
{
    0xff000000,
    0xffff0000,
    0xff00ff00,
    0xffffff00,
    0xff0000ff,
    0xffff00ff,
    0xff00ffff,
    0xffffffff
};

window_info_elk_t current_elk_window;

/******************************************************************************
* Function Prototypes
*******************************************************************************/


/******************************************************************************
* Private Function Definitions
*******************************************************************************/

void log_window_config(const char * title)
{
    log_debug("Window status (%s)", title);
    log_debug("-------------");
    log_debug("- Actual window: %d, %d", elkConfig.display.native_window_width, elkConfig.display.native_window_height);
    log_debug("- Current Elk  : %d, %d", current_elk_window.winsizex, current_elk_window.winsizey);
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

void video_apply_window_size()
{
    log_window_config("video_apply_window_size");
    ALLEGRO_COLOR blue = al_map_rgb(0, 0, 64);
    al_draw_filled_rectangle(0,0, elkConfig.display.native_window_width, elkConfig.display.native_window_height, blue);
}

void video_set_gfx_mode_windowed()
{
    ALLEGRO_DISPLAY *display;
    display = al_get_current_display();

    video_apply_window_size();

    al_resize_display(display, current_elk_window.winsizex,  current_elk_window.winsizey);
    al_set_display_flag(display, ALLEGRO_MAXIMIZED, false);
    al_set_display_flag(display, ALLEGRO_FRAMELESS, false);
    al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, false);
//    al_set_display_flag
//    set_gfx_mode(GFX_AUTODETECT_WINDOWED, w, h, v_w, v_h);
}

void video_set_window_size(int w, int h, int v_w, int v_h)
{
    log_debug("Set window size %d, %d", w, h);
    current_elk_window.winsizex = w;
    current_elk_window.winsizey = h;
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

// Called from linux.c (main)
int video_init_begin()
{
    if (!al_init())
    {
        fprintf(stderr, "Error initializing Allegro.\n");
        exit(-1);
    }

    current_elk_window.winsizex = 800; // TODO: Will be configured in future
    current_elk_window.winsizey = 600; // TODO: Will be configured in future
    current_elk_window.startx = 0;
    current_elk_window.starty = 0;

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

    if ((display = al_create_display(current_elk_window.winsizex, current_elk_window.winsizey)) == NULL) {
        log_fatal("video: unable to create display");
        exit(1);
    }

    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP|ALLEGRO_NO_PRESERVE_TEXTURE);

    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    b16 = al_create_bitmap(1300,600);
    al_set_target_bitmap(b16);
    al_clear_to_color(black);

    moviebitmap = al_create_bitmap(640,256);

    b = al_create_bitmap(640, 616);
    al_set_target_bitmap(b);
    al_clear_to_color(al_map_rgb(0, 0,0));

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

void video_init_complete()
{
    video_set_window_size(elkConfig.display.native_window_width, elkConfig.display.native_window_height,0,0);
    video_set_gfx_mode_windowed();

    menu_init(display);
    video_set_window_size(elkConfig.display.native_window_width, elkConfig.display.native_window_height,0,0);
    //video_set_window_size(640,512,0,0);
    //video_set_gfx_mode_windowed();
    initpaltables();
}

void video_set_window_title(char * format, ...)
{
    char window_title_buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (window_title_buffer, sizeof(window_title_buffer), format, args);
    va_end (args);
    al_set_window_title(display, window_title_buffer);
}

void video_update_native_window_size(int w, int h)
{
    elkConfig.display.native_window_width = w;
    elkConfig.display.native_window_height = h;
    log_debug("video_update_native_window_size(%d, %d)", w, h);
}

void video_resize_elk_window(bool aspect_ratio)
{
    // Now we resize the screen based upon the above.
    int winsizeX = elkConfig.display.native_window_width;
    int winsizeY = elkConfig.display.native_window_height;
    current_elk_window.startx = 0;
    current_elk_window.starty = 0;

    log_window_config("video_resize_elk_window");

    // Maintain pixel ratio experimental code
    //if(winsizeX > 640 && winsizeY > 512)
    //{
    //    winsizeX = (winsizeX / 320) * 320;
    //    winsizeY = (winsizeY / 256) * 256;
    //}

    if(aspect_ratio)
    {
        int adjusted_width = ((elkConfig.display.native_window_width * 4) / 5) + 1;
        log_debug("Adjusted width = %d", adjusted_width);
        if(adjusted_width > winsizeY)
        {
            // Resize based on height
            winsizeX = ((winsizeY * 5) / 4);
            log_debug("w > h aspect ratio x, y: %d, %d", winsizeX, winsizeY);
        }
        else
        {
            winsizeY = ((winsizeX * 4) / 5);
            log_debug("w <= h aspect ratio x, y: %d, %d", winsizeX, winsizeY);
        }
        // Calculate startx and starty offsets.
        current_elk_window.startx = (elkConfig.display.native_window_width - winsizeX) / 2;
        current_elk_window.starty = (elkConfig.display.native_window_height - winsizeY) / 2;
    }

    video_set_window_size(winsizeX, winsizeY, 0,0);
    video_apply_window_size();
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

void video_enterfullscreen()
{
        video_set_window_size(800,600, 0, 0);
        video_set_gfx_mode_fullscreen();
        video_set_window_size(800,600, 0, 0);
}

void video_leavefullscreen()
{
//        #ifdef WIN32
//        remove_mouse();
//        destroy_bitmap(vidb);
//        destroy_bitmap(vp2);
//        destroy_bitmap(vp1);
//        #endif
//        #ifdef WIN32
//        video_set_gfx_mode_windowed(048,2048,0,0);
//        vidb=create_video_bitmap(800,300);
//        #else
        video_set_window_size(640,512,0,0);
        video_set_gfx_mode_windowed();
//        #endif
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

void blit_normal(ALLEGRO_BITMAP * destBitmap, uint8_t * elk_screen_data)
{
    int y = 0;
    int x = 0;
    int color = 0;
    char * region_data = NULL;
    
    ALLEGRO_LOCKED_REGION * destRegion = al_lock_bitmap(destBitmap, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

    // Here we create B from the memory data we have assembled.
    for(y=0; y<256; y++)
    {
        region_data = (char *)destRegion->data + (destRegion->pitch * y);
        for(x=0; x<640; x++)
        {
            color = *(elk_screen_data + (y * 640) + x);
            *((uint32_t *)((char *)region_data)) = elkpal[color];
            region_data += destRegion->pixel_size;
        }
    }
    al_unlock_bitmap(destBitmap);
}

void blit_scanlines(ALLEGRO_BITMAP * destBitmap, uint8_t * elk_screen_data)
{
    int y = 0;
    int x = 0;
    int color = 0;
    char * region_data = NULL;
    char * region_scan = NULL;

    ALLEGRO_LOCKED_REGION * destRegion = al_lock_bitmap(destBitmap, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

    // Here we create B from the memory data we have assembled.
    for(y=0; y<256; y++)
    {
        region_data = (char *)destRegion->data + (destRegion->pitch * (y * 2));
        region_scan = (char *)destRegion->data + (destRegion->pitch * ((y * 2) + 1));
        for(x=0; x<640; x++)
        {
            color = *(elk_screen_data + (y * 640) + x);
            *((uint32_t *)((char *)region_data)) = elkpal[color];
            *((uint32_t *)((char *)region_scan)) = 0xff000000;
            region_data += destRegion->pixel_size;
            region_scan += destRegion->pixel_size;
        }
    }
    al_unlock_bitmap(destBitmap);
}


t_timeDiffAverage video_blit_average;
t_timeDiffAverage video_scaled_draw_average;

void video_blit_to_screen(int drawMode, uint8_t * elk_screen_data)
{
    //log_timer_begin();
    //log_time_mark("video_blit_to_screen - start");

    native_time_init_average(&video_blit_average, 50);
    native_time_init_average(&video_scaled_draw_average, 50);

    native_timestamp_t timestamp = native_timestamp_get();
    startblit();

    switch (drawMode)
    {
        case SCANLINES:
            blit_scanlines(b, elk_screen_data);
            native_time_add_sample(&video_blit_average, native_timestamp_get() - timestamp);
            timestamp = native_timestamp_get();
            al_set_target_backbuffer(al_get_current_display());
            al_draw_scaled_bitmap(b, 0,0,640,512,
                                     current_elk_window.startx, current_elk_window.starty,
                                     current_elk_window.winsizex,current_elk_window.winsizey, 0);
            break;

        case LINEDBL:
            blit_normal(b, elk_screen_data);
            native_time_add_sample(&video_blit_average, native_timestamp_get() - timestamp);
            timestamp = native_timestamp_get();
            al_set_target_backbuffer(al_get_current_display());
            al_draw_scaled_bitmap(b, 0,0,640,256, 
                                     current_elk_window.startx, current_elk_window.starty,
                                     current_elk_window.winsizex,current_elk_window.winsizey, 0);
            break;

        case _2XSAI:  // TODO: Get filter working for allegro5
            blit_normal(b, elk_screen_data);
            native_time_add_sample(&video_blit_average, native_timestamp_get() - timestamp);
            timestamp = native_timestamp_get();

            al_set_target_backbuffer(al_get_current_display());
            al_draw_scaled_bitmap(b, 0,0,640,256, 
                                     current_elk_window.startx, current_elk_window.starty,
                                     current_elk_window.winsizex,current_elk_window.winsizey, 0);
            //blit(b,b162,0,0,0,0,640,256);
            //Super2xSaI(elk_screen_data,b,0,0,0,0,640,256);
            //al_set_target_backbuffer(al_get_current_display());
            //al_draw_scaled_bitmap(b16, firstx, firsty, xsize, ysize, scr_x_start, scr_y_start, scr_x_size, scr_y_size, 0);
            //al_draw_bitmap(b16, (winsizeX-640)/2,(winsizeY-512)/2);
            //blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case SCALE2X:
            scale2x(elk_screen_data, b16, 640,256);
            native_time_add_sample(&video_blit_average, native_timestamp_get() - timestamp);
            timestamp = native_timestamp_get();
            al_set_target_backbuffer(al_get_current_display());
            al_draw_scaled_bitmap(b16, 0,0,1280,512, 
                                     current_elk_window.startx, current_elk_window.starty,
                                     current_elk_window.winsizex,current_elk_window.winsizey, 0);
            break;

        case EAGLE: // TODO: Get filter working for allegro5
            blit_normal(b, elk_screen_data);
            native_time_add_sample(&video_blit_average, native_timestamp_get() - timestamp);
            timestamp = native_timestamp_get();
            al_set_target_backbuffer(al_get_current_display());
            al_draw_scaled_bitmap(b, 0,0,640,256, 
                                     current_elk_window.startx, current_elk_window.starty,
                                     current_elk_window.winsizex,current_elk_window.winsizey, 0);
            //blit(b,b162,0,0,0,0,640,256);
            //SuperEagle(b162,b16,0,0,0,0,320,256);
            //al_set_target_backbuffer(al_get_current_display());
            //al_draw_scaled_bitmap(b16, firstx, firsty, xsize, ysize, scr_x_start, scr_y_start, scr_x_size, scr_y_size, 0);
            //al_draw_bitmap(b16, (winsizeX-640)/2,(winsizeY-512)/2);
            //blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case PAL:
        {
            palfilter(b, elk_screen_data);
            native_time_add_sample(&video_blit_average, native_timestamp_get() - timestamp);
            timestamp = native_timestamp_get();
            //log_time_mark("video_blit_to_screen - pmid");
            al_set_target_backbuffer(al_get_current_display());
            al_draw_scaled_bitmap(b, 0,0,640,512, 
                                     current_elk_window.startx, current_elk_window.starty,
                                     current_elk_window.winsizex,current_elk_window.winsizey, 0);
            break;
        }
    }

    al_flip_display();
    endblit();
    native_time_add_sample(&video_scaled_draw_average, native_timestamp_get() - timestamp);

    log_debug("avgct=%d, %d", video_blit_average.current_sample, video_blit_average.max_samples);
    if(native_time_all_samples_collected(&video_blit_average))
    {
        native_time_log_average(&video_blit_average, "blit average");
        native_time_reset_samples(&video_blit_average);
    }

    if(native_time_all_samples_collected(&video_scaled_draw_average))
    {
        native_time_log_average(&video_scaled_draw_average, "scaled draw average");
        native_time_reset_samples(&video_scaled_draw_average);
    }
}

void video_capture_screenshot(int drawMode)
{
    bm_screenshot = al_create_bitmap(640,512);
    // NOTE: No need to run any filtering (e.g. palfilt) here as
    //       these routines have already run as part of screen 
    //       building, so all bitmaps are prepared.
    switch (drawMode)
    {
        case SCANLINES:
        case LINEDBL:
        case PAL:
            al_set_target_bitmap(bm_screenshot);
            al_draw_scaled_bitmap(b, 0,0,640,512, 0,0,640,512, 0);
            break;

        case _2XSAI:  // TODO: Get filter working for allegro5
            al_set_target_bitmap(bm_screenshot);
            al_draw_scaled_bitmap(b, 0,0,640,512, 0,0,640,512, 0);
            //blit(b,b162,0,0,0,0,640,256);
            //Super2xSaI(b162,b16,0,0,0,0,320,256);
            //blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case SCALE2X:
            al_set_target_bitmap(bm_screenshot);
            al_draw_scaled_bitmap(b16, 0,0,640,512, 0,0,640,512, 0); // TODO: Better resolution available.
            break;

        case EAGLE:  // TODO: Get filter working for allegro5
            al_set_target_bitmap(bm_screenshot);
            al_draw_scaled_bitmap(b, 0,0,640,512, 0,0,640,512, 0);
            //blit(b,b162,0,0,0,0,640,256);
            //SuperEagle(b162,b16,0,0,0,0,320,256);
            //blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

    }
}

int video_save_screenshot_bmp(const char * filename)
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
    //al_clear_to_color(black);
    //al_set_target_bitmap(al_get_current_display()); // TODO: do we need this?
    //al_clear_to_color(black);
}

void video_shutdown()
{
    //allegro_exit();
}

bool video_is_main_display(ALLEGRO_DISPLAY * current_display)
{
    log_debug("display %p = %p = %p", current_display, display, al_get_current_display());
    return(current_display == display);
}


