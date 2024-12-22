/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * palfilt.c
 * 
 * Contains all functions to initialize and use the palfilter for
 * electrn video output.
 * 
 * This is the allegro 5 implementation of the pal filter.
 *
 */

/******************************************************************************
* Include files
*******************************************************************************/
#include <allegro5/allegro.h>
#include "video_internal.h"
#include "logger.h"


/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

float ACoef[2],ACoef2[3];
float BCoef[2],BCoef2[3];


/******************************************************************************
* Private Function Definitions
*******************************************************************************/

void initcoef()
{
    ACoef[0]  = 0.13673463961326307000;
    ACoef[1]  = 0.13673463961326307000;
    BCoef[0]  = 1;
    BCoef[1]  = -0.72654252800536090000;
    ACoef2[0] = 0.29289321881333563000;
    ACoef2[1] = 0.58578643762667126000;
    ACoef2[2] = 0.29289321881333563000;
    BCoef2[2] = 0.17157287525380990000;
}


#define NCoef 2

float yy[NCoef+2]; //output samples
float yx[NCoef+2]; //input samples
int ycount=0;

static inline float iir(float newsample)
{
    yx[ycount] = newsample;
    yy[ycount] = ACoef2[0] * yx[ycount];
    yy[ycount] += (ACoef2[1] * yx[(ycount+1)&3]);// - BCoef2[1] * yy[(ycount+1)&3];
    yy[ycount] += (ACoef2[2] * yx[(ycount+2)&3]) - (BCoef2[2] * yy[(ycount+2)&3]);

    ycount=(ycount-1)&3;

    return yy[(ycount+1)&3];
}

const float constr = -0.509f;
const float constb = 0.194;

float ry[2]; //output samples
float rx[2]; //input samples
int rycount=0;


static inline float firry(float newsample)
{
    //Calculate the new output
    rx[rycount] = (ACoef[0] * newsample);
    ry[rycount] = rx[0]+rx[1];
    ry[rycount] -= (BCoef[1] * ry[rycount^1]);
    rycount^=1;
    return ry[rycount^1];
}
float by[2]; //output samples
float bx[2]; //input samples
int f_bycount = 0;

static inline float firby(float newsample) 
{
    //Calculate the new output
    bx[f_bycount] = (ACoef[0] * newsample);
    by[f_bycount] = bx[0]+bx[1];
    by[f_bycount] -= (BCoef[1] * by[f_bycount^1]);
    f_bycount^=1;

    //log_cmp_float(__FUNCTION__, "by[bycount^1] = ", by[bycount^1], by[bycount^1]);

    return by[f_bycount^1];
}

float ytable[8]; // Convert to float alongside for now.


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void initpaltables()
{
    int c;
    int red,green,blue;
    // Init constants.

    for (c=0;c<8;c++)
    {
        red   = (c&1)?255:0;
        green = (c&2)?255:0;
        blue  = (c&4)?255:0;
        ytable[c] = (red*0.299f)+(green*0.587)+(blue*0.114);
    }
    initcoef();
}


void palfilter(ALLEGRO_BITMAP * destBitmap, uint8_t * elk_screen_data)
{
    int x,y;
    uint32_t c;
    float red,green,blue;
    float paly, palry, palby;
    char * region_data = NULL;

    ALLEGRO_LOCKED_REGION * destRegion = al_lock_bitmap(destBitmap, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
    //log_time_mark("palfilt - locked");

    for (y=0;y<512;y++)
    {
        region_data = (char *)destRegion->data + (destRegion->pitch * y);

        yx[2]=yx[1]=yx[0]=0;
        yy[2]=yy[1]=yy[0]=0;
        rx[0]=ry[0]=rx[1]=ry[1]=0;
        bx[0]=by[0]=bx[1]=by[1]=0;
        for (x=0;x<640;x++)
        {
            c = *(elk_screen_data + ((y >> 1) * 640) + x);

            red = (c & 1)?255:0; 
            green = (c & 2)?255:0; 
            blue = (c & 4)?255:0;

            paly = ytable[c];
            palry= red - paly;
            palby= blue - paly;
 
            paly=iir(paly);


            if (paly > 255) paly=255;
            if (paly < 0)   paly=0;

            palry = firry(palry);
            palby = firby(palby);

            red   = palry+paly; 
            blue  = palby+paly; 
            green = (constr * palry) - (constb * palby) + paly;

            if (red>255) red=255;
            if (red<0)   red=0;
            if (green>255) green=255;
            if (green<0)   green=0;
            if (blue>255) blue=255;
            if (blue<0)   blue=0;

            c = 0xff000000 | ((int)red << 16) | ((int)green << 8) | (int) blue;

            // Plot to the correct pixel
            *((uint32_t *)((char *)region_data)) = c;

            region_data += destRegion->pixel_size;
        }
    }
    al_unlock_bitmap(destBitmap);
    //log_time_mark("palfilt - unlock and end");
}
