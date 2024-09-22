#include <allegro.h>
#include "samples.h"

SAMPLE *seeksmp[4][2];
SAMPLE *motorsmp[3];
SAMPLE *tsamples[2];

//typedef enum 
//{
//    BIT_SCREEN,
//    BIT_MAIN,
//    BIT_B16,
//    BIT_B162,
//    BIT_VIDB,
//    BIT_VP1,
//    BIT_VP2
//} sampleSelect;

bool sample_seek_load(uint8_t indexA, uint8_t indexB, const char * filename) // Don't know what a and b are at the moment.
{
    seeksmp[indexA][indexB] = load_wav(filename);
    return (seeksmp[indexA][indexB] != NULL);
}

unsigned long sample_seek_get_length(uint8_t indexA, uint8_t indexB)
{
    int length = 0;
    if(seeksmp[indexA][indexB])
    {
        length = seeksmp[indexA][indexB]->len;
    }
    return (length);
}

void * sample_seek_get_data_ptr(uint8_t indexA, uint8_t indexB)
{
    void * data = 0;
    if(seeksmp[indexA][indexB])
    {
        data = seeksmp[indexA][indexB]->data;
    }
    return (data);
}

int sample_seek_get_frequency(uint8_t indexA, uint8_t indexB)
{
    int freq = 0;
    if(seeksmp[indexA][indexB])
    {
        freq = seeksmp[indexA][indexB]->freq;
    }
    return (freq);
}                    

void sample_seek_destroy_all()
{
    int index;
    for (index=0;index<4;index++)
    {
        if (seeksmp[index][0])
        {
            destroy_sample(seeksmp[index][0]);
            seeksmp[index][0] = NULL;
        } 
        if (seeksmp[index][1]) 
        {
            destroy_sample(seeksmp[index][1]);
            seeksmp[index][1] = NULL;
        }
    }
}

bool sample_motor_load(uint8_t index, const char * filename) // Don't know what a is at the moment.
{
    motorsmp[index] = load_wav(filename);
    return (motorsmp[index] != NULL);
}

unsigned long sample_motor_get_length(uint8_t index)
{
    int length = 0;
    if(motorsmp[index])
    {
        length = motorsmp[index]->len;
    }
    return (length);
}

void * sample_motor_get_data_ptr(uint8_t index)
{
    void * data = 0;
    if(motorsmp[index])
    {
        data = motorsmp[index]->data;
    }
    return (data);
}

int sample_motor_get_frequency(uint8_t index)
{
    int freq = 0;
    if(motorsmp[index])
    {
        freq = motorsmp[index]->freq;
    }
    return (freq);
}    

void sample_motor_destroy_all()
{
    int index;
    for (index=0;index<3;index++)
    {
        if (motorsmp[index])
        {
            destroy_sample(motorsmp[index]);
            motorsmp[index] = NULL;
        } 
    }
}


bool sample_tape_noise_motor_load(uint8_t index, const char * filename) // Don't know what a is at the moment.
{
    tsamples[index] = load_wav(filename);
    return (tsamples[index] != NULL);
}

unsigned long sample_tape_noise_get_length(uint8_t index)
{
    int length = 0;
    if(tsamples[index])
    {
        length = tsamples[index]->len;
    }
    return (length);
}

void * sample_tape_noise_get_data_ptr(uint8_t index)
{
    void * data = 0;
    if(tsamples[index])
    {
        data = tsamples[index]->data;
    }
    return (data);
}

int sample_tape_noise_get_frequency(uint8_t index)
{
    int freq = 0;
    if(tsamples[index])
    {
        freq = tsamples[index]->freq;
    }
    return (freq);
}    

void sample_tape_noise_destroy_all()
{
    if(tsamples[0])
    {
        destroy_sample(tsamples[0]);
        tsamples[0] = NULL;
    }

    if(tsamples[1])
    {
        destroy_sample(tsamples[1]);
        tsamples[1] = NULL;
    }
}