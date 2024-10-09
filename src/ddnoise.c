/*Elkulator v1.0 by Sarah Walker
  Disc drive noise*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "elk.h"
#include "common/samples.h"
#include "common/sound.h"

static const char * ddnoise35 = "ddnoise/35";   // Name of directory containing wav files to emulate noise of 3.5 inch Disk Drive
static const char * ddnoise525 = "ddnoise/525"; // Name of directory containing wav files to emulate noise of 5.25 inch Disk Drive

int ddvol=3;

float ddnoise_mpos=0;
int ddnoise_mstat=-1;
int oldmotoron=0;

float ddnoise_spos=0;
int ddnoise_sstat=-1;
int ddnoise_sdir=0;
int ddtype=0;

void loaddiscsamps()
{
        char path[MAX_PATH_FILENAME_BUFFER_SIZE + strlen(ddnoise525)];
        char p2[MAX_PATH_FILENAME_BUFFER_SIZE];
        bool result = false;
        getcwd(p2,(MAX_PATH_FILENAME_BUFFER_SIZE - 1));
        printf("In %s\n",p2);
        if (ddtype) sprintf(path,"%s%s",exedir, ddnoise35);
        else        sprintf(path,"%s%s",exedir, ddnoise525);
        printf("path now %s\n",path);
        chdir(path);
        result = sample_seek_load(0, 0, "stepo.wav");
        if (result)
        {
                sample_seek_load(0, 1, "stepi.wav");
                sample_seek_load(1, 0, "seek1o.wav");
                sample_seek_load(1, 1, "seek1i.wav");
                sample_seek_load(2, 0, "seek2o.wav");
                sample_seek_load(2, 1, "seek2i.wav");
                sample_seek_load(3, 0, "seek3o.wav");
                sample_seek_load(3, 1, "seek3i.wav");
        }
        else
        {
                sample_seek_load(0, 0, "step.wav");
                sample_seek_load(0, 1, "step.wav");
                sample_seek_load(1, 0, "seek.wav");
                sample_seek_load(1, 1, "seek.wav");
                sample_seek_load(2, 0, "seek3.wav");
                sample_seek_load(2, 1, "seek3.wav");
                sample_seek_load(3, 0, "seek2.wav");
                sample_seek_load(4, 1, "seek2.wav");
        }
        sample_motor_load(0, "motoron.wav");
        sample_motor_load(1, "motor.wav");
        sample_motor_load(2, "motoroff.wav");
        chdir(p2);
        printf("done!\n");
}

void closeddnoise()
{
        sample_seek_destroy_all();
        sample_motor_destroy_all();
}

int16_t ddbuffer[4410];

void ddnoise_seek(int len)
{
        printf("Seek %i tracks\n",len);
        ddnoise_sdir=(len<0)?1:0;
        if (len<0) len=-len;
        ddnoise_spos=0;
        if (len==0) { ddnoise_sstat=-1; fdctime=200; }
        else if (len==1) { ddnoise_sstat=0; fdctime=140000; }
        else if (len<7)  ddnoise_sstat=1;
        else if (len<30) ddnoise_sstat=2;
        else             ddnoise_sstat=3;
        if (!sndddnoise) fdctime=200;
        rpclog("Start seek!\n");
}

FILE *f1,*f2;
void mixddnoise()
{
        int c;
//        if (!f1) f1=fopen("f1.pcm","wb");
//        if (!f2) f2=fopen("f2.pcm","wb");

        memset(ddbuffer,0,4410*2);
//        fwrite(ddbuffer,4410*2,1,f1);
        if (motoron && !oldmotoron)
        {
                ddnoise_mstat=0;
                ddnoise_mpos=0;
        }
        if (!motoron && oldmotoron)
        {
                ddnoise_mstat=2;
                ddnoise_mpos=0;
        }
        
        if (sndddnoise)
        {
printf("Mixing ddnoise...\n");
        for (c=0;c<4410;c++)
        {
                ddbuffer[c]=0;
                if (ddnoise_mstat>=0)
                {
                        if (ddnoise_mpos>=sample_motor_get_length(ddnoise_mstat))
                        {
                                ddnoise_mpos=0;
                                if (ddnoise_mstat!=1) ddnoise_mstat++;
                                if (ddnoise_mstat==3) ddnoise_mstat=-1;
                        }
                        if (ddnoise_mstat!=-1)
                        {
//                                if (!c) rpclog("MixM!\n");
                                ddbuffer[c]+=((int16_t)((((int16_t *)sample_motor_get_data_ptr(ddnoise_mstat))[(int)ddnoise_mpos])^0x8000)/2);
                                ddnoise_mpos+=((float)sample_motor_get_frequency(ddnoise_mstat)/44100.0);
                                //ddbuffer[c]+=((int16_t)((((int16_t *)motorsmp[ddnoise_mstat]->data)[(int)ddnoise_mpos])^0x8000)/2);
                                //ddnoise_mpos+=((float)motorsmp[ddnoise_mstat]->freq/44100.0);
                        }
                }
        }

        for (c=0;c<4410;c++)
        {
                if (ddnoise_sstat>=0)
                {
//                        rpclog("SSTAT %i %i\n",ddnoise_sstat,c);
                        if (ddnoise_spos>=sample_seek_get_length(ddnoise_sstat,ddnoise_sdir))
                        {
                                if (ddnoise_sstat>0)
                                {
                                        fdctime=100;
//                                        printf("FDC time\n");
                                }
                                ddnoise_spos=0;
                                ddnoise_sstat=-1;
//                                printf("Stat off\n");
//                                rpclog("Stat off!\n");
                        }
                        else
                        {
//                                if (!c) rpclog("MixS!\n");
                                ddbuffer[c]+=( (int16_t) ((((int16_t *)sample_seek_get_data_ptr(ddnoise_sstat,ddnoise_sdir))[(int)ddnoise_spos])^0x8000)/2);
                                ddnoise_spos+=((float)sample_seek_get_frequency(ddnoise_sstat, ddnoise_sdir)/44100.0);
                                //ddbuffer[c]+=((int16_t)((((int16_t *)seeksmp[ddnoise_sstat][ddnoise_sdir]->data)[(int)ddnoise_spos])^0x8000)/2);
                                //ddnoise_spos+=((float)seeksmp[ddnoise_sstat][ddnoise_sdir]->freq/44100.0);
                        }
                }
                ddbuffer[c]=(ddbuffer[c]/3)*ddvol;
        }
        }
        
        mixtapenoise(ddbuffer);
//        fwrite(ddbuffer,4410*2,1,f2);
//rpclog("Give buffer... %i %i\n",ddnoise_mstat,ddnoise_sstat);
        sound_givealbufferdd(ddbuffer);
        
        oldmotoron=motoron;
}
