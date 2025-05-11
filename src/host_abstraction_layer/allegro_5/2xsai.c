#include <string.h>
#include <allegro5/allegro.h>
//#include <allegro/internal/aintern.h>
#include <allegro5/allegro_primitives.h>
#include "video_internal.h"
#include "logger.h"

//#define TRACE rpclog
#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char

static uint32 colorMask = 0xF7DEF7DE;
static uint32 lowPixelMask = 0x08210821;
static uint32 qcolorMask = 0xE79CE79C;
static uint32 qlowpixelMask = 0x18631863;
static uint32 redblueMask = 0xF81F;
static uint32 greenMask = 0x7E0;
static int xsai_depth = 0;

typedef uint32_t elk_pallete_t;
extern elk_pallete_t elkpal[8];

// TODO: Raid allegro4 code for implementation.
int makecol_depth(int depth, int r, int g, int b){
    switch (depth){
        //case 8: return bestfit_color(current_palette, r>>2, g>>2, b>>2);
        case 15: return (r >> 3) + ((g >> 3) << 5) + ((b >> 3) << 10);
        case 16: return (r >> 3) + ((g >> 2) << 5) + ((b >> 3) << 11);
        case 24: return r + (g << 8) + (b << 16);
        case 32: return r + (g << 8) + (b << 16) + (255 << 24);
        default: return 0;
    }
}

int makecol(int r, int g, int b){
    return makecol_depth(32, r, g, b); // TODO: Check code here.
}

int Init_2xSaI(int d)
{

	int minr = 0, ming = 0, minb = 0;
	int i;
	
	if (d != 15 && d != 16 && d != 24 && d != 32)
		return -1;

	/* Get lowest color bit */	
	for (i = 0; i < 255; i++) {
		if (!minr)
			minr = makecol(i, 0, 0);
		if (!ming)
			ming = makecol(0, i, 0);
		if (!minb)
			minb = makecol(0, 0, i);
	}

	colorMask = (makecol_depth(d, 255, 0, 0) - minr) | (makecol_depth(d, 0, 255, 0) - ming) | (makecol_depth(d, 0, 0, 255) - minb);
	lowPixelMask = minr | ming | minb;
	qcolorMask = (makecol_depth(d, 255, 0, 0) - 3 * minr) | (makecol_depth(d, 0, 255, 0) - 3 * ming) | (makecol_depth(d, 0, 0, 255) - 3 * minb);
	qlowpixelMask = (minr * 3) | (ming * 3) | (minb * 3);
	redblueMask = makecol_depth(d, 255, 0, 255);
	greenMask = makecol_depth(d, 0, 255, 0);

	log_debug("Color Depth:      %d\n", d);
	log_debug("Color Mask:       0x%lX\n", colorMask);
	log_debug("Low Pixel Mask:   0x%lX\n", lowPixelMask);
	log_debug("QColor Mask:      0x%lX\n", qcolorMask);
	log_debug("QLow Pixel Mask:  0x%lX\n", qlowpixelMask);
	
	xsai_depth = d;

	return 0;
}


static int GetResult1(uint32 A, uint32 B, uint32 C, uint32 D)
{
	int x = 0;
	int y = 0;
	int r = 0;
	if (A == C)
		x += 1;
	else if (B == C)
		y += 1;
	if (A == D)
		x += 1;
	else if (B == D)
		y += 1;
	if (x <= 1)
		r += 1;
	if (y <= 1)
		r -= 1;
	return r;
}

static int GetResult2(uint32 A, uint32 B, uint32 C, uint32 D, uint32 E)
{
	int x = 0;
	int y = 0;
	int r = 0;
	if (A == C)
		x += 1;
	else if (B == C)
		y += 1;
	if (A == D)
		x += 1;
	else if (B == D)
		y += 1;
	if (x <= 1)
		r -= 1;
	if (y <= 1)
		r += 1;
	return r;
}


#define GET_RESULT(A, B, C, D) ((A != C || A != D) - (B != C || B != D))

#define INTERPOLATE(A, B) (((A & colorMask) >> 1) + ((B & colorMask) >> 1) + (A & B & lowPixelMask))

#define Q_INTERPOLATE(A, B, C, D) ((A & qcolorMask) >> 2) + ((B & qcolorMask) >> 2) + ((C & qcolorMask) >> 2) + ((D & qcolorMask) >> 2) \
	+ ((((A & qlowpixelMask) + (B & qlowpixelMask) + (C & qlowpixelMask) + (D & qlowpixelMask)) >> 2) & qlowpixelMask)


