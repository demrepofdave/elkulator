// Video abstration layer.
// Allows actual graphics libraries and sound used for the emulation to be abstracted from the actual electron code
// This allows easier porting to different graphics and sound libraries in futurer in order to allow maximum
// cross platform support and long term durability.

// This is the allegro4 implementation of the abstraction layer.

#include <allegro5/allegro.h>
#include "common/video.h"
#include "video_internal.h"

ALLEGRO_BITMAP *b             = NULL;    // Main bitmap used before blitting to window screen.
ALLEGRO_BITMAP *b16           = NULL;  // Intermediate bitmap 1
ALLEGRO_BITMAP *b162          = NULL; // Intermediate bitmap 2
ALLEGRO_BITMAP *vidb          = NULL; // Windows bitmap
ALLEGRO_BITMAP *vp1           = NULL;  // Windows bitmap 1?
ALLEGRO_BITMAP *vp2           = NULL;  // Windows bitmap 2?
ALLEGRO_BITMAP *bm_screenshot = NULL; // USed for screenshots.

ALLEGRO_LOCKED_REGION *region = NULL; // Region lock on bitmap b (to allow writing of pixels)

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

void video_init_part2()
{
    b16 = al_create_bitmap(800*2,600);
    b162= al_create_bitmap(640,256);
    clear(b16);
    Init_2xSaI(desktop_color_depth());
}

void video_init_part3()
{
    set_color_depth(8);
    b = al_create_bitmap(640,616);
    set_palette(elkpal);
}


void video_set_gfx_mode_windowed(int w, int h, int v_w, int v_h)
{
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, w, h, v_w, v_h);
}

void video_set_gfx_mode_fullscreen(int w, int h, int v_w, int v_h)
{
    set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, w, h, v_w, v_h);
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

void video_put_pixel(int y, int x, uint8_t color)
{
    *((uint32_t *)((char *)region->data + region->pitch * y + x * region->pixel_size)) = color;
}

void video_put_pixel_line(int y, int x, int width, uint8_t color)
{
    int count = width;

    char *ptr = (char *)region->data + region->pitch * y + x * region->pixel_size;
    while (count--) {
        *(uint32_t *)ptr = color;
        ptr += region->pixel_size;
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

void video_blit_to_screen(int drawMode, int winsizeX, int winsizeY, int colDepth)
{
    int c;
    startblit();
    switch (drawMode)
    {
        case SCANLINES:
            blit(b,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case LINEDBL:
            #ifdef WIN32
                blit(b,vidb,0,0,0,0,640,256);
                if (videoresize) stretch_blit(vidb,screen,0,0,640,256,0,0,winsizex,winsizey);
                else             stretch_blit(vidb,screen,0,0,640,256,(winsizex-640)/2,(winsizey-512)/2,640,512);
            #else
                for (c=0;c<512;c++)
                {
                    blit(b,b16,0,c>>1,0,c,640,1);
                }
                blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            #endif
            break;

        case _2XSAI:
            blit(b,b162,0,0,0,0,640,256);
            Super2xSaI(b162,b16,0,0,0,0,320,256);
            blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case SCALE2X:
            blit(b,b162,0,0,0,0,640,256);
            scale2x(b162,b16,320,256);
            blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case EAGLE:
            blit(b,b162,0,0,0,0,640,256);
            SuperEagle(b162,b16,0,0,0,0,320,256);
            blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
            break;

        case PAL: // TODO: Not currently working (blank screen)
            palfilter(b,b16,colDepth);
            blit(b16,screen,0,0,(winsizeX-640)/2,(winsizeY-512)/2,640,512);
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
