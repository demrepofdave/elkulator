/*Elkulator v1.0 by Sarah Walker
  First Byte joystick emulation*/
#include <allegro.h>

#include "elk.h"

uint8_t readfirstbyte()
{
        int c;
        uint8_t temp=0xFF;
        if (!num_joysticks) num_joysticks++;
        if (joy[joffset%num_joysticks].stick[0].axis[1].pos<-32) temp&=~0x01;
        if (joy[joffset%num_joysticks].stick[0].axis[1].pos>=32) temp&=~0x02;
        if (joy[joffset%num_joysticks].stick[0].axis[0].pos<-32) temp&=~0x04;
        if (joy[joffset%num_joysticks].stick[0].axis[0].pos>=32) temp&=~0x08;
        for (c=0;c<joy[joffset%num_joysticks].num_buttons;c++) if (joy[joffset%num_joysticks].button[c].b) temp&=~0x10;
        return temp;
}
