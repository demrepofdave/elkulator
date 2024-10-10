/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * soundopenal.c
 * 
 * Sound abstration layer (OpenAL)
/*
 * Allows actual sound libraries and sound used for the emulation
 * to be abstracted from the actual electron code
 * 
 * This allows easier porting to different sound libraries in 
 * future in order to allow maximum cross platform support and 
 * long term durability.
 *
 * This is the openAL implementation of the abstraction layer.
 * 
 * TODO: The above description is not completely true as
 *       some sound code still exist in the electron emulation
 *       main code.  This should change in future.
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdio.h>
#include <allegro5/allegro.h>
#include <AL/al.h>
#include <AL/alut.h>
#include <stdint.h>
#include <string.h>
#include "elk.h"
#include "config_vars.h"
#include "logger.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

#define FREQ 31250
//#define BUFLEN (3125<<2)
#define BUFLEN (2000<<2)


/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

int samples=0;
FILE *allog;
//#undef printf

ALuint buffers[4];    // front and back buffers
ALuint source[2];     // audio source
ALuint buffersdd[4];  // front and back buffers
ALenum format;        // internal format

int16_t tempbuf[BUFLEN>>1];
int16_t tempbufdd[4410*2];

int16_t zbuf[16384];

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

void check()
{
    ALenum error;
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("AL Error : %08X\n", error);
        //printf("Description : %s\n",alGetErrorString(error));
    }
/*  if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
    {
        printf("ALut Error : %08X\n", error);
        printf("Description : %s\n",alutGetErrorString(error));
    }*/
}

void closeal()
{
    log_debug("End!\n");
    alutExit();
}

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void sound_init_part1(int argc, char *argv[])
{
    log_debug("Start...\n");
    alutInit(0,0);
    check();
    atexit(closeal);
    log_debug("AlutInit\n");
}


void sound_init_part2()
{
    int c;
    format = AL_FORMAT_STEREO16;
    check();

    alGenBuffers(4, buffers);
    check();

    alGenSources(2, source);
    check();

    alSource3f(source[0], AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(source[0], AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(source[0], AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (source[0], AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (source[0], AL_SOURCE_RELATIVE, AL_TRUE      );
    check();

    memset(tempbuf,0,BUFLEN);

    for (c=0;c<4;c++)
    {
        alBufferData(buffers[c], AL_FORMAT_STEREO16, tempbuf, BUFLEN, 31250);
    }

    alSourceQueueBuffers(source[0], 4, buffers);
    check();
    alSourcePlay(source[0]);
    check();
    printf("InitAL\n");

    alGenBuffers(4, buffersdd);
    check();

    alSource3f(source[1], AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(source[1], AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(source[1], AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (source[1], AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (source[1], AL_SOURCE_RELATIVE, AL_TRUE      );
    check();

    memset(tempbufdd,0,4410*4);

    for (c=0;c<4;c++)
    {
        alBufferData(buffersdd[c], AL_FORMAT_STEREO16, tempbufdd, 4410*4, 44100);
    }

    alSourceQueueBuffers(source[1], 4, buffersdd);
    check();
    alSourcePlay(source[1]);
    check();
    log_debug("InitAL\n");
}

void sound_givealbuffer(int16_t *buf)
{
    int processed;
    int state;
    int c;

    samples+=2000;
    
    alGetSourcei(source[0], AL_SOURCE_STATE, &state);

    if (state==0x1014)
    {
            alSourcePlay(source[0]);
            printf("Resetting sound\n");
    }
    alGetSourcei(source[0], AL_BUFFERS_PROCESSED, &processed);
    check();

    if (processed>=1)
    {
        ALuint buffer;
        alSourceUnqueueBuffers(source[0], 1, &buffer);
        check();

        for (c=0;c<(BUFLEN>>1);c++)
        { 
            zbuf[c]=buf[c>>1]^0x8000;
        }
            
        alBufferData(buffer, AL_FORMAT_STEREO16, zbuf, BUFLEN, 31250);
        check();

        alSourceQueueBuffers(source[0], 1, &buffer);
        check();
    }
}

uint16_t sndbufi[BUFLEN>>2];
int sndbufpos=0;

void addsnd(uint8_t dat)
{
    if (sndbufpos<2000)
    {
        sndbufi[sndbufpos++]=(elkConfig.sound.sndint)?(dat*31):0;
    }
/*   if (sndbufpos==2000)
     {
         sound_givealbuffer(sndbuf);
         sndbufpos=0;
     }*/
}

FILE *f;

void mixbuffer(int16_t *d)
{
        int c;
        sndbufpos=0;
        for (c=0;c<(BUFLEN>>2);c++)
                d[c]+=(sndbufi[c]-(64*31));
//        if (!f) f=fopen("sound.pcm","wb");
//        for (c=0;c<BUFLEN;c++) d[c]^=0x8000;
//        fwrite(d,BUFLEN>>1,1,f);
//        for (c=0;c<BUFLEN;c++) d[c]^=0x8000;
        sound_givealbuffer(d);
}

void sound_givealbufferdd(int16_t *buf)
{
    int processed;
    int state;
    int c;

    if (elkConfig.sound.sndddnoise || elkConfig.sound.sndtape)
    {

        alGetSourcei(source[1], AL_SOURCE_STATE, &state);

        if (state==0x1014)
        {
                alSourcePlay(source[1]);
                log_debug("Resetting sounddd\n");
        }
        alGetSourcei(source[1], AL_BUFFERS_PROCESSED, &processed);
        check();
        if (processed>=1)
        {
                ALuint buffer;
                alSourceUnqueueBuffers(source[1], 1, &buffer);
                check();

                for (c=0;c<(4410*2);c++) zbuf[c]=buf[c>>1];//^0x8000;

                alBufferData(buffer, AL_FORMAT_STEREO16, zbuf, 4410*4, 44100);
                check();

                alSourceQueueBuffers(source[1], 1, &buffer);
                check();
        }
    }
}
