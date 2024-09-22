// Video abstration layer.
// Allows actual graphics libraries and sound used for the emulation to be abstracted from the actual electron code
// This allows easier porting to different graphics and sound libraries in futurer in order to allow maximum
// cross platform support and long term durability.

// This is the allegro4 implementation of the abstraction layer.

#include <allegro.h>
#include "video.h"

BITMAP *b = NULL;    // Main bitmap used before blitting to window screen.
BITMAP *b16 = NULL;  // Intermediate bitmap 1
BITMAP *b162 = NULL; // Intermediate bitmap 2
BITMAP *vidb = NULL; // Windows bitmap
BITMAP *vp1 = NULL;  // Windows bitmap 1?
BITMAP *vp2 = NULL;  // Windows bitmap 2?

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

void video_set_depth_and_elk_palette()
{
    set_color_depth(8);
    set_palette(elkpal);
}

void video_set_desktop_color_depth()
{
    set_color_depth(desktop_color_depth());
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
