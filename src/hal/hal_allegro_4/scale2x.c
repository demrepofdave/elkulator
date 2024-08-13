/*
   This implements the AdvanceMAME Scale2x feature found on this page,
   http://scale2x.sourceforge.net/

   It is an incredibly simple and powerful image doubling routine that does
   an astonishing job of doubling game graphic data while interpolating out
   the jaggies. Congrats to the AdvanceMAME team, I'm very impressed and
   surprised with this code!
*/

/*Modified by Sarah Walker to work with Allegro*/

#include <allegro.h>
#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned long Uint32;

#define READINT24(x)      ((x)[0]<<16 | (x)[1]<<8 | (x)[2])
#define WRITEINT24(x, i)  {(x)[0]=i>>16; (x)[1]=(i>>8)&0xff; x[2]=i&0xff; }

/*
  this requires a destination surface already setup to be twice as
  large as the source. oh, and formats must match too. this will just
  blindly assume you didn't flounder.
*/

void scale2x(BITMAP *src, BITMAP *dst, int width, int height)
{
	int looph, loopw;

	Uint8* srcpix = (Uint8*)src->dat;
	Uint8* dstpix = (Uint8*)dst->dat;

	const int srcpitch = (src->w*bitmap_color_depth(src))/8;
	const int dstpitch = (dst->w*bitmap_color_depth(dst))/8;
//	const int width = src->w;
//	const int height = src->h;

	switch(bitmap_color_depth(src))
	{
	case 8: {
	    	Uint8 E0, E1, E2, E3, B, D, E, F, H;
		for(looph = 0; looph < height; ++looph)
		{
			for(loopw = 0; loopw < width; ++ loopw)
			{
			    	B = *(Uint8*)(srcpix + (MAX(0,looph-1)*srcpitch) + (1*loopw));
			    	D = *(Uint8*)(srcpix + (looph*srcpitch) + (1*MAX(0,loopw-1)));
			    	E = *(Uint8*)(srcpix + (looph*srcpitch) + (1*loopw));
			    	F = *(Uint8*)(srcpix + (looph*srcpitch) + (1*MIN(width-1,loopw+1)));
			    	H = *(Uint8*)(srcpix + (MIN(height-1,looph+1)*srcpitch) + (1*loopw));

				E0 = D == B && B != F && D != H ? D : E;
    	    	    	    	E1 = B == F && B != D && F != H ? F : E;
				E2 = D == H && D != B && H != F ? D : E;
				E3 = H == F && D != H && B != F ? F : E;

				*(Uint8*)(dstpix + looph*2*dstpitch + loopw*2*1) = E0;
				*(Uint8*)(dstpix + looph*2*dstpitch + (loopw*2+1)*1) = E1;
				*(Uint8*)(dstpix + (looph*2+1)*dstpitch + loopw*2*1) = E2;
				*(Uint8*)(dstpix + (looph*2+1)*dstpitch + (loopw*2+1)*1) = E3;
			}
		}break;}
	case 15: case 16: {
	    	Uint16 E0, E1, E2, E3, B, D, E, F, H;
		for(looph = 0; looph < height; ++looph)
		{
			for(loopw = 0; loopw < width; ++ loopw)
			{
			    	B = *(Uint16*)(srcpix + (MAX(0,looph-1)*srcpitch) + (2*loopw));
			    	D = *(Uint16*)(srcpix + (looph*srcpitch) + (2*MAX(0,loopw-1)));
			    	E = *(Uint16*)(srcpix + (looph*srcpitch) + (2*loopw));
			    	F = *(Uint16*)(srcpix + (looph*srcpitch) + (2*MIN(width-1,loopw+1)));
			    	H = *(Uint16*)(srcpix + (MIN(height-1,looph+1)*srcpitch) + (2*loopw));

				E0 = D == B && B != F && D != H ? D : E;
    	    	    	    	E1 = B == F && B != D && F != H ? F : E;
				E2 = D == H && D != B && H != F ? D : E;
				E3 = H == F && D != H && B != F ? F : E;

				*(Uint16*)(dstpix + looph*2*dstpitch + loopw*2*2) = E0;
				*(Uint16*)(dstpix + looph*2*dstpitch + (loopw*2+1)*2) = E1;
				*(Uint16*)(dstpix + (looph*2+1)*dstpitch + loopw*2*2) = E2;
				*(Uint16*)(dstpix + (looph*2+1)*dstpitch + (loopw*2+1)*2) = E3;
			}
		}break;}
	case 24: {
	    	int E0, E1, E2, E3, B, D, E, F, H;
		for(looph = 0; looph < height; ++looph)
		{
			for(loopw = 0; loopw < width; ++ loopw)
			{
			    	B = READINT24(srcpix + (MAX(0,looph-1)*srcpitch) + (3*loopw));
			    	D = READINT24(srcpix + (looph*srcpitch) + (3*MAX(0,loopw-1)));
			    	E = READINT24(srcpix + (looph*srcpitch) + (3*loopw));
			    	F = READINT24(srcpix + (looph*srcpitch) + (3*MIN(width-1,loopw+1)));
			    	H = READINT24(srcpix + (MIN(height-1,looph+1)*srcpitch) + (3*loopw));

				E0 = D == B && B != F && D != H ? D : E;
    	    	    	    	E1 = B == F && B != D && F != H ? F : E;
				E2 = D == H && D != B && H != F ? D : E;
				E3 = H == F && D != H && B != F ? F : E;

				WRITEINT24((dstpix + looph*2*dstpitch + loopw*2*3), E0);
				WRITEINT24((dstpix + looph*2*dstpitch + (loopw*2+1)*3), E1);
				WRITEINT24((dstpix + (looph*2+1)*dstpitch + loopw*2*3), E2);
				WRITEINT24((dstpix + (looph*2+1)*dstpitch + (loopw*2+1)*3), E3);
			}
		}break;}
	default: { /*case 4:*/
	    	Uint32 E0, E1, E2, E3, B, D, E, F, H;
		for(looph = 0; looph < height; ++looph)
		{
			for(loopw = 0; loopw < width; ++ loopw)
			{
			    	B = *(Uint32*)(srcpix + (MAX(0,looph-1)*srcpitch) + (4*loopw));
			    	D = *(Uint32*)(srcpix + (looph*srcpitch) + (4*MAX(0,loopw-1)));
			    	E = *(Uint32*)(srcpix + (looph*srcpitch) + (4*loopw));
			    	F = *(Uint32*)(srcpix + (looph*srcpitch) + (4*MIN(width-1,loopw+1)));
			    	H = *(Uint32*)(srcpix + (MIN(height-1,looph+1)*srcpitch) + (4*loopw));

				E0 = D == B && B != F && D != H ? D : E;
    	    	    	    	E1 = B == F && B != D && F != H ? F : E;
				E2 = D == H && D != B && H != F ? D : E;
				E3 = H == F && D != H && B != F ? F : E;

				*(Uint32*)(dstpix + looph*2*dstpitch + loopw*2*4) = E0;
				*(Uint32*)(dstpix + looph*2*dstpitch + (loopw*2+1)*4) = E1;
				*(Uint32*)(dstpix + (looph*2+1)*dstpitch + loopw*2*4) = E2;
				*(Uint32*)(dstpix + (looph*2+1)*dstpitch + (loopw*2+1)*4) = E3;
			}
		}break;}
	}
}

