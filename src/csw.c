/*Elkulator v1.0 by Sarah Walker*/
/*CSW handling*/
#include <stdio.h>
#include <zlib.h>
#include "elk.h"

#define HIGHTONE 0x40
static int reallyfasttapebreak;
int cintone=1,cindat=0,datbits=0,enddat=0;
FILE *cswf;
char csws[256];
FILE *cswlog;
uint8_t *cswdat;
int cswpoint;
uint8_t cswhead[0x34];
static int tapelcount,tapellatch,pps;
int cswena;
int cswskip=0;
int tapespeed=0;

void opencsw(char *fn)
{
        int end,c;
        uint32_t destlen=16*1024*1024;
        uint8_t *tempin;
        if (cswf) fclose(cswf);
        if (cswdat) free(cswdat);
        cswena=1;
        /*Allocate buffer*/
        cswdat=malloc(16*1024*1024);
        /*Open file and get size*/
        cswf=fopen(fn,"rb");
        if (!cswf)
        {
                free(cswdat);
                cswf=(FILE *)NULL;
                cswdat=(uint8_t *)NULL;
                return;
        }
        fseek(cswf,-1,SEEK_END);
        end=ftell(cswf);
        fseek(cswf,0,SEEK_SET);
        /*Read header*/
        fread(cswhead,0x34,1,cswf);
        for (c=0;c<cswhead[0x23];c++) getc(cswf);
        /*Allocate temporary memory and read file into memory*/
        end-=ftell(cswf);
        tempin=malloc(end);
        fread(tempin,end,1,cswf);
        fclose(cswf);
//        sprintf(csws,"Decompressing %i %i\n",destlen,end);
//        fputs(csws,cswlog);
        /*Decompress*/
        uncompress(cswdat,(uLongf *)&destlen,tempin,end);
        free(tempin);
        /*Reset data pointer*/
        cswpoint=-1;
//        dcd();
      tapellatch=(1000000/(1200/10))/64;
      tapelcount=0;
      pps=120;
}
void closecsw()
{
        if (cswf) fclose(cswf);
        if (cswdat) free(cswdat);
        cswdat=NULL;
        cswf=NULL;
}
void pollcsw()
{
        int c;
        uint8_t dat;
//        sprintf(csws,"CSW poll %08X %i\n",cswdat,cswpoint);
//        fputs(csws,cswlog);
        if (!cswdat) return;
        if (cswpoint==-1)
        {
                cswpoint=0;
                intula(HIGHTONE);
                return;
        }
/*        if (!cswf)
        {
                cswf=fopen("3dgran.out","rb");
//                fseek(cswf,0x500,SEEK_SET);
                if (!cswlog) cswlog=fopen("cswlog.txt","wt");
        dcd();
        }*/
        for (c=0;c<10;c++)
        {
                if (cswpoint>=(16*1024*1024)) cswpoint=0;
                dat=cswdat[cswpoint++];
                if (cswpoint>=(16*1024*1024)) cswpoint=0;
                if (cswskip)
                   cswskip--;
                else if (cintone && dat>0xD) /*Not in tone any more - data start bit*/
                {
                        cswpoint++; /*Skip next half of wave*/
                        if (tapespeed) cswskip=6;
                        cintone=0;
                        cindat=1;
//                        sprintf(csws,"Entered data at %i\n",ftell(cswf));
//                        fputs(csws,cswlog);
                        datbits=enddat=0;
//                        dcdlow();
//                        return;
                }
                else if (cindat && datbits!=-1 && datbits!=-2)
                {
                        cswpoint++; /*Skip next half of wave*/
                        if (tapespeed) cswskip=6;
                        enddat>>=1;
                        if (dat<=0xD)
                        {
                                cswpoint+=2;
                                if (tapespeed) cswskip+=6;
                                enddat|=0x80;
                        }
                        datbits++;
                        if (datbits==8)
                        {
                                receive(enddat);
//                                sprintf(csws,"Received 8 bits %02X %c\n",enddat,enddat);
//                                fputs(csws,cswlog);
                                datbits=-2;
                                reallyfasttapebreak=1;
                                return;
                        }
                }
                else if (cindat && datbits==-2) /*Deal with stop bit*/
                {
                        cswpoint++;
                        if (tapespeed) cswskip=6;
                        if (dat<=0xD)
                        {
                                cswpoint+=2;
                                if (tapespeed) cswskip+=6;
                        }
                        datbits=-1;
                }
                else if (cindat && datbits==-1)
                {
                        if (dat<=0xD) /*Back in tone again*/
                        {
                                intula(HIGHTONE);
//                                dcd();
//                                sprintf(csws,"Entering tone again\n");
//                                fputs(csws,cswlog);
                                cindat=0;
                                cintone=1;
                                datbits=0;
                                reallyfasttapebreak=1;
                                return;
                        }
                        else /*Start bit*/
                        {
                                cswpoint++; /*Skip next half of wave*/
                                if (tapespeed) cswskip+=6;
                                datbits=0;
                                enddat=0;
                        }
                }
        }
}
