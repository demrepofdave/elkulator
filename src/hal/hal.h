// Graphic Adaption Layer.

// Provides common API for interaction with Graphics library.
// This allows Elkulator to be compiled with allegro4, allegro5, or a different graphics library
// (Maybe SDL) in future.  The aim is to make it easier to swapout and update the
// native graphics driver portion of the code and separate it from the actual Electron driving
// code

// Following functions have been identified as being needed in allegro4.

#ifndef _HAL_H
#define _HAL_H

#include "stdint.h"

//  Possible API

// Abstract handles

typedef int hal_result; 
typedef int hal_bitmap_handle;

// Define hal_result error codes.
#define HAL_OK                  0
#define HAL_UNINITIALISED      -1
#define HAL_INVALID_HANDLE     -2
#define HAL_HANDLE_TABLE_FULL  -3
#define HAL_BITMAP_UNALLOCATED -4

// General hal functions.
hal_result hal_init(); // Initialise the HAL. This must be done before any other API calls to the HAL are made.
hal_result hal_set_window_title(const char * name);
hal_result hal_set_close_button_callback( void (*handler_function)(void));
hal_result hal_install_mouse();

// Bitmap management
hal_bitmap_handle hal_allocate_bitmap(); // Returns a handler to a BITMAP type and initialises the base BITMAP
hal_result hal_create_bitmap(hal_bitmap_handle handle, int width, int height);
hal_result hal_create_bitmap_ex(hal_bitmap_handle handle, int color_depth, int width, int height);
hal_result hal_destroy_bitmap(hal_bitmap_handle handle);
hal_result hal_release_bitmap(hal_bitmap_handle handle); // Releases bitmap and returns handler to the pool.

// Blitting functions
hal_result hal_blit_screen(hal_bitmap_handle source, int source_x, int source_y, int dest_x, int dest_y, int width, int height); 
hal_result hal_blit(hal_bitmap_handle source, hal_bitmap_handle dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);

// Bitmap manipulation functions
hal_result hal_clear_bitmap(hal_bitmap_handle handle);

hal_result hal_Super2xSaI(hal_bitmap_handle source, hal_bitmap_handle dest, int s_x, int s_y, int d_x, int d_y, int w, int h);
hal_result hal_SuperEagle(hal_bitmap_handle source, hal_bitmap_handle dest, int s_x, int s_y, int d_x, int d_y, int w, int h);
hal_result hal_scale2x   (hal_bitmap_handle source, hal_bitmap_handle dest, int width, int height);
hal_result hal_palfilter (hal_bitmap_handle source, hal_bitmap_handle dest, int depth);

hal_result hal_save_bmp(hal_bitmap_handle handle, const char *filename);

hal_result hal_bitmap_setpixel(hal_bitmap_handle handle, int y, int x, uint8_t col);
//b->line[ula.y][(ula.x+x)>>1]=pal[col];

// Functions that break the HAL
void *hal_get_bitmap_dat_ex(hal_bitmap_handle handle);

#endif // _HAL_H

/* DemRepOfDave Notes.

Things that need to be abstracted:

BITMAP * (allegro5 = ALLEGRO_BITMAP)
PALLETE * (allegro5 = ???)

*/