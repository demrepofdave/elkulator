/*Elkulator v1.0 by Sarah Walker
  PAL filter*/

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


//#define OLD_CALC
#define NEW_CALC

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/
#if defined (OLD_CALC)
al_fixed ACoef[2],ACoef2[3];
al_fixed BCoef[2],BCoef2[3];
#endif

#if defined (NEW_CALC)    
float f_ACoef[2],f_ACoef2[3];
float f_BCoef[2],f_BCoef2[3];
#endif

#if defined (OLD_CALC)
al_fixed constr;
al_fixed constb;
#endif

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

static inline void log_cmp_float(const char * function, const char * msg, al_fixed al5float, float normalfloat)
{
    al_fixed corr_al_float = 0;
    if(al5float != 0 || normalfloat != 0)
    {
        if(al5float < 0)
        {
            corr_al_float = al5float * -1;
        }
        else
        {
            corr_al_float = al5float;
        }
        log_debug("%s : %s = %d.%d : %f", function, msg, corr_al_float >> 16, corr_al_float & 0xFFFF, normalfloat);
    }
}

void initcoef()
{
#if defined (OLD_CALC)    
    ACoef[0]=ACoef[1]=al_ftofix(0.13673463961326307000);
    BCoef[0]=al_itofix(1);
    BCoef[1]=al_ftofix(-0.72654252800536090000);
    ACoef2[0]=al_ftofix(0.29289321881333563000);
    ACoef2[1]=al_ftofix(0.58578643762667126000);
    ACoef2[2]=al_ftofix(0.29289321881333563000);
    BCoef2[2]=al_ftofix(0.17157287525380990000);
#endif

#if defined (NEW_CALC)
    f_ACoef[0]  = 0.13673463961326307000;
    f_ACoef[1]  = 0.13673463961326307000;
    f_BCoef[0]  = 1;
    f_BCoef[1]  = -0.72654252800536090000;
    f_ACoef2[0] = 0.29289321881333563000;
    f_ACoef2[1] = 0.58578643762667126000;
    f_ACoef2[2] = 0.29289321881333563000;
    f_BCoef2[2] = 0.17157287525380990000;
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
    log_cmp_float(__FUNCTION__, "Acoef[0] = ", ACoef[0], f_ACoef[0]);
    log_cmp_float(__FUNCTION__, "Acoef[1] = ", ACoef[1], f_ACoef[1]);

    log_cmp_float(__FUNCTION__, "Acoef2[0] = ", ACoef2[0], f_ACoef2[0]);
    log_cmp_float(__FUNCTION__, "Acoef2[1] = ", ACoef2[1], f_ACoef2[1]);
    log_cmp_float(__FUNCTION__, "Acoef2[1] = ", ACoef2[2], f_ACoef2[2]);

    log_cmp_float(__FUNCTION__, "Bcoef[0] = ", BCoef[0], f_BCoef[0]);
    log_cmp_float(__FUNCTION__, "Bcoef[1] = ", BCoef[1], f_BCoef[1]);

    log_cmp_float(__FUNCTION__, "Bcoef2[2] = ", BCoef2[2], f_BCoef2[2]);
#endif
}


#define NCoef 2

#if defined (OLD_CALC)
al_fixed yy[NCoef+2]; //output samples
al_fixed yx[NCoef+2]; //input samples
int ycount=0;
#endif

#if defined (NEW_CALC)
float f_yy[NCoef+2]; //output samples
float f_yx[NCoef+2]; //input samples
int f_i_ycount=0;
#endif

#if defined (OLD_CALC)
static inline al_fixed iir(al_fixed NewSample)
{
    yx[ycount] = NewSample;
    yy[ycount] = al_fixmul(ACoef2[0],yx[ycount]);
    yy[ycount] += al_fixmul(ACoef2[1],yx[(ycount+1)&3]);// - BCoef2[1] * yy[(ycount+1)&3];
    yy[ycount] += al_fixmul(ACoef2[2],yx[(ycount+2)&3]) - al_fixmul(BCoef2[2],yy[(ycount+2)&3]);

    ycount=(ycount-1)&3;
    return yy[(ycount+1)&3];
}
#endif