static uint8_t *src_line[4];
static uint8_t *dst_line[2];


void Super2xSaI(uint8_t * elk_screen_data, ALLEGRO_BITMAP * bitmapDest, int w, int h)
{
	Super2xSaI_ex(elk_screen_data, bitmapDest, w, h);
	return;
}

void Super2xSaI_ex(uint8_t * elk_screen_data, ALLEGRO_BITMAP *dest, uint32 width, uint32 height) 
{
	unsigned int x, y;
	uint32_t color[16];

	/* Point to the first 3 lines. */
	src_line[0] = elk_screen_data;
	src_line[1] = elk_screen_data;
	src_line[2] = elk_screen_data + width;
	src_line[3] = elk_screen_data + (width * 2);
	
	/* Set destination */
	//bmp_select(dest);
    uint8_t * region_data_line = NULL;
	ALLEGRO_LOCKED_REGION * destRegion = al_lock_bitmap(dest, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
    region_data_line = (uint8_t *)destRegion->data;

	/* Can we write the results directly? */
	dst_line[0] = region_data_line;
	dst_line[1] = region_data_line + (destRegion->pitch * 2);

	x = 0, y = 0;
	
    uint32_t lbp, lbp1, lbp2;

    lbp  = elkpal[*src_line[0]];
	lbp1 = elkpal[*(src_line[0] + 1)];
	lbp2 = elkpal[*(src_line[0] + 2)];

    color[0] = lbp;       color[1] = color[0];   color[2] = color[0];    color[3] = color[0];
    color[4] = color[0];  color[5] = color[0];   color[6] = lbp1;  color[7] = lbp2;

	lbp  = elkpal[*src_line[2]];
	lbp1 = elkpal[*(src_line[2] + 1)];
	lbp2 = elkpal[*(src_line[2] + 2)];

    color[8] = lbp;     color[9] = color[8];     color[10] = lbp1; color[11] = lbp2;

	lbp  = elkpal[*src_line[3]];
	lbp1 = elkpal[*(src_line[3] + 1)];
	lbp2 = elkpal[*(src_line[3] + 2)];

    color[12] = lbp;    color[13] = color[12];   color[14] = lbp1; color[15] = lbp2;

	for (y = 0; y < height; y++) {
	
		/* Todo: x = width - 2, x = width - 1 */
		
		for (x = 0; x < width; x++) {
			uint32_t product1a, product1b, product2a, product2b;

//---------------------------------------  B0 B1 B2 B3    0  1  2  3
//                                         4  5* 6  S2 -> 4  5* 6  7
//                                         1  2  3  S1    8  9 10 11
//                                         A0 A1 A2 A3   12 13 14 15
//--------------------------------------
			if (color[9] == color[6] && color[5] != color[10]) {
				product2b = color[9];
				product1b = product2b;
			}
			else if (color[5] == color[10] && color[9] != color[6]) {
				product2b = color[5];
				product1b = product2b;
			}
			else if (color[5] == color[10] && color[9] == color[6]) {
				int r = 0;

				r += GET_RESULT(color[6], color[5], color[8], color[13]);
				r += GET_RESULT(color[6], color[5], color[4], color[1]);
				r += GET_RESULT(color[6], color[5], color[14], color[11]);
				r += GET_RESULT(color[6], color[5], color[2], color[7]);

				if (r > 0)
					product1b = color[6];
				else if (r < 0)
					product1b = color[5];
				else
					product1b = INTERPOLATE(color[5], color[6]);
					
				product2b = product1b;

			}
			else {
				if (color[6] == color[10] && color[10] == color[13] && color[9] != color[14] && color[10] != color[12])
					product2b = Q_INTERPOLATE(color[10], color[10], color[10], color[9]);
				else if (color[5] == color[9] && color[9] == color[14] && color[13] != color[10] && color[9] != color[15])
					product2b = Q_INTERPOLATE(color[9], color[9], color[9], color[10]);
				else
					product2b = INTERPOLATE(color[9], color[10]);

				if (color[6] == color[10] && color[6] == color[1] && color[5] != color[2] && color[6] != color[0])
					product1b = Q_INTERPOLATE(color[6], color[6], color[6], color[5]);
				else if (color[5] == color[9] && color[5] == color[2] && color[1] != color[6] && color[5] != color[3])
					product1b = Q_INTERPOLATE(color[6], color[5], color[5], color[5]);
				else
					product1b = INTERPOLATE(color[5], color[6]);
			}

			if (color[5] == color[10] && color[9] != color[6] && color[4] == color[5] && color[5] != color[14])
				product2a = INTERPOLATE(color[9], color[5]);
			else if (color[5] == color[8] && color[6] == color[5] && color[4] != color[9] && color[5] != color[12])
				product2a = INTERPOLATE(color[9], color[5]);
			else
				product2a = color[9];

			if (color[9] == color[6] && color[5] != color[10] && color[8] == color[9] && color[9] != color[2])
				product1a = INTERPOLATE(color[9], color[5]);
			else if (color[4] == color[9] && color[10] == color[9] && color[8] != color[5] && color[9] != color[0])
				product1a = INTERPOLATE(color[9], color[5]);
			else
				product1a = color[5];

            *((uint32_t *) (&dst_line[0][x * 8])) = product1a;
            *((uint32_t *) (&dst_line[0][x * 8 + 4])) = product1b;
            *((uint32_t *) (&dst_line[1][x * 8])) = product2a;
            *((uint32_t *) (&dst_line[1][x * 8 + 4])) = product2b;
			// TODO: Following does not work yet.
			*((uint32_t *) (&dst_line[0][x * 8]) + destRegion->pitch) = product1a;
            *((uint32_t *) (&dst_line[0][x * 8 + 4]) + destRegion->pitch) = product1b;
            *((uint32_t *) (&dst_line[1][x * 8]) + destRegion->pitch) = product2a;
            *((uint32_t *) (&dst_line[1][x * 8 + 4])+ destRegion->pitch) = product2b;
			
			/* Move color matrix forward */
			color[0] = color[1]; color[4] = color[5]; color[8] = color[9];   color[12] = color[13];
			color[1] = color[2]; color[5] = color[6]; color[9] = color[10];  color[13] = color[14];
			color[2] = color[3]; color[6] = color[7]; color[10] = color[11]; color[14] = color[15];
			
			if (x < width - 3) {
				x += 3;

    			color[3]  = elkpal[*(src_line[0] + x)];
				color[7]  = elkpal[*(src_line[1] + x)];
				color[11] = elkpal[*(src_line[2] + x)];
				color[15] = elkpal[*(src_line[3] + x)];

				x -= 3;
			}
		}

		/* We're done with one line, so we shift the source lines up */
		src_line[0] = src_line[1];
		src_line[1] = src_line[2];
		src_line[2] = src_line[3];		

		/* Read next line */
		if (y + 3 >= height)
			src_line[3] = src_line[2];
		else
			src_line[3] = src_line[2] + width;
			
		/* Then shift the color matrix up */
		lbp  = elkpal[*src_line[0]];
		lbp1 = elkpal[*(src_line[0] + 1)];
		lbp2 = elkpal[*(src_line[0] + 2)];

		color[0] = lbp;     color[1] = color[0];    color[2] = lbp1;  color[3] = lbp2;

		lbp  = elkpal[*src_line[1]];
		lbp1 = elkpal[*(src_line[1] + 1)];
		lbp2 = elkpal[*(src_line[1] + 2)];

		color[4] = lbp;     color[5] = color[4];    color[6] = lbp1;  color[7] = lbp2;

		lbp  = elkpal[*src_line[2]];
		lbp1 = elkpal[*(src_line[2] + 1)];
		lbp2 = elkpal[*(src_line[2] + 2)];

		color[8] = lbp;     color[9] = color[9];    color[10] = lbp1; color[11] = lbp2;

		lbp  = elkpal[*src_line[3]];
		lbp1 = elkpal[*(src_line[3] + 1)];
		lbp2 = elkpal[*(src_line[3] + 2)];

		color[12] = lbp;    color[13] = color[12];  color[14] = lbp1; color[15] = lbp2;
		
		/* Write the 2 lines, if not already done so */
		if (y < height - 1) {

			int dest_pitch_y_plus_2 = (destRegion->pitch * ((y + 2) *2));
			int dest_pitch_y_plus_3 = (destRegion->pitch * ((y + 3) *2));

			dst_line[0] = region_data_line + dest_pitch_y_plus_2;
			dst_line[1] = region_data_line + dest_pitch_y_plus_3;

		}
	}
	al_unlock_bitmap(dest);
}



//void SuperEagle(ALLEGRO_BITMAP * bitmapSource, ALLEGRO_BITMAP * bitmapDest, int s_x, int s_y, int d_x, int d_y, int w, int h)
//{
//	int sbpp, dbpp;

//	ALLEGRO_BITMAP * dst2         = NULL;

//	if (!bitmapSource || !bitmapDest)
//		return;

//	sbpp = bitmap_color_depth(bitmapSource);
//	dbpp = bitmap_color_depth(bitmapDest);

//	if ((sbpp != xsai_depth) || (sbpp != dbpp))	/* Must be same color depth */
//		return;
		
//	if (w < 4 || h < 4) {  /* Image is too small to be 2xSaI'ed. */
//		stretch_blit(bitmapSource, bitmapDest, s_x, s_y, w, h, d_x, d_y, w * 2, h * 2);
//		return;
//	}	
	
//	sbpp = BYTES_PER_PIXEL(sbpp);
//	if (d_x || d_y)
//		dst2 = create_sub_bitmap(bitmapDest, d_x, d_y, w * 2, h * 2);
	
//	SuperEagle_ex(bitmapSource->line[s_y] + s_x * sbpp, (unsigned int)(bitmapSource->line[1] - bitmapSource->line[0]), NULL, dst2 ? dst2 : bitmapDest, w, h);
	
//	if (dst2)
//		destroy_bitmap(dst2);
	
//	return;
//}

//void SuperEagle_ex(uint8 *src, uint32 src_pitch, uint8 *unused, ALLEGRO_BITMAP *dest, uint32 width, uint32 height) {

//	int j, v;
//	unsigned int x, y;
//	int sbpp = BYTES_PER_PIXEL(bitmap_color_depth(dest));
//	uint32_t color[12];

	/* Point to the first 3 lines. */
//	src_line[0] = src;
//	src_line[1] = src;
//	src_line[2] = src + src_pitch;
//	src_line[3] = src + src_pitch * 2;
	
	/* Can we write the results directly? */
//	if (is_video_bitmap(dest) || is_planar_bitmap(dest)) {
//		dst_line[0] = malloc(sizeof(char) * sbpp * width);
//		dst_line[1] = malloc(sizeof(char) * sbpp * width);
//		v = 1;
//	}
//	else {
//		dst_line[0] = dest->line[0];
//		dst_line[1] = dest->line[1];
//		v = 0;
//	}
	
	/* Set destination */
//	bmp_select(dest);

//	x = 0, y = 0;
	
//    uint32_t *lbp;
//    lbp = (uint32_t*)src_line[0];
//    color[0] = *lbp;       color[1] = color[0];   color[2] = color[0];    color[3] = color[0];
//    color[4] = *(lbp + 1); color[5] = *(lbp + 2);
//    lbp = (uint32_t*)src_line[2];
//    color[6] = *lbp;     color[7] = color[6];     color[8] = *(lbp + 1); color[9] = *(lbp + 2);
//    lbp = (uint32_t*)src_line[3];
//    color[10] = *lbp;    color[11] = *(lbp + 1);

//	for (y = 0; y < height; y++) {
	
		/* Todo: x = width - 2, x = width - 1 */
		
//		for (x = 0; x < width; x++) {
//			uint32_t product1a, product1b, product2a, product2b;

//---------------------------------------     B1 B2           0  1
//                                         4  5  6  S2 ->  2  3  4  5
//                                         1  2  3  S1     6  7  8  9
//                                            A1 A2          10 11

//			if (color[7] == color[4] && color[3] != color[8]) {
//				product1b = product2a = color[7];

//				if ((color[6] == color[7]) || (color[4] == color[1]))
//					product1a = INTERPOLATE(color[7], INTERPOLATE(color[7], color[3]));
//				else
//					product1a = INTERPOLATE(color[3], color[4]);

//				if ((color[4] == color[5]) || (color[7] == color[10]))
//					product2b = INTERPOLATE(color[7], INTERPOLATE(color[7], color[8]));
//				else
//					product2b = INTERPOLATE(color[7], color[8]);
//			}
//			else if (color[3] == color[8] && color[7] != color[4]) {
//				product2b = product1a = color[3];

//				if ((color[0] == color[3]) || (color[5] == color[9]))
//					product1b = INTERPOLATE(color[3], INTERPOLATE(color[3], color[4]));
//				else
//					product1b = INTERPOLATE(color[3], color[1]);

//				if ((color[8] == color[11]) || (color[2] == color[3]))
//					product2a = INTERPOLATE(color[3], INTERPOLATE(color[3], color[2]));
//				else
//					product2a = INTERPOLATE(color[7], color[8]);

//			}
//			else if (color[3] == color[8] && color[7] == color[4]) {
//				register int r = 0;

//				r += GET_RESULT(color[4], color[3], color[6], color[10]);
//				r += GET_RESULT(color[4], color[3], color[2], color[0]);
//				r += GET_RESULT(color[4], color[3], color[11], color[9]);
//				r += GET_RESULT(color[4], color[3], color[1], color[5]);

//				if (r > 0) {
//					product1b = product2a = color[7];
//					product1a = product2b = INTERPOLATE(color[3], color[4]);
//				}
//				else if (r < 0) {
//					product2b = product1a = color[3];
//					product1b = product2a = INTERPOLATE(color[3], color[4]);
//				}
//				else {
//					product2b = product1a = color[3];
//					product1b = product2a = color[7];
//				}
//			}
//			else {
//				product2b = product1a = INTERPOLATE(color[7], color[4]);
//				product2b = Q_INTERPOLATE(color[8], color[8], color[8], product2b);
//				product1a = Q_INTERPOLATE(color[3], color[3], color[3], product1a);

//				product2a = product1b = INTERPOLATE(color[3], color[8]);
//				product2a = Q_INTERPOLATE(color[7], color[7], color[7], product2a);
//				product1b = Q_INTERPOLATE(color[4], color[4], color[4], product1b);
//			}

//            *((uint32_t *) (&dst_line[0][x * 8])) = product1a;
//            *((uint32_t *) (&dst_line[0][x * 8 + 4])) = product1b;
//            *((uint32_t *) (&dst_line[1][x * 8])) = product2a;
//            *((uint32_t *) (&dst_line[1][x * 8 + 4])) = product2b;
			
			/* Move color matrix forward */
//			color[0] = color[1]; 
//			color[2] = color[3]; color[3] = color[4]; color[4] = color[5];
//			color[6] = color[7]; color[7] = color[8]; color[8] = color[9]; 
//			color[10] = color[11];
			
//			if (x < width - 2) {
//				x += 2;
//                color[1] = *(((uint32_t*)src_line[0]) + x);
//                if (x < width) {
//                    color[5] = *(((uint32_t*)src_line[1]) + x + 1);
//                    color[9] = *(((uint32_t*)src_line[2]) + x + 1);
//                }
//                color[11] = *(((uint32_t*)src_line[3]) + x);
//				x -= 2;
//			}
//		}

		/* We're done with one line, so we shift the source lines up */
//		src_line[0] = src_line[1];
//		src_line[1] = src_line[2];
//		src_line[2] = src_line[3];		

		/* Read next line */
//		if (y + 3 >= height)
//			src_line[3] = src_line[2];
//		else
//			src_line[3] = src_line[2] + src_pitch;
			
		/* Then shift the color matrix up */
//        uint32_t *lbp;
//        lbp = (uint32_t*)src_line[0];
//        color[0] = *lbp;     color[1] = *(lbp + 1);
//        lbp = (uint32_t*)src_line[1];
//        color[2] = *lbp;     color[3] = color[2];    color[4] = *(lbp + 1);  color[5] = *(lbp + 2);
//        lbp = (uint32_t*)src_line[2];
//        color[6] = *lbp;     color[7] = color[6];    color[8] = *(lbp + 1);  color[9] = *(lbp + 2);
//        lbp = (uint32_t*)src_line[3];
//        color[10] = *lbp;    color[11] = *(lbp + 1);

		/* Write the 2 lines, if not already done so */
//		if (v) {
//			uint32_t dst_addr;
		
//			dst_addr = bmp_write_line(dest, y * 2);
//			for (j = 0; j < dest->w * sbpp; j += sizeof(long))
//				bmp_write32(dst_addr + j, *((uint32_t *) (dst_line[0] + j)));
				
//			dst_addr = bmp_write_line(dest, y * 2 + 1);
//			for (j = 0; j < dest->w * sbpp; j += sizeof(long))
//				bmp_write32(dst_addr + j, *((uint32_t *) (dst_line[1] + j)));
//		}
//		else {
//			if (y < height - 1) {
//				dst_line[0] = dest->line[y * 2 + 2];
//				dst_line[1] = dest->line[y * 2 + 3];
//			}
//		}
//	}
//	bmp_unwrite_line(dest);
	
//	if (v) {
//		free(dst_line[0]);
//		free(dst_line[1]);
//	}
//}
