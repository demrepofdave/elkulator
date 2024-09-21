/*Elkulator v1.0 by Sarah Walker
  ADC / Plus 1 emulation*/
#include <allegro.h>
#include "elk.h"

uint8_t plus1stat=0x7F;

int adctime=0;
uint8_t adcdat;

void writeadc(uint8_t val)
{
        int dat1=0,dat2=0;
        if (!num_joysticks) num_joysticks++;
        plus1stat|=0x40;
        adctime=80;
        switch (val&3)
        {
                case 0: dat1=-joy[joffset%num_joysticks].stick[0].axis[0].pos; break;
                case 1: dat1=-joy[joffset%num_joysticks].stick[0].axis[1].pos; break;
                case 2: dat1=-joy[(joffset+1)%num_joysticks].stick[0].axis[0].pos; break;
                case 3: dat1=-joy[(joffset+1)%num_joysticks].stick[0].axis[1].pos; break;
        }
        switch (val&0xC)
        {
                case 0: case 8:
                switch (val&3)
                {
                        case 0: dat2=-joy[joffset%num_joysticks].stick[0].axis[1].pos; break;
                        case 1: dat2=-joy[joffset%num_joysticks].stick[0].axis[0].pos; break;
                        case 2: dat2=-joy[(joffset+1)%num_joysticks].stick[0].axis[1].pos; break;
                        case 3: dat2=-joy[(joffset+1)%num_joysticks].stick[0].axis[0].pos; break;
                }
                break;
                case 4: dat2=0; break;
                case 12: dat2=-joy[(joffset+1)%num_joysticks].stick[0].axis[1].pos; break;
        }
        if (dat1>127) dat1=127;
        if (dat1<-128) dat1=-128;
        if (dat2>127) dat2=127;
        if (dat2<-128) dat2=-128;
        adcdat=(dat1-dat2)+128;
//        rpclog("ADC conv %02X %i %i %02X\n",val,dat1,dat2,adcdat);
}

uint8_t readadc()
{
        return adcdat;
}

uint8_t getplus1stat()
{
        int c;
        if (!num_joysticks) num_joysticks++;
        plus1stat|=0x30;
        for (c=0;c<joy[joffset%num_joysticks].num_buttons;c++) if (joy[joffset%num_joysticks].button[c].b) plus1stat&=~0x10;
        for (c=0;c<joy[(joffset+1)%num_joysticks].num_buttons;c++) if (joy[(joffset+1)%num_joysticks].button[c].b) plus1stat&=~0x20;
        return plus1stat;
}
