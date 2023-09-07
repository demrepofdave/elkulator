/*Elkulator v1.0 by Sarah Walker
  ULA and video emulation*/
#include <allegro.h>
#include <stdio.h>
#include <zlib.h>
#include "elk.h"
#include "2xsai.h"
#include "uef.h"

void dosavescrshot();
void saveframe();
void stopmovie();

int bitcount=0,sbitcount=9;
int tapeout=0;
int tapeoutwait=0;

int discspd=16;
#define INT_VBL      0x04
#define INT_RTC      0x08
#define INT_RECEIVE  0x10
#define INT_SEND     0x20
#define INT_HIGHTONE 0x40

#define BIT_LENGTH 1628

uint16_t tapedat;
int tapewrite=0;
int irq=0,nmi=0;
int extrom,rombank,intrombank;
int tapeon;

int soundlimit,soundcount,soundstat;
extern int soundon;
uint8_t sndstreambuf[626];
int sndstreamindex = 0;
int sndstreamcount = 0;

struct
{
        uint8_t isr,ier;
        uint8_t addrlo,addrhi;
        uint8_t recdat;
        
        int x,y;
        int dispon;
        int oddeven;
        int mode;
        
        uint16_t addr,addrback;
        int sc;
        int framecount;
        int draw;
        
        uint8_t tapelatch;
} ula;

int coldepth;
BITMAP *b,*b16,*b162,*vidb;
BITMAP *vp1,*vp2;

void clearall()
{
        clear(b);
        clear(b16);
        clear(b162);
        clear(screen);
}

uint8_t pal[16];
int palwritenum=0,palwritenum2=0;
PALETTE elkpal =
{
      {0,0,0},
      {63,0,0},
      {0,63,0},
      {63,63,0},
      {0,0,63},
      {63,0,63},
      {0,63,63},
      {63,63,63},
};

uint8_t ulalookup[256];
void initula()
{
        int c;
//        allegro_init();
        coldepth=desktop_color_depth();
        set_color_depth(desktop_color_depth());
        #ifdef WIN32
        set_gfx_mode(GFX_AUTODETECT_WINDOWED,2048,2048,0,0);
        vidb=create_video_bitmap(800,300);
        #else
        set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,512,0,0);
        winsizex=640; winsizey=512;
        #endif
        b16=create_bitmap(800*2,600);
        b162=create_bitmap(640,256);
        clear(b16);
        Init_2xSaI(desktop_color_depth());
        initpaltables();
        set_color_depth(8);
        b=create_bitmap(640,616);
        set_palette(elkpal);
        for (c=0;c<256;c++)
        {
                ulalookup[c]=0;
                if (c&2) ulalookup[c]|=1;
                if (c&8) ulalookup[c]|=2;
                if (c&0x20) ulalookup[c]|=4;
                if (c&0x80) ulalookup[c]|=8;
        }
        set_color_depth(desktop_color_depth());

        /* Clear the sound stream buffer before we start filling it. */
        memset(sndstreambuf, 0, sizeof(sndstreambuf));
}

int fullblit=0;

void enterfullscreen()
{
        #ifdef WIN32
        destroy_bitmap(vidb);
        #endif
        set_color_depth(desktop_color_depth());
        set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,800,600,0,0);
        #ifdef WIN32
        vp1=create_video_bitmap(800,600);
        vp2=create_video_bitmap(800,600);
        show_video_bitmap(vp2);
        vidb=create_video_bitmap(800,300);
        clear(vidb);
        install_mouse();
        #endif
        set_color_depth(8);
        set_palette(elkpal);
        winsizex=800; winsizey=600;
}

void leavefullscreen()
{
        #ifdef WIN32
        remove_mouse();
        destroy_bitmap(vidb);
        destroy_bitmap(vp2);
        destroy_bitmap(vp1);
        #endif
        set_color_depth(desktop_color_depth());
        #ifdef WIN32
        set_gfx_mode(GFX_AUTODETECT_WINDOWED,2048,2048,0,0);
        vidb=create_video_bitmap(800,300);
        #else
        set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,512,0,0);
        winsizex=640; winsizey=512;
        #endif
        set_color_depth(8);
        set_palette(elkpal);
}

