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
#include "allegro.h"  // Temporary for now until abstraction is abstracted :)

//  Possible API

// Abstract handles

typedef int hal_result; 
typedef uint8_t hal_bitmap_handle;

//hal_result hal_init(); // Initialise the HAL. This must be done before any other API calls to the HAL are made.
//hal_bitmap_handle hal_allocate_bitmap(); // Returns a handler to a BITMAP type and initialises the base BITMAP
//hal_result hal_release_bitmap(); // Releases bitmap and returns handler to the pool.

// Temporary home for some functions moved from src into hal
void scale2x(BITMAP *src, BITMAP *dst, int width, int height);
void palfilter(BITMAP *src, BITMAP *dest, int depth);

#endif // _HAL_H

/* DemRepOfDave Notes.

Things that need to be abstracted:

BITMAP * (allegro5 = ALLEGRO_BITMAP)
PALLETE * (allegro5 = ???)

*/