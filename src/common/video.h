#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h>

#define SCANLINES 0
#define LINEDBL   1
#define _2XSAI    2
#define SCALE2X   3
#define EAGLE     4
#define PAL       5

typedef enum 
{
    BIT_SCREEN,
    BIT_MAIN,
    BIT_B16,
    BIT_B162,
    BIT_VIDB,
    BIT_VP1,
    BIT_VP2
} bitmapSelect;


void video_init();
void video_init2();
void video_set_gfx_mode_windowed(int w, int h, int v_w, int v_h);
void video_set_gfx_mode_fullscreen(int w, int h, int v_w, int v_h);
void video_set_depth_and_elk_palette();
void video_set_desktop_color_depth();
int  video_get_desktop_color_depth();

void video_put_pixel(bitmapSelect bitmapIndex, int y, int x, uint8_t color);

void video_blit(bitmapSelect bitmapSourceIndex, bitmapSelect bitmapDestIndex, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
void video_blit_to_screen(int drawMode, int winsizeX, int winsizeY, int colDepth);
void video_capture_screenshot(int drawMode, int colDepth);
int  video_save_bmp(const char * filename);
void video_destroy_screenshot();

void video_clearall();

void video_shutdown();

#endif // _VIDEO_H
