
#include <string.h>

#ifdef HAL_ALLEGRO_5
    #include <allegro5/allegro.h>
#else
    #include <allegro.h>
	#include <allegro/internal/aintern.h>
#endif

#include "hal/2xsai.h"

//#define TRACE rpclog
#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char

int Init_2xSaI(int d)
{

	return 0;
}


//void Super2xSaI(BITMAP * src, BITMAP * dest, int s_x, int s_y, int d_x, int d_y, int w, int h)
//{
//	
//	return;
//}

//void Super2xSaI_ex(uint8 *src, uint32 src_pitch, uint8 *unused, BITMAP *dest, uint32 width, uint32 height)
//{
//	return;
//}

//void SuperEagle(BITMAP * src, BITMAP * dest, int s_x, int s_y, int d_x, int d_y, int w, int h)
//{
//	return;
//}

//void SuperEagle_ex(uint8 *src, uint32 src_pitch, uint8 *unused, BITMAP *dest, uint32 width, uint32 height)
//{
//	return;
//}
