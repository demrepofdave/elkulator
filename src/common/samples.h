#include <stdint.h>
#include <stdbool.h>

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