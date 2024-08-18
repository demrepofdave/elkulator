/*Elkulator v1.0 by Sarah Walker
  Tape noise (not very good)*/

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "elk.h"
#include "hal/hal.h"

static const char * ddnoise = "ddnoise"; // Name of directory containing wav files to emulate tape noise

int tpnoisep=0;
int tmcount=0;
int16_t tapenoise[4410];

float swavepos=0;

int sinewave[32];

#define PI 3.142

hal_sample_handle handle_tsamples[2];

void maketapenoise()
{
        char path[MAX_PATH_FILENAME_BUFFER_SIZE + sizeof(ddnoise)];
        char p2[MAX_PATH_FILENAME_BUFFER_SIZE];
        int c;

        getcwd(p2,MAX_PATH_FILENAME_BUFFER_SIZE - 1);
        if (ddtype) sprintf(path,"%s%s",exedir, ddnoise);
        else        sprintf(path,"%s%s",exedir, ddnoise);
        printf("path now %s\n",path);
        chdir(path);
        handle_tsamples[0]=hal_allocate_sample_and_load_wav("motoron.wav");
        handle_tsamples[1]=hal_allocate_sample_and_load_wav("motoroff.wav");
        chdir(p2);
        for (c=0;c<32;c++)
        {
                sinewave[c]=(int)(sin((float)c*((2.0*PI)/32.0))*128.0);
        }
}

// NOTE: Following function seems never to be called.
void closetapenoise()
{
        hal_destroy_sample(handle_tsamples[0]);
        hal_destroy_sample(handle_tsamples[1]);
        hal_release_sample(handle_tsamples[0]);
        hal_release_sample(handle_tsamples[1]);
}

void addhightone()
{
        int c;
        float wavediv=(32.0f*2400.0f)/44100.0f;
//        rpclog("Wavediv %f %i\n",wavediv,tmcount);
        tmcount++;
        for (c=0;c<368;c++)
        {
                if (tpnoisep>=4410) return;
                tapenoise[tpnoisep++]=sinewave[((int)swavepos)&0x1F]*64;
                swavepos+=wavediv;
        }
}

void adddatnoise(unsigned char dat)
{
        int c,d,e=0;
        float wavediv=(32.0f*2400.0f)/44100.0f;
//        swavepos=0;
        for (c=0;c<30;c++) /*Start bit*/
        {
                if (tpnoisep>=4410) return;
                tapenoise[tpnoisep++]=sinewave[((int)swavepos)&0x1F]*64;
                e++;
                swavepos+=(wavediv/2);
        }
        swavepos=fmod(swavepos,32.0);
        while (swavepos<32.0)
        {
                if (tpnoisep>=4410) return;
                tapenoise[tpnoisep++]=sinewave[((int)swavepos)&0x1F]*64;
                swavepos+=(wavediv/2);
                e++;
        }
        for (d=0;d<8;d++)
        {
//                swavepos=0;
                swavepos=fmod(swavepos,32.0);
//                for (c=0;c<36;c++)
                while (swavepos<32.0)
                {
                        if (tpnoisep>=4410) return;
                        tapenoise[tpnoisep++]=sinewave[((int)swavepos)&0x1F]*((dat&1)?50:64);
                        if (dat&1) swavepos+=wavediv;
                        else       swavepos+=(wavediv/2);
                        e++;
                }
                dat>>=1;
        }
//        swavepos=0;
        for (;e<368;e++) /*Stop bit*/
        {
                if (tpnoisep>=4410) return;
                tapenoise[tpnoisep++]=sinewave[((int)swavepos)&0x1F]*64;
                swavepos+=(wavediv/2);
        }

        addhightone();
}

int tnoise_sstat=-1,tnoise_spos;

void tapemotorchange(int stat)
{
        tnoise_sstat=stat^1;
        tnoise_spos=0;
}

void mixtapenoise(int16_t *tapebuffer)
{
        int c;
        tpnoisep=0;
        if (!sndtape) return;
//        rpclog("Mix!\n");

        for (c=0;c<4410;c++)
        {
                tapebuffer[c]+=tapenoise[c];
                tapenoise[c]=0;
        }

        for (c=0;c<4410;c++)
        {
                if (tnoise_sstat>=0)
                {
//                        rpclog("SSTAT %i %i\n",tnoise_sstat,c);
                        if (tnoise_spos>=hal_sample_get_length(handle_tsamples[tnoise_sstat]))
                        {
                                tnoise_spos=0;
                                tnoise_sstat=-1;
//                                rpclog("Stat off!\n");
                        }
                        else
                        {
//                                if (!c) rpclog("MixS!\n");
                                tapebuffer[c]+=((int16_t)((((int16_t *)hal_sample_get_data_ptr(handle_tsamples[tnoise_sstat]))[(int)tnoise_spos])^0x8000)/2);
                                tnoise_spos+=((float)hal_sample_get_frequency(handle_tsamples[tnoise_sstat])/44100.0);
                        }
                }
        }

}
