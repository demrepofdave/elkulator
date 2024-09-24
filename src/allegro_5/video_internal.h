#ifndef _VIDEO_INTERNAL_H
#define _VIDEO_INTERNAL_H

#include <allegro5/allegro.h>
#include "common/video.h"

// Private functions to be places in hal only header.

// 2xSaI routines.
#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char

int Init_2xSaI(int depth);
void Super2xSaI(ALLEGRO_BITMAP * bitmapSource, ALLEGRO_BITMAP * bitmapDest, int s_x, int s_y, int d_x, int d_y, int w, int h);
void SuperEagle(ALLEGRO_BITMAP * bitmapSource, ALLEGRO_BITMAP * bitmapDest, int s_x, int s_y, int d_x, int d_y, int w, int h);

// Other filer routines.
void scale2x(ALLEGRO_BITMAP * bitmapSource, ALLEGRO_BITMAP * bitmapDest, int width, int height);
void palfilter(ALLEGRO_BITMAP * bitmapSource, ALLEGRO_BITMAP * bitmapDest, int depth);

void Super2xSaI_ex(uint8 *src, uint32 src_pitch, uint8 *unused, ALLEGRO_BITMAP *dest, uint32 width, uint32 height);
void SuperEagle_ex(uint8 *src, uint32 src_pitch, uint8 *unused, ALLEGRO_BITMAP *dest, uint32 width, uint32 height);

#endif // _VIDEO_INTERNAL_H
