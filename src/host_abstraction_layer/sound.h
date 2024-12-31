/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * soundopenal.c
 * 
 * Sound abstration layer (OpenAL)
 *
 * Allows actual sound libraries and sound used for the emulation
 * to be abstracted from the actual electron code
 * 
 * This allows easier porting to different sound libraries in 
 * future in order to allow maximum cross platform support and 
 * long term durability.
 *
 * TODO: The above description is not completely true as
 *       some sound code still exist in the electron emulation
 *       main code.  This should change in future.
 */


/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/


/******************************************************************************
* Typedefs
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void sound_add(uint8_t dat);
void sound_mixbuffer(int16_t *d);
void sound_givealbufferdd(int16_t *buf);