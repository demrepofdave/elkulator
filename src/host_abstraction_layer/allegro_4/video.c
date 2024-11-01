// Video abstration layer.
// Allows actual graphics libraries and sound used for the emulation to be abstracted from the actual electron code
// This allows easier porting to different graphics and sound libraries in futurer in order to allow maximum
// cross platform support and long term durability.

// This is the allegro4 implementation of the abstraction layer.

#include <allegro.h>
#include "host_abstraction_layer/video.h"
#include "video_internal.h"
#include "logger.h"

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

window_config_t main_window;

uint8_t movie_frame_data[640*256];


// Called from linux.c (main)
int video_init_part1()
{
    return(allegro_init());
}

// Called from ula.c (ulainit)
void video_init_part2()
{
    b16=create_bitmap(800*2,600);
    b162=create_bitmap(640,256);
    moviebitmap=create_bitmap_ex(8, 640, 256);
    clear(b16);
    Init_2xSaI(desktop_color_depth());
    set_color_depth(8);
    b=create_bitmap(640,616);
    set_palette(elkpal);
    initpaltables();
}

// Called from main.c (initelk)
void video_init_part3(void (*timer_function)(void))
{
#ifndef WIN32
        install_keyboard();
#endif
        install_timer();
        install_int_ex(timer_function,MSEC_TO_TIMER(20));
        install_joystick(JOY_TYPE_AUTODETECT);
        install_mouse();
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

void video_rest(unsigned int period)
{
    return rest(period);
}

void video_set_window_size(int w, int h, int v_w, int v_h)
{
    main_window.current_elk.winsizex = w;
    main_window.current_elk.winsizey = h;
}

void video_set_gfx_mode_windowed()
{
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, main_window.current_elk.winsizex, main_window.current_elk.winsizey, 0,0);
}

void video_set_gfx_mode_fullscreen()
{
    set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, main_window.current_elk.winsizex, main_window.current_elk.winsizey, 0, 0);
}

void video_set_depth_and_elk_palette()
{
    set_color_depth(8);
    set_palette(elkpal);
}

void video_set_desktop_color_depth()
{
    set_color_depth(desktop_color_depth());
}

int video_get_desktop_color_depth()
{
    return(desktop_color_depth());
}

uint32_t video_get_pixel(int y, int x)
{
    return *(movie_frame_data + (y * 640) + x);
}

// TODO: Depricated.
void video_put_pixel(int y, int x, uint8_t color)
{
    if(x >= 640 || y >= 256)
    {
        log_debug("Overflow %d, %d", x, y);
    }
    else
    {
        *(movie_frame_data + (y * 640) + x) = color;
    }
}

void video_put_pixel_line(int y, int x, int width, uint8_t color)
{
    int count = width;

    if((x + width) >= 640 || y >= 256)
    {
        log_debug("Overflow %d, %d", x, y);
    }
    else
    {
        while (count--) 
        {
            *(movie_frame_data + (y * 640) + x + count) = color;
        }
    }
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

void blit_normal(BITMAP * destBitmap)
{
    int y = 0;
    int x = 0;
    int color = 0;
    char * region_data = NULL;
    
    //ALLEGRO_LOCKED_REGION * destRegion = al_lock_bitmap(destBitmap, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

    // Here we create B from the memory data we have assembled.
    for(y=0; y<256; y++)
    {
        for(x=0; x<640; x++)
        {
            destBitmap->line[y][x] = color;
        }
    }
//    al_unlock_bitmap(destBitmap);
}

void video_blit_to_screen(int drawMode, int colDepth)
{
    int c;
    startblit();
    switch (drawMode)
    {
        case SCANLINES:
            blit_normal(b);
            blit(b,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case LINEDBL:
            blit_normal(b);
            for (c=0;c<512;c++)
            {
                blit(b,b16,0,c>>1,0,c,640,1);
            }
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case _2XSAI:
            blit_normal(b);
            blit(b,b162,0,0,0,0,640,256);
            Super2xSaI(b162,b16,0,0,0,0,320,256);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case SCALE2X:
            blit_normal(b);
            blit(b,b162,0,0,0,0,640,256);
            scale2x(b162,b16,320,256);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case EAGLE:
            blit_normal(b);
            blit(b,b162,0,0,0,0,640,256);
            SuperEagle(b162,b16,0,0,0,0,320,256);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;

        case PAL: // TODO: Not currently working (blank screen)
            blit_normal(b);
            palfilter(b,b16,colDepth);
            blit(b16,screen,0,0,(main_window.current_elk.winsizex-640)/2,(main_window.current_elk.winsizey-512)/2,640,512);
            break;
    }
    endblit();
}

void video_capture_screenshot(int drawMode, int colDepth)
{
    video_set_desktop_color_depth();
    bm_screenshot = create_bitmap(640,512);
    switch (drawMode)
    {
        case SCANLINES:
            blit(b,bm_screenshot,0,0,0,0,640,512);
            break;

        case LINEDBL:
            #ifdef WIN32
                stretch_blit(vidb,tb,0,0,640,256,0,0,640,512);
            #else
                blit(b16,bm_screenshot,0,0,0,0,640,512);
            #endif
            break;

        case _2XSAI:
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
            break;
    }
    set_color_depth(8);
}

int video_save_bmp(const char * filename)
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

void video_render_frame_for_movie()
{
    blit(b,moviebitmap,0,0,0,0,640,256);
}

uint8_t * video_get_moviebitmap_data()
{
    return(movie_frame_data);
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
