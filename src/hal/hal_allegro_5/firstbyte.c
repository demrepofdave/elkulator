/*Elkulator v1.0 by Sarah Walker
  First Byte joystick emulation*/

#ifdef HAL_ALLEGRO_5
    #include <allegro5/allegro.h>
#else
    #include <allegro.h>
#endif

#include "hal/hal.h"

int j_offset = 0;

hal_result hal_set_joffset(int offset)
{
    hal_result result = HAL_OK;
    offset = j_offset;
    return result;
}

// TODO: Move this into main hal.c
int hal_get_joffset()
{
    return j_offset;
}

uint8_t hal_read_firstbyte()
{
//        int c;
        uint8_t temp=0xFF;
//        int joffset = hal_get_joffset();
//        if (!num_joysticks) num_joysticks++;
//        if (joy[joffset%num_joysticks].stick[0].axis[1].pos<-32) temp&=~0x01;
//        if (joy[joffset%num_joysticks].stick[0].axis[1].pos>=32) temp&=~0x02;
//        if (joy[joffset%num_joysticks].stick[0].axis[0].pos<-32) temp&=~0x04;
//        if (joy[joffset%num_joysticks].stick[0].axis[0].pos>=32) temp&=~0x08;
//        for (c=0;c<joy[joffset%num_joysticks].num_buttons;c++) if (joy[joffset%num_joysticks].button[c].b) temp&=~0x10;
        return temp;
}