void resetula()
{
        ula.x=ula.y=ula.sc=0;
        ula.dispon=ula.oddeven=0;
        ula.isr=2;
        ula.ier=0;
        ula.mode=6;
        tapeon=0;
        ula.framecount++;
        soundon=soundstat=0;
        tapewrite=0;
//        wipesoundbuffer();
}

void updateulaints()
{
        extern int serial_irq;

        if (ula.isr & ula.ier & 0x7C)
        {
                ula.isr|=1;
                irq=1;
//                printf("Interrupt %02X %02X\n",ula.isr,ula.ier);
        }
        else if (plus1 && serial_irq)
                irq = 1;
        else
        {
                ula.isr&=~1;
                irq=0;
        }
}

int hightone=0;

void intula(uint8_t num)
{
        if (num==INT_HIGHTONE)
        {
                hightone=1;
//                printf("In high tone!\n");
                return;
        }
//        if (num&0x10) printf("INT &0x10\n");
        ula.isr|=num;
        updateulaints();
}

void clearintula(uint8_t num)
{
        ula.isr &= ~num;
        updateulaints();
}

void writeula(uint16_t addr, uint8_t val)
{
        switch (addr&0xF)
        {
                case 0: /*Interrupt control*/
                ula.ier=val;
                updateulaints();
                break;
                case 2:
                ula.addrlo=val&0xE0;
                break;
                case 3:
                ula.addrhi=val&0x3F;
                break;
                case 4:
//                printf("Write tape out %i %i %02X\n",tapeoutwait,tapewrite,val);
                if (tapewrite<BIT_LENGTH)
                {
                        tapewrite=BIT_LENGTH;
                        ula.isr&=~INT_SEND;
                        updateulaints();
                }
//                ula.isr&=~INT_RECEIVE;
//                updateulaints();
                sbitcount=9;
                tapeoutwait=0;
                bitcount=0;
                tapedat=(val<<2)|1;
//                tapewrite=2000000/(1200/10);
                break;
                case 5: /*ROM control / interrupt clear*/
                if (val&0x10) ula.isr&=~INT_VBL;
                if (val&0x20) ula.isr&=~INT_RTC;
                if (val&0x40) ula.isr&=~INT_HIGHTONE;
                updateulaints();
                rombank=val&0xF;
                if (rombank>=0xC) extrom=1;
                if ((rombank&0xC)==8)
                {
                        extrom=0;
                        intrombank=rombank;
                }
                break;
                case 6: /*Timer*/
                soundlimit=(val<<16)+0x20000;
                break;
                case 7: /*Misc control*/
                if ((((val&6)==2)?1:0)!=soundon)
                   soundon=((val&6)==2)?1:0;
                ula.mode=(val>>3)&7;
//                if (!(ula.mode&4)) discspd=24;
                /*else               *///discspd=16;
//                if (tapeon && !(val&64)) resetsound();
                tapeon=val&64;
                tapeout=((val&6)==4);
                break;
                case 0x8: case 0xA: case 0xC: case 0xE:
                switch ((addr>>1)&3)
                {
                        case 0:
                        pal[0]&=~4;
                        pal[2]&=~4;
                        pal[8]&=~6;
                        pal[10]&=~6;
                        if (!(val&0x10)) pal[0]|=4;
                        if (!(val&0x20)) pal[2]|=4;
                        if (!(val&0x40)) pal[8]|=4;
                        if (!(val&0x80)) pal[10]|=4;
                        if (!(val&0x4)) pal[8]|=2;
                        if (!(val&0x8)) pal[10]|=2;
                        break;
                        case 1:
                        pal[4]&=~4;
                        pal[6]&=~4;
                        pal[12]&=~6;
                        pal[14]&=~6;
                        if (!(val&0x10)) pal[4]|=4;
                        if (!(val&0x20)) pal[6]|=4;
                        if (!(val&0x40)) pal[12]|=4;
                        if (!(val&0x80)) pal[14]|=4;
                        if (!(val&0x4)) pal[12]|=2;
                        if (!(val&0x8)) pal[14]|=2;
                        break;
                        case 2:
                        pal[5]&=~4;
                        pal[7]&=~4;
                        pal[13]&=~6;
                        pal[15]&=~6;
                        if (!(val&0x10)) pal[5]|=4;
                        if (!(val&0x20)) pal[7]|=4;
                        if (!(val&0x40)) pal[13]|=4;
                        if (!(val&0x80)) pal[15]|=4;
                        if (!(val&0x4)) pal[13]|=2;
                        if (!(val&0x8)) pal[15]|=2;
                        break;
                        case 3:
                        pal[1]&=~4;
                        pal[3]&=~4;
                        pal[9]&=~6;
                        pal[11]&=~6;
                        if (!(val&0x10)) pal[1]|=4;
                        if (!(val&0x20)) pal[3]|=4;
                        if (!(val&0x40)) pal[9]|=4;
                        if (!(val&0x80)) pal[11]|=4;
                        if (!(val&0x4)) pal[9]|=2;
                        if (!(val&0x8)) pal[11]|=2;
                        break;
                }
                break;
                case 0x9: case 0xB: case 0xD: case 0xF:
                switch ((addr>>1)&3)
                {
                        case 0:
                        pal[0]&=~3;
                        pal[2]&=~3;
                        pal[8]&=~1;
                        pal[10]&=~1;
                        if (!(val&1)) pal[0]|=1;
                        if (!(val&2)) pal[2]|=1;
                        if (!(val&4)) pal[8]|=1;
                        if (!(val&8)) pal[10]|=1;
                        if (!(val&16)) pal[0]|=2;
                        if (!(val&32)) pal[2]|=2;
                        break;
                        case 1:
                        pal[4]&=~3;
                        pal[6]&=~3;
                        pal[12]&=~1;
                        pal[14]&=~1;
                        if (!(val&1)) pal[4]|=1;
                        if (!(val&2)) pal[6]|=1;
                        if (!(val&4)) pal[12]|=1;
                        if (!(val&8)) pal[14]|=1;
                        if (!(val&16)) pal[4]|=2;
                        if (!(val&32)) pal[6]|=2;
                        break;
                        case 2:
                        pal[5]&=~3;
                        pal[7]&=~3;
                        pal[13]&=~1;
                        pal[15]&=~1;
                        if (!(val&1)) pal[5]|=1;
                        if (!(val&2)) pal[7]|=1;
                        if (!(val&4)) pal[13]|=1;
                        if (!(val&8)) pal[15]|=1;
                        if (!(val&16)) pal[5]|=2;
                        if (!(val&32)) pal[7]|=2;
                        break;
                        case 3:
                        pal[1]&=~3;
                        pal[3]&=~3;
                        pal[9]&=~1;
                        pal[11]&=~1;
                        if (!(val&1)) pal[1]|=1;
                        if (!(val&2)) pal[3]|=1;
                        if (!(val&4)) pal[9]|=1;
                        if (!(val&8)) pal[11]|=1;
                        if (!(val&16)) pal[1]|=2;
                        if (!(val&32)) pal[3]|=2;
                        break;
                }
                break;
        }
}

