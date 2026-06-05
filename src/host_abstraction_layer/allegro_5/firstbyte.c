/*Elkulator v1.0 by Sarah Walker
  First Byte joystick emulation*/
#include <allegro5/allegro.h>

#include "config_vars.h"
#include "joydev.h"

uint8_t readfirstbyte()
{
        int c;
        uint8_t temp=0xFF;
        joydev_poll();
        int n=joydev_num_joysticks();
        if (n>0)
        {
                int j=elkConfig.expansion.joffset%n;
                if (joydev_get_axis(j,1)<-32) temp&=~0x01;
                if (joydev_get_axis(j,1)>=32) temp&=~0x02;
                if (joydev_get_axis(j,0)<-32) temp&=~0x04;
                if (joydev_get_axis(j,0)>=32) temp&=~0x08;
                for (c=0;c<joydev_num_buttons(j);c++) if (joydev_get_button(j,c)) temp&=~0x10;
        }
        return temp;
}
