/*Elkulator v1.0 by Sarah Walker
  Disc drive noise*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "hal/hal.h"
#include "elk.h"


static const char * ddnoise35 = "ddnoise/35";   // Name of directory containing wav files to emulate noise of 3.5 inch Disk Drive
static const char * ddnoise525 = "ddnoise/525"; // Name of directory containing wav files to emulate noise of 5.25 inch Disk Drive

int ddvol=3;
hal_sample_handle handle_seeksmp[4][2] = { { -1, -1}, {-1, -1}, {-1,-1}, {-1, -1} };
hal_sample_handle handle_motorsmp[3] = { -1, -1, -1}; // TODO: Tide this initialisation up. //, *seek2smp,*seek3smp; // We seem to need the last two pointers otherwise sound goes haywire.  This suggest a memory overrun to me.

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
        getcwd(p2,(MAX_PATH_FILENAME_BUFFER_SIZE - 1));
        printf("In %s\n",p2);
        if (ddtype) sprintf(path,"%s%s",exedir, ddnoise35);
        else        sprintf(path,"%s%s",exedir, ddnoise525);
        printf("path now %s\n",path);
        chdir(path);
        handle_seeksmp[0][0] = hal_allocate_sample_and_load_wav("stepo.wav");
        if(handle_seeksmp[0][0] >= 0) // valid handle.
        {
                handle_seeksmp[0][1]=hal_allocate_sample_and_load_wav("stepi.wav");
                handle_seeksmp[1][0]=hal_allocate_sample_and_load_wav("seek1o.wav");
                handle_seeksmp[1][1]=hal_allocate_sample_and_load_wav("seek1i.wav");
                handle_seeksmp[2][0]=hal_allocate_sample_and_load_wav("seek2o.wav");
                handle_seeksmp[2][1]=hal_allocate_sample_and_load_wav("seek2i.wav");
                handle_seeksmp[3][0]=hal_allocate_sample_and_load_wav("seek3o.wav");
                handle_seeksmp[3][1]=hal_allocate_sample_and_load_wav("seek3i.wav");
        }
        else
        {
                handle_seeksmp[0][0]=hal_allocate_sample_and_load_wav("step.wav");
                handle_seeksmp[0][1]=hal_allocate_sample_and_load_wav("step.wav");
                handle_seeksmp[1][0]=hal_allocate_sample_and_load_wav("seek.wav");
                handle_seeksmp[1][1]=hal_allocate_sample_and_load_wav("seek.wav");
                handle_seeksmp[2][0]=hal_allocate_sample_and_load_wav("seek3.wav");
                handle_seeksmp[2][1]=hal_allocate_sample_and_load_wav("seek3.wav");
                handle_seeksmp[3][0]=hal_allocate_sample_and_load_wav("seek2.wav");
                handle_seeksmp[3][1]=hal_allocate_sample_and_load_wav("seek2.wav");
        }

        handle_motorsmp[0] = hal_allocate_sample_and_load_wav("motoron.wav");
        handle_motorsmp[1] = hal_allocate_sample_and_load_wav("motor.wav");
        handle_motorsmp[2] = hal_allocate_sample_and_load_wav("motoroff.wav");
        
        chdir(p2);
        printf("done!\n");
}

void closeddnoise()
{
        int c;
        for (c=0;c<4;c++)
        {
                if(handle_seeksmp[c][0] != -1)
                {
                        hal_destroy_sample(handle_seeksmp[c][0]);
                        hal_destroy_sample(handle_seeksmp[c][1]);
                        hal_release_sample(handle_seeksmp[c][0]);
                        hal_release_sample(handle_seeksmp[c][1]);
                        handle_seeksmp[c][0]= -1;
                        handle_seeksmp[c][1]= -1;
                }
        }
        for (c=0;c<3;c++)
        {
                if(handle_motorsmp[c] != -1)
                {
                        hal_destroy_sample(handle_motorsmp[c]);
                        hal_release_sample(handle_motorsmp[c]);
                        handle_motorsmp[c]= -1;
                }
        }
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
                        if (ddnoise_mpos>=hal_sample_get_length(handle_motorsmp[ddnoise_mstat]))
                        {
                                ddnoise_mpos=0;
                                if (ddnoise_mstat!=1) ddnoise_mstat++;
                                if (ddnoise_mstat==3) ddnoise_mstat=-1;
                        }
                        if (ddnoise_mstat!=-1)
                        {
//                                if (!c) rpclog("MixM!\n");
                                ddbuffer[c]+=((int16_t)((((int16_t *)hal_sample_get_data_ptr(handle_motorsmp[ddnoise_mstat]))[(int)ddnoise_mpos])^0x8000)/2);
                                ddnoise_mpos+=((float)hal_sample_get_frequency(handle_motorsmp[ddnoise_mstat])/44100.0);
                        }
                }
        }

        for (c=0;c<4410;c++)
        {
                if (ddnoise_sstat>=0)
                {
//                        rpclog("SSTAT %i %i\n",ddnoise_sstat,c);
                        if (ddnoise_mpos>=hal_sample_get_length(handle_seeksmp[ddnoise_mstat][ddnoise_sdir]))
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
                                ddbuffer[c]+=((int16_t)((((int16_t *)hal_sample_get_data_ptr(handle_seeksmp[ddnoise_mstat][ddnoise_sdir]))[(int)ddnoise_mpos])^0x8000)/2);
                                ddnoise_spos+=((float)hal_sample_get_frequency(handle_seeksmp[ddnoise_mstat][ddnoise_sdir])/44100.0);
                        }
                }
                ddbuffer[c]=(ddbuffer[c]/3)*ddvol;
        }
        }
        
        mixtapenoise(ddbuffer);
//        fwrite(ddbuffer,4410*2,1,f2);
//rpclog("Give buffer... %i %i\n",ddnoise_mstat,ddnoise_sstat);
        givealbufferdd(ddbuffer);
        
        oldmotoron=motoron;
}
