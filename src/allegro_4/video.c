// Video abstration layer.
// Allows actual graphics libraries and sound used for the emulation to be abstracted from the actual electron code
// This allows easier porting to different graphics and sound libraries in futurer in order to allow maximum
// cross platform support and long term durability.

// This is the allegro4 implementation of the abstraction layer.

#include <allegro.h>
#include "common/video.h"
#include "video_internal.h"

BITMAP *b = NULL;    // Main bitmap used before blitting to window screen.
BITMAP *b16 = NULL;  // Intermediate bitmap 1
BITMAP *b162 = NULL; // Intermediate bitmap 2
BITMAP *vidb = NULL; // Windows bitmap
BITMAP *vp1 = NULL;  // Windows bitmap 1?
BITMAP *vp2 = NULL;  // Windows bitmap 2?
BITMAP *bm_screenshot = NULL; // USed for screenshots.

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


BITMAP * getBitmap(bitmapSelect bitmapIndex)
{
    BITMAP * bitmap = NULL;

    switch(bitmapIndex)
    {
        case BIT_SCREEN:
            bitmap = screen;
            break;
        
        case BIT_MAIN:
            bitmap = b;
            break;

        case BIT_B16:
            bitmap = b16;
            break;

        case BIT_B162:
            bitmap = b162;
            break;

        case BIT_VIDB:
            bitmap = vidb;
            break;

        case BIT_VP1:
            bitmap = vp1;
            break;

        case BIT_VP2:
            bitmap = vp2;
            break;

        default:
            break;
    }
    return (bitmap);
}

void video_init()
{
    b16=create_bitmap(800*2,600);
    b162=create_bitmap(640,256);
    clear(b16);
    Init_2xSaI(desktop_color_depth());
}

void video_init2()
{
    set_color_depth(8);
    b=create_bitmap(640,616);
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

void video_put_pixel(bitmapSelect bitmapIndex, int y, int x, uint8_t color)
{
    BITMAP * bitmap = getBitmap(bitmapIndex);
    bitmap->line[y][x]=color;
}

void video_blit(bitmapSelect bitmapSourceIndex, bitmapSelect bitmapDestIndex, int source_x, int source_y, int dest_x, int dest_y, int width, int height)
{
    BITMAP * bitmapSource = getBitmap(bitmapSourceIndex);
    BITMAP * bitmapDest   = getBitmap(bitmapDestIndex);
    blit(bitmapSource, bitmapDest, source_x, source_y, dest_x, dest_y, width, height);
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