#if defined (NEW_CALC)
static inline float f_iir(float f_newsample)
{
    #if defined (OLD_CALC) && defined (NEW_CALC)
    if(f_newsample)
    {
        log_debug("f_iir newsample=%f", f_newsample);
    }
    #endif
    f_yx[f_i_ycount] = f_newsample;
    #if defined (OLD_CALC) && defined (NEW_CALC)
    if(f_newsample)
    {
        log_debug("f_iir f_yx[%d]=%f", f_i_ycount, f_yx[f_i_ycount]);
    }
    #endif
    f_yy[f_i_ycount] = f_ACoef2[0] * f_yx[f_i_ycount];
    #if defined (OLD_CALC) && defined (NEW_CALC)
    if(f_newsample)
    {
        log_debug("f_iir f_yy[%d]=%f (ACoef2)", f_i_ycount, f_yy[f_i_ycount]);
    }
    #endif
    f_yy[f_i_ycount] += (f_ACoef2[1] * f_yx[(f_i_ycount+1)&3]);// - BCoef2[1] * yy[(f_i_ycount+1)&3];
    #if defined (OLD_CALC) && defined (NEW_CALC)
    if(f_newsample)
    {
        log_debug("f_iir f_yy[%d]=%f (yx1)", f_i_ycount, f_yy[f_i_ycount]);
    }
    #endif
    f_yy[f_i_ycount] += (f_ACoef2[2] * f_yx[(f_i_ycount+2)&3]) - (f_BCoef2[2] * f_yy[(f_i_ycount+2)&3]);

    #if defined (OLD_CALC) && defined (NEW_CALC)
    if(f_newsample)
    {
        log_debug("f_iir f_yy[%d]=%f (yx2)", f_i_ycount, f_yy[f_i_ycount]);
    }
    #endif

    f_i_ycount=(f_i_ycount-1)&3;

    #if defined (OLD_CALC) && defined (NEW_CALC)
    if(f_newsample)
    {
        log_debug("f_iir f_yy[%d+1]&3]=%f", (f_i_ycount+1)&3, f_yy[(f_i_ycount+1)&3]);
    }
    #endif
    return f_yy[(f_i_ycount+1)&3];
}

const float f_constr = -0.509f;
const float f_constb = 0.194;

float f_ry[2]; //output samples
float f_rx[2]; //input samples
int f_rycount=0;

#endif

#if defined (OLD_CALC)
al_fixed ry[2]; //output samples
al_fixed rx[2]; //input samples
int rycount=0;

static inline al_fixed firry(al_fixed NewSample)
{
    //Calculate the new output
    rx[rycount] = al_fixmul(ACoef[0],NewSample);
    ry[rycount] = rx[0]+rx[1];
    ry[rycount] -= al_fixmul(BCoef[1],ry[rycount^1]);
    rycount^=1;
    return ry[rycount^1];
}
#endif

#if defined (NEW_CALC)
static inline float f_firry(float f_newsample)
{
    //Calculate the new output
    f_rx[f_rycount] = (f_ACoef[0] * f_newsample);
    f_ry[f_rycount] = f_rx[0]+f_rx[1];
    f_ry[f_rycount] -= (f_BCoef[1] * f_ry[f_rycount^1]);
    f_rycount^=1;
    return f_ry[f_rycount^1];
}
float f_by[2]; //output samples
float f_bx[2]; //input samples
int f_bycount = 0;

#endif

#if defined (OLD_CALC)
al_fixed by[2]; //output samples
al_fixed bx[2]; //input samples
int bycount=0;

static inline al_fixed firby(al_fixed NewSample) 
{
    //Calculate the new output
    bx[bycount] = al_fixmul(ACoef[0],NewSample);
    by[bycount] = bx[0]+bx[1];
    by[bycount] -= al_fixmul(BCoef[1],by[bycount^1]);
    bycount^=1;
    return by[bycount^1];
}

al_fixed ytable[8];

#endif

#if defined (NEW_CALC)
static inline float f_firby(float f_newsample) 
{
    //Calculate the new output
    f_bx[f_bycount] = (f_ACoef[0] * f_newsample);
    f_by[f_bycount] = f_bx[0]+f_bx[1];
    f_by[f_bycount] -= (f_BCoef[1] * f_by[f_bycount^1]);
    f_bycount^=1;

    //log_cmp_float(__FUNCTION__, "by[bycount^1] = ", by[bycount^1], f_by[bycount^1]);

    return f_by[f_bycount^1];
}

float f_ytable[8]; // Convert to float alongside for now.

#endif




/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void initpaltables()
{
    int c;
    int r,g,b;
    // Init constants.
#if defined (OLD_CALC)
    constr = al_ftofix(-0.509f);
    constb = al_ftofix(0.194);
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
    log_cmp_float(__FUNCTION__, "constr = ", constr, f_constr);
    log_cmp_float(__FUNCTION__, "constb = ", constb, f_constb);
#endif

    for (c=0;c<8;c++)
    {
        r = (c&1)?255:0;
        g = (c&2)?255:0;
        b = (c&4)?255:0;

#if defined (OLD_CALC)
        ytable[c]=al_ftofix((r*0.299f)+(g*0.587)+(b*0.114));
#endif

#if defined (NEW_CALC)
        f_ytable[c] = (r*0.299f)+(g*0.587)+(b*0.114);
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
        log_cmp_float(__FUNCTION__, "ytable = ", ytable[c], f_ytable[c]);
#endif
    }
    initcoef();
}