uint8_t readula(uint16_t addr)
{
        uint8_t temp;
        switch (addr&0xF)
        {
                case 0: /*Interrupt status*/
                temp=ula.isr|0x80;
                ula.isr&=~2;
/*                if (temp&0x10)
                {
                        printf("Reading int %04X\n",pc);
                }*/
                return temp;
                case 4:
                ula.isr&=~0x10;
                updateulaints();
//                printf("Read tapelatch %02X\n",ula.tapelatch);
                return ula.tapelatch;
//                return ula.recdat;
        }
        return 0xFE;
}

uint8_t taperecdat;

void receive(uint8_t val)
{
        ula.recdat=taperecdat=val;
        hightone=0;
        bitcount=9;
//        printf("Receive dat %02X\n",ula.recdat);
/*        if (reallyfasttape && inreallyfasttape)
        {
//                rpclog("Recieve really fast data %02X\n",val);
                reallyfasttapebreak=1;
//                ram[0xC0]=0x80;
//                ram[0xBD]=val;
        }*/
//        else
//        {
//        intula(INT_RECEIVE);
//        }
}

void tapenextbyte()
{
        intula(INT_SEND);
}

int fasttapebreak;
extern int pauseit;
extern int cswena;
int bitcount;
void polltape()
{
        fasttapebreak=0;
        tapewrite+=BIT_LENGTH;
        if (pauseit)
        {
                if (!tapeout)
                {
                        if (tapeon)
                        {
                                if (cswena) pollcsw();
                                else        polluef();
                        }
                }
                return;
        }
        sbitcount--;
        if (tapeoutwait==1) tapeoutwait=2;
        if (sbitcount<=0)
        {
                sbitcount=9;
                ula.isr|=INT_SEND;
                updateulaints();
                if (!tapeoutwait) tapeoutwait=1;
        }

        if (bitcount>0)
        {
                bitcount--;
                if (bitcount==7)
                {
                        ula.isr&=~INT_RECEIVE;
                        updateulaints();
                }
        }
        else
        {
                if ((tapedat&3)==1)
                {
                        ula.isr&=~INT_HIGHTONE;
                        ula.isr|=INT_RECEIVE;
                        updateulaints();
                        ula.tapelatch=ula.recdat;
                        bitcount=9;
                        fasttapebreak=1;
                        adddatnoise(ula.tapelatch);
//                        printf("Receive int %02X\n",ula.tapelatch);
                }
                if ((tapedat&0x3FF)==0x3FF)
                {
                        addhightone();
                        ula.isr|=INT_HIGHTONE;
                        updateulaints();
                        bitcount=0;
                        fasttapebreak=1;
                }
                if (!tapeout)
                {
                        if (tapeon)
                        {
                                if (cswena) pollcsw();
                                else        polluef();
                        }
                }
        }

        if (hightone || tapeout) tapedat=(tapedat>>1)|(1<<9);
        else if (!tapeon) tapedat>>=1;
        else
        {
//                printf("%i - was %03X ",bitcount,tapedat&0x3FF);
                if (bitcount==9)      tapedat=(tapedat>>1)|(1<<9);
                else if (bitcount==8) tapedat>>=1;
                else
                {
                        tapedat=(tapedat>>1)|((taperecdat&1)<<9);
                        taperecdat>>=1;
                }
//                printf("now %03X\n",tapedat&0x3FF);
        }
}

