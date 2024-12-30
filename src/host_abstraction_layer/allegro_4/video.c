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
 * This is the allegro 4 implementation of the abstraction layer.
 *
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <allegro.h>
#include "host_abstraction_layer/video.h"
#include "video_internal.h"
#include "logger.h"

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

BITMAP *b = NULL;    // Main bitmap used before blitting to window screen.
BITMAP *b16 = NULL;  // Intermediate bitmap 1
BITMAP *b162 = NULL; // Intermediate bitmap 2
BITMAP *vidb = NULL; // Windows bitmap
BITMAP *vp1 = NULL;  // Windows bitmap 1?
BITMAP *vp2 = NULL;  // Windows bitmap 2?
BITMAP *bm_screenshot = NULL; // USed for screenshots.
BITMAP *moviebitmap   = NULL; // Used for capturing movies.

PALETTE elkpal =
{
      {0,0,0},
      {63,0,0},
      {0,63,0},
      {63,63,0},
      {0,0,63},
      {63,0,63},
      {0,63,63},
      {63,63,63},
};

int coldepth;

window_config_t main_window;

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

void video_set_depth_and_elk_palette()
{
    set_color_depth(8);
    set_palette(elkpal);
}

void video_set_desktop_color_depth()
{
    set_color_depth(desktop_color_depth());
}

void video_set_gfx_mode_fullscreen()
{
    set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, main_window.current_elk.winsizex, main_window.current_elk.winsizey, 0, 0);
}

void video_set_gfx_mode_windowed()
{
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, main_window.current_elk.winsizex, main_window.current_elk.winsizey, 0,0);
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

// Called from linux.c (main)
int video_init_begin()
{
    return(allegro_init());
}

// Called from ula.c (ulainit)
void video_init_complete()
{
    coldepth=desktop_color_depth();

    video_set_desktop_color_depth();
    video_set_window_size(640,512,0,0);
    video_set_gfx_mode_windowed();

    b16=create_bitmap(800*2,600);
    b162=create_bitmap(640,256);
    moviebitmap=create_bitmap_ex(8, 640, 256);
    clear(b16);
    Init_2xSaI(desktop_color_depth());
    set_color_depth(8);
    b=create_bitmap(640,616);
    set_palette(elkpal);
    initpaltables();
    video_set_desktop_color_depth();
}

void video_register_close_button_handler(void (*handler_function)(void))
{
    set_close_button_callback(handler_function);
}

int video_set_display_switch_mode_background()
{
    return set_display_switch_mode(SWITCH_BACKGROUND);
}

int video_poll_joystick()
{
    return poll_joystick();
}

void video_enterfullscreen()
{
        video_set_desktop_color_depth();
        video_set_window_size(800,600, 0, 0);
        video_set_gfx_mode_fullscreen();
        video_set_depth_and_elk_palette();
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
        video_set_desktop_color_depth();
//        #ifdef WIN32
//        video_set_gfx_mode_windowed(048,2048,0,0);
//        vidb=create_video_bitmap(800,300);
//        #else
        video_set_window_size(640,512,0,0);
        video_set_gfx_mode_windowed();
//        #endif
        video_set_depth_and_elk_palette();
}

void video_set_window_size(int w, int h, int v_w, int v_h)
{
    main_window.current_elk.winsizex = w;
    main_window.current_elk.winsizey = h;
}


#ifdef WIN32
CRITICAL_SECTION cs;
#endif

void startblit()
{
    #ifdef WIN32
        EnterCriticalSection(&cs);
    #endif
}

void endblit()
{
    #ifdef WIN32
        LeaveCriticalSection(&cs);
    #endif
}

void blit_normal(BITMAP * destBitmap, uint8_t * elk_screen_data)
{
    int y = 0;
    int x = 0;
    int color = 0;
    
    // Here we create B from the memory data we have assembled.
    for(y=0; y<256; y++)
    {
        for(x=0; x<640; x++)
        {
            color = *(elk_screen_data + (y * 640) + x);
            destBitmap->line[y*2][x] = color;
            destBitmap->line[y*2+1][x] = color;
        }
    }
}

void blit_scanlines(BITMAP * destBitmap, uint8_t * elk_screen_data)
{
    int y = 0;
    int x = 0;
    int color = 0;

    clear(destBitmap);

    // Here we create B from the memory data we have assembled.
    for(y=0; y<256; y++)
    {
        for(x=0; x<640; x++)
        {
            color = *(elk_screen_data + (y * 640) + x);
            destBitmap->line[y*2][x] = color;
        }
    }
}

void video_blit_to_screen(int drawMode, uint8_t * elk_screen_data)
{
    int c;
    startblit();
    video_set_desktop_color_depth();
    switch (drawMode)
    {
        case SCANLINES:
            blit_scanlines(b, elk_screen_data);
            blit(b,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case LINEDBL:
            blit_normal(b, elk_screen_data);
            blit(b,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case _2XSAI:
            blit_normal(b, elk_screen_data);
            blit(b,b162,0,0,0,0,640,256);
            Super2xSaI(b162,b16,0,0,0,0,320,256);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case SCALE2X:
            blit_normal(b, elk_screen_data);
            blit(b,b162,0,0,0,0,640,256);
            scale2x(b162,b16,320,256);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case EAGLE:
            blit_normal(b, elk_screen_data);
            blit(b,b162,0,0,0,0,640,256);
            SuperEagle(b162,b16,0,0,0,0,320,256);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case PAL: // TODO: Not currently working (blank screen)
            blit_normal(b, elk_screen_data);
            palfilter(b,b16,coldepth);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;
    }
    set_color_depth(8);
    endblit();
}

void video_capture_screenshot(int drawMode)
{
    video_set_desktop_color_depth();
    bm_screenshot = create_bitmap(640,512);
    switch (drawMode)
    {
        case SCANLINES:
            //blit_scanlines(b, elk_screen_data);
            blit(b,bm_screenshot,0,0,0,0,640,512);
            break;

        case LINEDBL:
            //blit_normal(b, elk_screen_data);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case _2XSAI:
            //blit_normal(b, elk_screen_data);
            blit(b,b162,0,0,0,0,640,256);
            Super2xSaI(b162,b16,0,0,0,0,320,256);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case SCALE2X:
            //blit_normal(b, elk_screen_data);
            blit(b,b162,0,0,0,0,640,256);
            scale2x(b162,b16,320,256);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case EAGLE:
            //blit_normal(b, elk_screen_data);
            blit(b,b162,0,0,0,0,640,256);
            SuperEagle(b162,b16,0,0,0,0,320,256);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;

        case PAL:
            //blit_normal(b, elk_screen_data);
            palfilter(b,b16,coldepth);
            blit(b16,bm_screenshot,0,0,0,0,640,512);
            break;
    }
    set_color_depth(8);
}

int video_save_screenshot_bmp(const char * filename)
{
    return(save_bmp(filename, bm_screenshot, NULL));
}
void video_destroy_screenshot()
{
    if(bm_screenshot)
    {
        destroy_bitmap(bm_screenshot);
        bm_screenshot = NULL;
    }
}

void video_clearall()
{
    clear(b);
    clear(b16);
    clear(b162);
    clear(screen);
}

void video_shutdown()
{
    allegro_exit();
}

void video_start_timer()
{
    // For allegro4 there is nothing to do.
}

void video_stop_timer()
{
    // For allegro4 there is nothing to do.
}

void video_set_window_title(const char * title)
{
    // TODO: Allegro4 version.
}