void palfilter(ALLEGRO_BITMAP * destBitmap)
{
    int x,y;
    uint32_t c;
#if defined (OLD_CALC)    
    int r,g,b;
    al_fixed paly,palry,palby;
#endif

#if defined (NEW_CALC)
    float f_r,f_g,f_b;
    float f_paly, f_palry, f_palby;
#endif
    char * region_data = NULL;

    ALLEGRO_LOCKED_REGION * destRegion = al_lock_bitmap(destBitmap, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
    log_time_mark("palfilt - locked");

    for (y=0;y<512;y++)
    {
        region_data = (char *)destRegion->data + (destRegion->pitch * y);
#if defined (OLD_CALC)
        yx[2]=yx[1]=yx[0]=0;
        yy[2]=yy[1]=yy[0]=0;
        rx[0]=ry[0]=rx[1]=ry[1]=0;
        bx[0]=by[0]=bx[1]=by[1]=0;
#endif

#if defined (NEW_CALC)
        f_yx[2]=f_yx[1]=f_yx[0]=0;
        f_yy[2]=f_yy[1]=f_yy[0]=0;
        f_rx[0]=f_ry[0]=f_rx[1]=f_ry[1]=0;
        f_bx[0]=f_by[0]=f_bx[1]=f_by[1]=0;
#endif
//                iir(0,1);
        for (x=0;x<640;x++)
        {
            c = video_get_pixel(y >> 1, x);

#if defined (OLD_CALC)            
            r = (c & 1)?255:0; 
            g = (c & 2)?255:0; 
            b = (c & 4)?255:0;
#endif


#if defined (NEW_CALC)
            f_r = (c & 1)?255:0; 
            f_g = (c & 2)?255:0; 
            f_b = (c & 4)?255:0;
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
            log_cmp_float(__FUNCTION__, "Begin x loop", r, f_r);
#endif

#if defined (OLD_CALC)
            paly=ytable[c];
            palry=al_itofix(r)-paly;
            palby=al_itofix(b)-paly;
#endif

#if defined (NEW_CALC)
            f_paly = f_ytable[c];
            f_palry= f_r - f_paly; // This is going wrong.
            f_palby= f_b - f_paly; // This is going wrong.
#endif
 
#if defined (OLD_CALC) && defined (NEW_CALC)
            log_cmp_float(__FUNCTION__, "f_paly", paly, f_paly);
            log_cmp_float(__FUNCTION__, "f_palry", palry, f_palry);
            log_cmp_float(__FUNCTION__, "f_palby", palby, f_palby);
#endif

#if defined (OLD_CALC)
            paly=iir(paly);
#endif
#if defined (NEW_CALC)
            f_paly=f_iir(f_paly);
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
            log_cmp_float(__FUNCTION__, "post iir f_paly", paly, f_paly);
#endif

#if defined (OLD_CALC)
            if (paly>(255<<16)) paly=255<<16;
            if (paly<0)   paly=0;
#endif
#if defined (NEW_CALC)
            if (f_paly > 255) f_paly=255;
            if (f_paly < 0)   f_paly=0;
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
            log_cmp_float(__FUNCTION__, "adju iir f_paly", paly, f_paly);
#endif

#if defined (OLD_CALC)            
            palry   = firry(palry);
            palby   = firby(palby);
#endif
#if defined (NEW_CALC)
            f_palry = f_firry(f_palry);
            f_palby = f_firby(f_palby);
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
            log_cmp_float(__FUNCTION__, "firry f_palry", palry, f_palry);
            log_cmp_float(__FUNCTION__, "firby f_palby", palby, f_palby);
#endif            

#if defined (OLD_CALC)
            r=al_fixtoi(palry+paly);
            b=al_fixtoi(palby+paly);
            g=al_fixtoi((al_fixmul(constr,palry)-al_fixmul(constb,palby))+paly);
#endif

#if defined (NEW_CALC)
            f_r = f_palry+f_paly; 
            f_g = f_palby+f_paly; 
            f_b = (f_constr * f_palry) - (f_constb * f_palby) + f_paly;
#endif

#if defined (OLD_CALC) && defined (NEW_CALC)
            if(f_r > 0 || f_g > 0 || f_b > 0 ||
               r > 0   || g > 0   || b > 0)
            {
                log_debug("%s : end r = %d : %f", __FUNCTION__, r, f_r);
                log_debug("%s : end g = %d : %f", __FUNCTION__, g, f_g);
                log_debug("%s : end b = %d : %f", __FUNCTION__, b, f_b);
            }
#endif

#if defined (OLD_CALC)            
            if (r>255) r=255;
            if (r<0)   r=0;
            if (g>255) g=255;
            if (g<0)   g=0;
            if (b>255) b=255;
            if (b<0)   b=0;
#endif            

#if defined (NEW_CALC)
            if (f_r>255) f_r=255;
            if (f_r<0)   f_r=0;
            if (f_g>255) f_g=255;
            if (f_g<0)   f_g=0;
            if (f_b>255) f_b=255;
            if (f_b<0)   f_b=0;
#endif

#if defined (OLD_CALC)
            c = 0xff000000 | (r << 16) | (g << 8) | b;
#else
            c = 0xff000000 | ((int)f_r << 16) | ((int)f_g << 8) | (int) f_b;
#endif

            // Plot to the correct pixel
            *((uint32_t *)((char *)region_data)) = c;

            region_data += destRegion->pixel_size;
            //video_put_pixel_rgb(destRegion, y, x, c);
        }
    }
    al_unlock_bitmap(destBitmap);
    log_time_mark("palfilt - unlock and end");
    //log_time_display();
}
