/*Elkulator v1.0 by Sarah Walker
  ADC / Plus 1 emulation*/
#include <allegro5/allegro.h>
#include "elk.h"
#include "config_vars.h"
#include "joydev.h"

uint8_t plus1stat=0x7F;

int adctime=0;
uint8_t adcdat;

void writeadc(uint8_t val)
{
        int dat1=0,dat2=0;
        joydev_poll();
        int n=joydev_num_joysticks();
        plus1stat|=0x40;
        adctime=80;
        if (n>0)
        {
                int j0=elkConfig.expansion.joffset%n;
                int j1=(elkConfig.expansion.joffset+1)%n;
                switch (val&3)
                {
                        case 0: dat1=-joydev_get_axis(j0,0); break;
                        case 1: dat1=-joydev_get_axis(j0,1); break;
                        case 2: dat1=-joydev_get_axis(j1,0); break;
                        case 3: dat1=-joydev_get_axis(j1,1); break;
                }
                switch (val&0xC)
                {
                        case 0: case 8:
                        switch (val&3)
                        {
                                case 0: dat2=-joydev_get_axis(j0,1); break;
                                case 1: dat2=-joydev_get_axis(j0,0); break;
                                case 2: dat2=-joydev_get_axis(j1,1); break;
                                case 3: dat2=-joydev_get_axis(j1,0); break;
                        }
                        break;
                        case 4: dat2=0; break;
                        case 12: dat2=-joydev_get_axis(j1,1); break;
                }
                if (dat1>127) dat1=127;
                if (dat1<-128) dat1=-128;
                if (dat2>127) dat2=127;
                if (dat2<-128) dat2=-128;
        }
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
        joydev_poll();
        int n=joydev_num_joysticks();
        plus1stat|=0x30;
        if (n>0)
        {
                int j0=elkConfig.expansion.joffset%n;
                int j1=(elkConfig.expansion.joffset+1)%n;
                for (c=0;c<joydev_num_buttons(j0);c++) if (joydev_get_button(j0,c)) plus1stat&=~0x10;
                for (c=0;c<joydev_num_buttons(j1);c++) if (joydev_get_button(j1,c)) plus1stat&=~0x20;
        }
        return plus1stat;
}
