extern int pauseit;
extern int output;
static int reallyfasttapebreak;
/*Elkulator v1.0 by Sarah Walker*/
/*UEF handling*/
#include <allegro.h>
#include <stdio.h>
#include <zlib.h>
#include "elk.h"

#define INT_HIGHTONE 0x40

static int tapelcount,tapellatch,pps;
int intone=0;
gzFile uef;
extern int cswena;

int inchunk=0,chunkid=0,chunklen=0;
int chunkpos=0,chunkdatabits=8;
float chunkf;

void openuef(char *fn)
{
      int c;
      if (uef)
         gzclose(uef);
      uef=gzopen(fn,"rb");
      if (!uef) return;
      for (c=0;c<12;c++)
          gzgetc(uef);
      inchunk=chunklen=chunkid=0;
      tapellatch=2000000/(1200/10);
      tapelcount=0;
      pps=120;
      cswena=0;
      chunkpos=0;
      chunkdatabits=8;
      intone=0;
}

int ueffileopen()
{
        if (!uef)
           return 0;
        return 1;
}

void polluef()
{
        int c;
        uint32_t templ;
        float *tempf;
        uint8_t temp;
        if (!uef)
           return;
        polltop:
        if (!inchunk)
        {
//                printf("%i ",gztell(uef));
                gzread(uef,&chunkid,2);
                gzread(uef,&chunklen,4);
                if (gzeof(uef))
                {
                        gzseek(uef,12,SEEK_SET);
                        gzread(uef,&chunkid,2);
                        gzread(uef,&chunklen,4);
                }
                inchunk=1;
                chunkpos=0;
                rpclog("Chunk ID %04X len %i\n",chunkid,chunklen);
                pauseit=0;
        }
        switch (chunkid)
        {
                case 0x000: /*Origin*/
                for (c=0;c<chunklen;c++)
                    gzgetc(uef);
                inchunk=0;
                return;

                case 0x005: /*Target platform*/
                for (c=0;c<chunklen;c++)
                    gzgetc(uef);
                inchunk=0;
                return;

                case 0x100: /*Raw data*/
//                if (output) output--;
//                output=1;
                chunklen--;
                if (!chunklen)
                {
                        inchunk=0;
                        output=0;
                }
                receive(gzgetc(uef));
                reallyfasttapebreak=1;
                return;

                case 0x104: /*Defined data*/
                if (!chunkpos)
                {
                        chunkdatabits=gzgetc(uef);
                        gzgetc(uef);
                        gzgetc(uef);
                        chunklen-=3;
                        chunkpos=1;
                }
                else
                {
                        chunklen--;
                        if (chunklen<=0)
                           inchunk=0;
                        temp=gzgetc(uef);
//                        printf("%i : %i %02X\n",gztell(uef),chunklen,temp);
                        if (chunkdatabits==7) receive(temp&0x7F);
                        else                  receive(temp);
                        reallyfasttapebreak=1;
                }
                return;

                case 0x110: /*High tone*/
//                rpclog("High tone start at %04X\n",pc);
//                output=1;
                if (!intone)
                {
                        templ=gzgetc(uef); templ|=(gzgetc(uef)<<8);
/*                        intone=templ>>2;
                        if (!intone) intone=1;
                        printf("High tone %04X\n",templ);
			if (tapespeed)
			{*/
	                        if (templ>20)
	                        {
	                                intula(INT_HIGHTONE);
	                                intone=6;
	                                reallyfasttapebreak=1;
	                        }
	                        else
	                        {
	                                inchunk=0;
	                                pauseit=1;
				}
//                        }
                }
                else
                {
                        intone--;
                        if (intone==0)
                        {
                                inchunk=0;
                        }
                }
                return;

                case 0x111: /*High tone with dummy byte*/
                if (!intone)
                {
                        intula(INT_HIGHTONE);
                        reallyfasttapebreak=1;
                        intone=3;
                }
                else
                {
                        if (intone==4)
                        {
                                intula(INT_HIGHTONE);
                                reallyfasttapebreak=1;
                        }
                        intone--;
                        if (intone==0 && inchunk==2)
                        {
                                inchunk=0;
                                gzgetc(uef); gzgetc(uef);
                                gzgetc(uef); gzgetc(uef);
                        }
                        else if (!intone)
                        {
                                inchunk=2;
                                intone=4;
                                receive(0xAA);
                                reallyfasttapebreak=1;
                        }
                }
                return;

                case 0x112: /*Gap*/
                if (!intone)
                {
                        templ=gzgetc(uef); templ|=(gzgetc(uef)<<8);
                        intone=templ>>2;
                        if (!intone) intone=1;
                        if (tapespeed) intone=3;
                }
                else
                {
                        intone--;
                        if (intone==0)
                        {
                                inchunk=0;
//                                gzgetc(uef); gzgetc(uef);
                        }
                }
                return;

                case 0x113: /*Float baud rate*/
                templ=gzgetc(uef);
                templ|=(gzgetc(uef)<<8);
                templ|=(gzgetc(uef)<<16);
                templ|=(gzgetc(uef)<<24);
                tempf=(float *)&templ;
                tapellatch=2000000/((*tempf)/10);
                pps=(*tempf)/10;
                inchunk=0;
                goto polltop;

                case 0x116: /*Float gap*/
                if (!chunkpos)
                {
                        templ=gzgetc(uef);
                        templ|=(gzgetc(uef)<<8);
                        templ|=(gzgetc(uef)<<16);
                        templ|=(gzgetc(uef)<<24);
                        tempf=(float *)&templ;
                        chunkf=*tempf;
//                        printf("Gap %f\n",chunkf);
                        chunkpos=1;
                }
                else
                {
//                        printf("Gap now %f\n",chunkf);
                        chunkf-=((float)1/(float)pps);
                        if (chunkf<=0) inchunk=0;
                }
                return;

                case 0x114: /*Security waves*/
                case 0x115: /*Polarity change*/
//                default:
                for (c=0;c<chunklen;c++)
                    gzgetc(uef);
                inchunk=0;
                return;

//116 : float gap
//113 : float baud rate

        }
        allegro_exit();
        printf("Bad chunk ID %04X length %i\n",chunkid,chunklen);
        exit(-1);
}

void closeuef()
{
        if (uef)
	{
           	gzclose(uef);
		uef=NULL;
	}
}
