/*Elkulator v1.0 by Sarah Walker
  PAL filter*/
#include <allegro.h>
#include "elk.h"

fixed ACoef[2],ACoef2[3];
fixed BCoef[2],BCoef2[3];

void initcoef()
{
        ACoef[0]=ACoef[1]=ftofix(0.13673463961326307000);
        BCoef[0]=itofix(1);
        BCoef[1]=ftofix(-0.72654252800536090000);
        ACoef2[0]=ftofix(0.29289321881333563000);
        ACoef2[1]=ftofix(0.58578643762667126000);
        ACoef2[2]=ftofix(0.29289321881333563000);
        BCoef2[2]=ftofix(0.17157287525380990000);
        rpclog("%08X %08X %08X %08X\n",ACoef2[0],ACoef2[1],ACoef2[2],BCoef2[2]);
}


#define NCoef 2

fixed yy[NCoef+2]; //output samples
fixed yx[NCoef+2]; //input samples
int ycount=0;
static inline fixed iir(fixed NewSample)
{
        yx[ycount] = NewSample;
        yy[ycount] = fixmul(ACoef2[0],yx[ycount]);
        yy[ycount] += fixmul(ACoef2[1],yx[(ycount+1)&3]);// - BCoef2[1] * yy[(ycount+1)&3];
        yy[ycount] += fixmul(ACoef2[2],yx[(ycount+2)&3]) - fixmul(BCoef2[2],yy[(ycount+2)&3]);
        ycount=(ycount-1)&3;
        return yy[(ycount+1)&3];
}

fixed ry[2]; //output samples
fixed rx[2]; //input samples
int rycount=0;

static inline fixed firry(fixed NewSample) {
    //Calculate the new output
    rx[rycount] = fixmul(ACoef[0],NewSample);
    ry[rycount] = rx[0]+rx[1];
    ry[rycount] -= fixmul(BCoef[1],ry[rycount^1]);
    rycount^=1;
    return ry[rycount^1];
}

fixed by[2]; //output samples
fixed bx[2]; //input samples
int bycount=0;

static inline fixed firby(fixed NewSample) {
    //Calculate the new output
    bx[bycount] = fixmul(ACoef[0],NewSample);
    by[bycount] = bx[0]+bx[1];
    by[bycount] -= fixmul(BCoef[1],by[bycount^1]);
    bycount^=1;
    return by[bycount^1];
}

int rtable[8],gtable[8],btable[8];
fixed ytable[8];

void initpaltables()
{
        int c;
        for (c=0;c<8;c++)
        {
                rtable[c]=(c&1)?255:0;
                gtable[c]=(c&2)?255:0;
                btable[c]=(c&4)?255:0;
                ytable[c]=ftofix((rtable[c]*0.299f)+(gtable[c]*0.587)+(btable[c]*0.114));
        }
        initcoef();
}

void palfilter(BITMAP *src, BITMAP *dest, int depth)
{
        int x,y,c;
        int r,g,b;
        fixed constr=ftofix(-0.509f),constb=ftofix(0.194);
        fixed paly,palry,palby;
        for (y=0;y<512;y+=2)
        {
                yx[2]=yx[1]=yx[0]=0;
                yy[2]=yy[1]=yy[0]=0;
                rx[0]=ry[0]=rx[1]=ry[1]=0;
                bx[0]=by[0]=bx[1]=by[1]=0;
//                iir(0,1);
                if (depth==32)
                {
                        for (x=0;x<640;x++)
                        {
                                c=src->line[y][x]&7;
                                r=(c&1)?255:0; g=(c&2)?255:0; b=(c&4)?255:0;
                                paly=ytable[c];

                                palry=itofix(r)-paly;
                                palby=itofix(b)-paly;

                                paly=iir(paly);
                                if (paly>(255<<16)) paly=255<<16;
                                if (paly<0)   paly=0;

                                palry=firry(palry);
                                palby=firby(palby);

                                r=fixtoi(palry+paly);
                                b=fixtoi(palby+paly);
                                g=fixtoi((fixmul(constr,palry)-fixmul(constb,palby))+paly);

                                if (r>255) r=255;
                                if (r<0)   r=0;
                                if (g>255) g=255;
                                if (g<0)   g=0;
                                if (b>255) b=255;
                                if (b<0)   b=0;

                                ((uint32_t *)dest->line[y])[x]=makecol(r,g,b);
                        }
                }
                else
                {
                        for (x=0;x<640;x++)
                        {
                                c=src->line[y][x]&7;
                                r=(c&1)?255:0; g=(c&2)?255:0; b=(c&4)?255:0;
                                paly=ytable[c];

                                palry=itofix(r)-paly;
                                palby=itofix(b)-paly;

                                paly=iir(paly);
                                if (paly>(255<<16)) paly=255<<16;
                                if (paly<0)   paly=0;

                                palry=firry(palry);
                                palby=firby(palby);

                                r=fixtoi(palry+paly);
                                b=fixtoi(palby+paly);
                                g=fixtoi((fixmul(constr,palry)-fixmul(constb,palby))+paly);

                                if (r>255) r=255;
                                if (r<0)   r=0;
                                if (g>255) g=255;
                                if (g<0)   g=0;
                                if (b>255) b=255;
                                if (b<0)   b=0;

                                ((uint16_t *)dest->line[y])[x]=makecol(r,g,b);
                        }
                }
        }
}
