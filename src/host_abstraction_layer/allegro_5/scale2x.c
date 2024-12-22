/*
   This implements the AdvanceMAME Scale2x feature found on this page,
   http://scale2x.sourceforge.net/

   It is an incredibly simple and powerful image doubling routine that does
   an astonishing job of doubling game graphic data while interpolating out
   the jaggies. Congrats to the AdvanceMAME team, I'm very impressed and
   surprised with this code!
*/

/*Modified by Sarah Walker to work with Allegro*/

#include <stdint.h>
#include <allegro5/allegro.h>
#include "video_internal.h"

#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

/*
  this requires a destination surface already setup to be twice as
  large as the source. oh, and formats must match too. this will just
  blindly assume you didn't flounder.
*/

typedef uint32_t elk_pallete_t;
extern elk_pallete_t elkpal[8];

void scale2x(uint8_t * elk_screen_data, ALLEGRO_BITMAP * bitmapDest, int width, int height)
{
	int looph, loopw;
    char * region_data = NULL;
    char * region_data_plus_1 = NULL;

    ALLEGRO_LOCKED_REGION * destRegion = al_lock_bitmap(bitmapDest, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
	uint8_t* srcpix = elk_screen_data;
   	uint32_t E0, E1, E2, E3, B, D, E, F, H;

	for(looph = 0; looph < height; ++looph)
	{
        region_data        = (char *)destRegion->data + (destRegion->pitch * (looph * 2));
        region_data_plus_1 = (char *)destRegion->data + (destRegion->pitch * ((looph * 2) + 1));
		for(loopw = 0; loopw < width; ++ loopw)
		{
	    	B = elkpal[*(srcpix + (MAX(0,looph-1)*width) + loopw)];
	    	D = elkpal[*(srcpix + (looph*width) + (MAX(0,loopw-1)))];
	    	E = elkpal[*(srcpix + (looph*width) + loopw)];
	    	F = elkpal[*(srcpix + (looph*width) + (MIN(width-1,loopw+1)))];
	    	H = elkpal[*(srcpix + (MIN(height-1,looph+1)*width) + (loopw))];

    		E0 = D == B && B != F && D != H ? D : E;
   	    	E1 = B == F && B != D && F != H ? F : E;
	    	E2 = D == H && D != B && H != F ? D : E;
		    E3 = H == F && D != H && B != F ? F : E;

            *((uint32_t *)((char *)region_data)) = E0;
            region_data += destRegion->pixel_size;
            *((uint32_t *)((char *)region_data)) = E1;
            region_data += destRegion->pixel_size;
            *((uint32_t *)((char *)region_data_plus_1)) = E2;
            region_data_plus_1 += destRegion->pixel_size;
            *((uint32_t *)((char *)region_data_plus_1)) = E3;
            region_data_plus_1 += destRegion->pixel_size;
		}
	}
    al_unlock_bitmap(bitmapDest);
}