void reallyfasttapepoll()
{
        int c;
        for (c=0;c<1000;c++)
        {
                polltape();
                if (fasttapebreak) break;
        }
}
int modescs[8]={8,8,8,10,8,8,10,10};
uint16_t modelens[8]={0x5000,0x5000,0x5000,0x4000,0x2800,0x2800,0x2000,0x2000};
int modeend[8]={256,256,256,250,256,256,250,250};

int nextulapoll;
int numlines=0;
int wantsavescrshot=0;
int wantmovieframe=0;
FILE *moviefile;
BITMAP *moviebitmap;

void yield()
{
        uint8_t temp;
        int x,c;
        uint16_t tempaddr;
        int col;
        int oldcycles;
//        if (nextulapoll) printf("Beginning poll %i ",ula.x);
        while (ulacycles<cycles)
        {
                oldcycles=ulacycles;
                while (ula.x<640 && ula.dispon && ula.draw && (ulacycles<cycles))
                {
                        if (ula.sc&8)
                        {
                                if (LINEDOUBLE) ula.y<<=1;
                                for (x=0;x<8;x++)
                                    b->line[ula.y][ula.x+x]=0;
                                if (LINEDOUBLE) ula.y>>=1;
                        }
                        else if (!(ula.x&8) || !(ula.mode&4))
                        {
                                if (ula.mode&4) tempaddr=ula.addr+ula.sc+((ula.x>>1)&~7);
                                else            tempaddr=ula.addr+ula.sc+(ula.x&~7);
                                if (tempaddr&0x8000) tempaddr-=modelens[ula.mode];
                                temp=ram[tempaddr];
                                if (LINEDOUBLE) ula.y<<=1;
                                if (HALFSIZE)
                                {
                                        switch (ula.mode)
                                        {
                                                case 0: case 3:
                                                for (x=0;x<8;x++)
                                                {
                                                        col=ulalookup[temp&0x80];
                                                        b->line[ula.y][(ula.x+x)>>1]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 1:
                                                for (x=0;x<8;x+=2)
                                                {
                                                        col=ulalookup[temp&0x88];
                                                        b->line[ula.y][(ula.x+x)>>1]=pal[col];
//                                                        b->line[ula.y][ula.x+x+1]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 2:
                                                for (x=0;x<8;x+=4)
                                                {
                                                        col=ulalookup[temp];
                                                        b->line[ula.y][(ula.x+x)>>1]=pal[col];
//                                                        b->line[ula.y][ula.x+x+1]=pal[col];
                                                        b->line[ula.y][(ula.x+x+2)>>1]=pal[col];
//                                                        b->line[ula.y][ula.x+x+3]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 4: case 6: case 7:
                                                for (x=0;x<16;x+=2)
                                                {
                                                        col=ulalookup[temp&0x80];
                                                        b->line[ula.y][(ula.x+x)>>1]=pal[col];
//                                                        b->line[ula.y][ula.x+x+1]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 5:
                                                for (x=0;x<16;x+=4)
                                                {
                                                        col=ulalookup[temp&0x88];
                                                        b->line[ula.y][(ula.x+x)>>1]=pal[col];
//                                                        b->line[ula.y][ula.x+x+1]=pal[col];
                                                        b->line[ula.y][(ula.x+x+2)>>1]=pal[col];
//                                                        b->line[ula.y][ula.x+x+3]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                        }
                                }
                                else
                                {
                                        switch (ula.mode)
                                        {
                                                case 0: case 3:
                                                for (x=0;x<8;x++)
                                                {
                                                        col=ulalookup[temp&0x80];
                                                        b->line[ula.y][ula.x+x]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 1:
                                                for (x=0;x<8;x+=2)
                                                {
                                                        col=ulalookup[temp&0x88];
                                                        b->line[ula.y][ula.x+x]=pal[col];
                                                        b->line[ula.y][ula.x+x+1]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 2:
                                                for (x=0;x<8;x+=4)
                                                {
                                                        col=ulalookup[temp];
                                                        b->line[ula.y][ula.x+x]=pal[col];
                                                        b->line[ula.y][ula.x+x+1]=pal[col];
                                                        b->line[ula.y][ula.x+x+2]=pal[col];
                                                        b->line[ula.y][ula.x+x+3]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 4: case 6: case 7:
                                                for (x=0;x<16;x+=2)
                                                {
                                                        col=ulalookup[temp&0x80];
                                                        b->line[ula.y][ula.x+x]=b->line[ula.y][ula.x+x+1]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                                case 5:
                                                for (x=0;x<16;x+=4)
                                                {
                                                        col=ulalookup[temp&0x88];
                                                        b->line[ula.y][ula.x+x]=pal[col];
                                                        b->line[ula.y][ula.x+x+1]=pal[col];
                                                        b->line[ula.y][ula.x+x+2]=pal[col];
                                                        b->line[ula.y][ula.x+x+3]=pal[col];
                                                        temp<<=1;
                                                }
                                                break;
                                        }
                                }
                                if (LINEDOUBLE) ula.y>>=1;
                        }
                        ula.x+=8;
                        ulacycles++;
                }
                while (ula.x<640 && ula.y<256 && !ula.dispon && ula.draw && (ulacycles<cycles))
                {
//                        if (!ula.x) rpclog("Blank line %i\n",ula.y);
                        if (LINEDOUBLE) ula.y<<=1;
                        for (x=0;x<8;x++)
                            b->line[ula.y][ula.x+x]=0;
                        if (LINEDOUBLE) ula.y>>=1;
                        ula.x+=8;
                        ulacycles++;
                }
/*                if (ula.x==640 && ula.y==99)
                {
                        ula.isr|=INT_RTC;
                        updateulaints();
                }*/
                if (ulacycles==oldcycles)
                {
                        ula.x+=8;
                        numlines+=8;
                        if (ula.x==1024)
                        {
                                /*Sound handling*/
                                if (!tapeon || !tapespeed)
                                {
                                        for (c=0;c<2;c++)
                                        {
                                                soundcount+=0x20000;
                                                if (soundcount>=soundlimit)
                                                {
                                                        soundcount-=soundlimit;
                                                        soundstat^=0x7F;
                                                }
                                                if (soundlimit<0x20000) addsnd((soundon)?0x7F:0);
                                                else                    addsnd((soundon)?soundstat:0);

                                                /* Add values to the stream buffer if we are making a movie. */
                                                if (wantmovieframe)
                                                {
                                                    sndstreambuf[sndstreamindex] = soundon ? ((soundlimit<0x20000) ? 0x7F : soundstat) : 0;
                                                    sndstreamindex = (sndstreamindex + 1) % sizeof(sndstreambuf);
                                                    sndstreamcount++;
                                                }
                                        }
                                        logvols();

                                }
                                else if (wantmovieframe)
                                {
                                    sndstreambuf[sndstreamindex] = 0;
                                    sndstreamindex = (sndstreamindex + 1) % sizeof(sndstreambuf);
                                    sndstreamcount++;
                                }
                                
                                ula.x=0;
                                ula.y++;
                                if (ula.dispon)
                                {
                                        ula.sc++;
                                        if (ula.sc==modescs[ula.mode])
                                        {
                                                ula.sc=0;
                                                ula.addr+=((ula.mode&4)?320:640);
                                                /*This does seem odd, that the ULA would fix up
                                                  the address at this point. But Firetrack has
                                                  graphic problems if this is not done*/
                                                if (ula.addr&0x8000) ula.addr-=modelens[ula.mode];
                                                ula.addrback=ula.addr;
                                        }
                                        ula.addr=ula.addrback;
                                }
                                if (ula.y==99)
                                {
                                        ula.isr|=INT_RTC;
                                        updateulaints();
                                }
                                c=modeend[ula.mode];
//                                if (ula.oddeven) c++;
                                if (ula.y==c)
                                {
                                        ula.dispon=0;
                                        ula.isr|=INT_VBL;
                                        updateulaints();
//                                        rpclog("Vsync\n");
                                }
                                if (ula.y==258) ula.dispon=0;
                                if (ula.y==((ula.oddeven)?313:312) || ula.y>=313)
                                {
//                                        printf("Numlines %i\n",numlines/1024);
                                        numlines=0;
                                        ula.y=0;
                                        ula.oddeven^=1;
                                        ula.dispon=1;
                                        ula.sc=0;
                                        ula.addr=(ula.addrlo<<1)|(ula.addrhi<<9);
//                                        rpclog("ULA addr %04X\n",ula.addr);
                                        if (!ula.addr) ula.addr=0x8000-modelens[ula.mode];
                                        
                                        if (ula.draw)
                                        {
                                                startblit();
                                                switch (drawmode)
                                                {
                                                        case SCANLINES:
                                                        blit(b,screen,0,0,(winsizex-640)/2,(winsizey-512)/2,640,512);
                                                        break;
                                                        case LINEDBL:
                                                        #ifdef WIN32
                                                        blit(b,vidb,0,0,0,0,640,256);
                                                        if (videoresize) stretch_blit(vidb,screen,0,0,640,256,0,0,winsizex,winsizey);
                                                        else             stretch_blit(vidb,screen,0,0,640,256,(winsizex-640)/2,(winsizey-512)/2,640,512);
                                                        #else
                                                        for (c=0;c<512;c++) blit(b,b16,0,c>>1,0,c,640,1);
                                                        blit(b16,screen,0,0,(winsizex-640)/2,(winsizey-512)/2,640,512);
                                                        #endif
                                                        break;
                                                        case _2XSAI:
                                                        blit(b,b162,0,0,0,0,640,256);
                                                        Super2xSaI(b162,b16,0,0,0,0,320,256);
                                                        blit(b16,screen,0,0,(winsizex-640)/2,(winsizey-512)/2,640,512);
                                                        break;
                                                        case SCALE2X:
                                                        blit(b,b162,0,0,0,0,640,256);
                                                        scale2x(b162,b16,320,256);
                                                        blit(b16,screen,0,0,(winsizex-640)/2,(winsizey-512)/2,640,512);
                                                        break;
                                                        case EAGLE:
                                                        blit(b,b162,0,0,0,0,640,256);
                                                        SuperEagle(b162,b16,0,0,0,0,320,256);
                                                        blit(b16,screen,0,0,(winsizex-640)/2,(winsizey-512)/2,640,512);
                                                        break;
                                                        case PAL:
                                                        palfilter(b,b16,coldepth);
                                                        blit(b16,screen,0,0,(winsizex-640)/2,(winsizey-512)/2,640,512);
                                                        break;
                                                }
                                                if (wantsavescrshot) dosavescrshot();
                                                if (wantmovieframe) saveframe();
                                                endblit();
                                        }
//                                        wait50();
                                        ula.addrback=ula.addr;
                                        ula.framecount++;
                                        if (ula.framecount==25) ula.framecount=0;
                                        ula.draw=((!tapeon || !tapespeed) || !ula.framecount);
                                }
                        }
                        ulacycles++;
                }
        }
/*        if (nextulapoll)
        {
                nextulapoll=0;
                printf("%i\n",ula.x);
        }*/
}

void waitforramsync()
{
        /* During ULA screen update region. */
        if (ula.dispon && ula.x<640)
        {
                /* Lower screen modes where the ULA is continuously active. */
                if (!(ula.mode&4))
                {
                        cycles+=((640-ula.x)/8);
                }

                if (cycles&1) cycles++;
                cycles++;
	}

        /* Outside the region, access RAM at 1MHz unless the ULA is enhanced. */
	else if (ulamode == 0)
                cycles++;
}

void saveulastate(FILE *f)
{
        int c;
        putc(ula.ier,f);
        putc(ula.isr,f);
        putc(ula.addrlo,f);
        putc(ula.addrhi,f);
        putc(rombank,f);
        putc(extrom,f);
        putc((soundlimit-0x20000)>>16,f);
        putc(soundon,f);
        putc(ula.mode,f);
        putc(tapeon,f);
        putc(tapeout,f);
        for (c=0;c<16;c++) putc(pal[c],f);
        
        putc(ula.draw,f);
        putc(ula.x,f);
        putc(ula.x>>8,f);
        putc(ula.y,f);
        putc(ula.y>>8,f);
        putc(ula.sc,f);
        putc(ula.dispon,f);
        putc(ula.oddeven,f);
        putc(ula.addrback,f);
        putc(ula.addrback>>8,f);
        putc(ulacycles,f);
        putc(ulacycles>>8,f);
        putc(ulacycles>>16,f);
        putc(ulacycles>>24,f);
}

void loadulastate(FILE *f)
{
        int c;
        ula.ier=getc(f);
        ula.isr=getc(f);
        ula.addrlo=getc(f);
        ula.addrhi=getc(f);
        rombank=getc(f);
        extrom=getc(f);
        soundlimit=(getc(f)<<16)+0x20000;
        soundon=getc(f);
        ula.mode=getc(f);
        tapeon=getc(f);
        tapeout=getc(f);
        for (c=0;c<16;c++) pal[c]=getc(f);

        ula.draw=getc(f);
        ula.x=getc(f);
        ula.x|=getc(f)<<8;
        ula.y=getc(f);
        ula.y|=getc(f)<<8;
        ula.sc=getc(f);
        ula.dispon=getc(f);
        ula.oddeven=getc(f);
        ula.addrback=getc(f);
        ula.addrback|=getc(f)<<8;
        ulacycles=getc(f);
        ulacycles|=getc(f)<<8;
        ulacycles|=getc(f)<<16;
        ulacycles|=getc(f)<<24;
}

void savescrshot()
{
        wantsavescrshot=1;
}

void dosavescrshot()
{
        BITMAP *tb;
        set_color_depth(desktop_color_depth());
        tb=create_bitmap(640,512);
        switch (drawmode)
        {
                case SCANLINES:
                blit(b,tb,0,0,0,0,640,512);
                break;
                case LINEDBL:
                #ifdef WIN32
                stretch_blit(vidb,tb,0,0,640,256,0,0,640,512);
                #else
                blit(b16,tb,0,0,0,0,640,512);
                #endif
                break;
                case _2XSAI:
                blit(b,b162,0,0,0,0,640,256);
                Super2xSaI(b162,b16,0,0,0,0,320,256);
                blit(b16,tb,0,0,0,0,640,512);
                break;
                case SCALE2X:
                blit(b,b162,0,0,0,0,640,256);
                scale2x(b162,b16,320,256);
                blit(b16,tb,0,0,0,0,640,512);
                break;
                case EAGLE:
                blit(b,b162,0,0,0,0,640,256);
                SuperEagle(b162,b16,0,0,0,0,320,256);
                blit(b16,tb,0,0,0,0,640,512);
                break;
                case PAL:
                palfilter(b,b16,coldepth);
                blit(b16,tb,0,0,0,0,640,512);
                break;
        }
        save_bmp(scrshotname,tb,NULL);
        destroy_bitmap(tb);
        set_color_depth(8);
        
        wantsavescrshot=0;
}

void startmovie()
{
    stopmovie();

    wantmovieframe = 1;
    moviefile = fopen(moviename, "wb");
    if (moviefile == NULL)
        return;

    moviebitmap=create_bitmap_ex(8, 640, 256);
    sndstreamindex = 0;
    sndstreamcount = 0;
}

void stopmovie()
{
    wantmovieframe = 0;
    if (moviefile != NULL) {
        fclose(moviefile);
        destroy_bitmap(moviebitmap);
        moviefile = NULL;
    }
}

#define DEFLATE_CHUNK_SIZE 262144

int deflate_bitmap(int level)
{
    unsigned int have;
    z_stream strm;
    unsigned char in[DEFLATE_CHUNK_SIZE];
    unsigned char out[DEFLATE_CHUNK_SIZE];

    /* Allocate the deflate state. */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if (deflateInit(&strm, level) != Z_OK)
        return Z_ERRNO;

    /* Compress the bitmap buffer. */
    strm.avail_in = 640*256;
    strm.next_in = moviebitmap->dat;

    /* Run deflate() on the bitmap buffer, finishing the compression. */
    strm.avail_out = DEFLATE_CHUNK_SIZE;
    strm.next_out = out;
    if (deflate(&strm, Z_FINISH) == Z_STREAM_ERROR)
        return Z_ERRNO;

    /* Write the length of the data. */
    have = DEFLATE_CHUNK_SIZE - strm.avail_out;
    fwrite(&have, sizeof(unsigned int), 1, moviefile);

    if (fwrite(out, 1, have, moviefile) != have || ferror(moviefile)) {
        deflateEnd(&strm);
        return Z_ERRNO;
    }

    /* clean up and return */
    deflateEnd(&strm);
    return Z_OK;
}

void saveframe()
{
    if (moviefile == NULL)
        return;

    int start;
    if (sndstreamcount == 624) {
        /* Take the last 625 samples. */
        start = (sndstreamindex + 1) % sizeof(sndstreambuf);
    } else if (sndstreamcount == 626) {
        /* Take the first 625 samples from the 626 obtained and leave the last
           one for the next frame. */
        start = sndstreamindex;
    }

    blit(b,moviebitmap,0,0,0,0,640,256);

    if (deflate_bitmap(6) != Z_OK) {
        stopmovie();
        return;
    }

    int remaining = sizeof(sndstreambuf) - start;
    if (remaining >= 625)
        fwrite(&sndstreambuf[start], 1, 625, moviefile);
    else {
        fwrite(&sndstreambuf[start], 1, remaining, moviefile);
        fwrite(sndstreambuf, 1, 625 - remaining, moviefile);
    }

    sndstreamcount = 0;
}

void clearscreen()
{
        clear(screen);
        clear(b);
        clear(b16);
        clear(b162);
}
