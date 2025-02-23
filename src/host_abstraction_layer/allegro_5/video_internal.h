#ifndef _VIDEO_INTERNAL_H
#define _VIDEO_INTERNAL_H

/******************************************************************************
* Include files
*******************************************************************************/

#include <allegro5/allegro.h>
#include "host_abstraction_layer/video.h"


/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

// 2xSaI routines.
#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char

/******************************************************************************
* Typedefs
*******************************************************************************/

// This structure will all information related to the native window state
// and coordinates.

typedef struct
{
    int startx;           // Start position within the native window for the electron window.
    int starty;           // Start position within the native window for the electron window.
    int winsizex;         // Current window size in pixels
    int winsizey;         // Current window size in pixels
} window_info_elk_t;

int  video_init_begin();
void video_init_complete();

void video_resize_elk_window(int width, int height, bool aspect_ratio);
void video_update_native_window_size(int w, int h);
void video_mouse_event();

bool video_is_main_display(ALLEGRO_DISPLAY * current_display);

int Init_2xSaI(int depth);
void Super2xSaI(ALLEGRO_BITMAP * bitmapDest, uint8_t * elk_screen_data, int w, int h);
void SuperEagle(ALLEGRO_BITMAP * bitmapSource, ALLEGRO_BITMAP * bitmapDest, int s_x, int s_y, int d_x, int d_y, int w, int h);

// Other filer routines.
void scale2x(uint8_t * elk_screen_data, ALLEGRO_BITMAP * bitmapDest, int width, int height);

void initpaltables();
void palfilter(ALLEGRO_BITMAP * destBitmap, uint8_t * elk_screen_data);

void Super2xSaI_ex(uint8_t * elk_screen_data, ALLEGRO_BITMAP *dest, uint32 width, uint32 height);
void SuperEagle_ex(uint8 *src, uint32 src_pitch, uint8 *unused, ALLEGRO_BITMAP *dest, uint32 width, uint32 height);

#endif // _VIDEO_INTERNAL_H
