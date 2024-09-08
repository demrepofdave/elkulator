/*Elkulator v1.0 by Sarah Walker
  ADC / Plus 1 emulation*/

#ifdef HAL_ALLEGRO_5
    #include <allegro5/allegro.h>
#else
    #include <allegro.h>
#endif

#include "hal/hal.h"

uint8_t plus1stat=0x7F;
int adctime=0;
uint8_t adcdat;

hal_result hal_write_adc(uint8_t val)
{
        hal_result result = HAL_OK;
        return(result);
}

uint8_t hal_read_adc()
{
        return adcdat;
}

uint8_t hal_get_plus1_stat()
{
        int c;
//        if (!num_joysticks) num_joysticks++;
        plus1stat|=0x30;
//        int joffset = hal_get_joffset();
//        for (c=0;c<joy[joffset%num_joysticks].num_buttons;c++) if (joy[joffset%num_joysticks].button[c].b) plus1stat&=~0x10;
//        for (c=0;c<joy[(joffset+1)%num_joysticks].num_buttons;c++) if (joy[(joffset+1)%num_joysticks].button[c].b) plus1stat&=~0x20;
        return plus1stat;
}
