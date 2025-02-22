/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * samples.h
 * 
 * Sound sample abstration layer include file.
 * 
 * Allows actual sound samples libraries used for the emulation to be 
 * abstracted from the actual electron code.
 * 
 * This allows easier porting to different graphics and sound libraries in 
 * future in order to allow maximum cross platform support and long term
 * durability.
 *
 */

#ifndef _SAMPLES_H
#define _SAMPLES_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

bool sample_seek_load(uint8_t indexA, uint8_t indexB, const char * filename); // Don't know what a and b are at the moment.
unsigned long sample_seek_get_length(uint8_t indexA, uint8_t indexB);
void * sample_seek_get_data_ptr(uint8_t indexA, uint8_t indexB);
int  sample_seek_get_frequency(uint8_t indexA, uint8_t indexB);
void sample_seek_destroy_all();

bool sample_motor_load(uint8_t index, const char * filename); // Don't know what index is at the moment.
unsigned long sample_motor_get_length(uint8_t index);
void * sample_motor_get_data_ptr(uint8_t index);
int  sample_motor_get_frequency(uint8_t index);
void sample_motor_destroy_all();

bool sample_tape_noise_motor_load(uint8_t index, const char * filename); // Don't know what index is at the moment.
unsigned long sample_tape_noise_get_length(uint8_t index);
void * sample_tape_noise_get_data_ptr(uint8_t index);
int  sample_tape_noise_get_frequency(uint8_t index);
void sample_tape_noise_destroy_all();

#endif // _SAMPLES_H