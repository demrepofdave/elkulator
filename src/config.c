/*Elkulator v1.0 by Sarah Walker
  Configuration file handling*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "elk.h"
#include "config_vars.h"
#include "config.h"

static const char * elk_cfg_filename = "/elk.cfg"; // Filename for elkulator config file.

elk_config_t elkConfig;

FILE *cfgfile;
uint8_t cfgbuffer[1024];
int sndex;
int sndint;
int firstbyte;
int joffset;
int enable_mgc;
int enable_db_flash_cartridge;
int enable_jim;

// TODO: Probably do not need this as loadconfig specifies defaults;

void init_config()
{
        elkConfig.expansion.plus1 = 0;
        elkConfig.expansion.plus3 = 0;
        elkConfig.expansion.mrb = 0;
        elkConfig.expansion.mrbmode = 0;
        elkConfig.expansion.turbo = 0;
}

char *getstringcfg(char *name)
{
        char *t;
        int c;
        int a,b;
        if (!cfgfile) return NULL;
//        rpclog("Looking for %s\n",name);
        fseek(cfgfile,0,SEEK_SET);
        while (1)
        {
                t=fgets(cfgbuffer,1024,cfgfile);
                //rpclog("New string - %s\n",cfgbuffer);
                if (!t) return 0;
                c=0;
                while (name[c] && cfgbuffer[c])
                {
                        a=toupper(name[c]);
                        b=toupper(cfgbuffer[c]);
                        if (a!=b) break;
                        c++;
                }
                if (name[c]) continue;
                if (!cfgbuffer[c]) continue;
//                rpclog("Matched - now %s\n",&cfgbuffer[c]);
                while (cfgbuffer[c] && cfgbuffer[c]!='=') c++;
                if (!cfgbuffer[c]) continue;
                c++;
//                rpclog("Found equals - now %s\n",&cfgbuffer[c]);
                while (cfgbuffer[c] && cfgbuffer[c]==' ') c++;
                if (!cfgbuffer[c]) continue;
                for (b=strlen(&cfgbuffer[c]);b>=0;b--)
                {
                        if (((char *)&cfgbuffer[c])[b]==13 || ((char *)&cfgbuffer[c])[b]==10)
                        {
                                ((char *)&cfgbuffer[c])[b]=0;
                                break;
                        }
                }
//                rpclog("Over! found %s\n",&cfgbuffer[c]);
                return &cfgbuffer[c];
        }
}

int getintcfg(char *name, int def)
{
        int c;
        char *s=getstringcfg(name);
        if (!s) return def;
//        rpclog("Returned string %s for %s\n",s,name);
        sscanf(s,"%i",&c);
//        rpclog("c is %i\n",c);
        return c;
}

void writestringcfg(char *name, char *s)
{
        if (s[0]) fprintf(cfgfile,"%s = %s\n",name,s);
}
void writeintcfg(char *name, int i)
{
        fprintf(cfgfile,"%s = %i\n",name,i);
}

void loadconfig()
{
        char *s;
        int c;
        char s2[20];
        char fn[MAX_PATH_FILENAME_BUFFER_SIZE + strlen(elk_cfg_filename)];
        sprintf(fn,"%s%s",exedir, elk_cfg_filename);
        cfgfile=fopen(fn,"rt");
        elkConfig.tape.speed                 = getintcfg("tapespeed",0);
        elkConfig.expansion.plus1            = getintcfg("plus1",0);
        elkConfig.expansion.plus3            = getintcfg("plus3",0);
        elkConfig.expansion.dfsena           = getintcfg("dfsena",0);
        elkConfig.expansion.adfsena          = getintcfg("adfsena",0);
        elkConfig.expansion.defaultwriteprot = getintcfg("defaultwriteprotect",1);
        
        elkConfig.expansion.turbo   = getintcfg("turbo",0);
        elkConfig.expansion.mrb     = getintcfg("mrb",0);
        elkConfig.expansion.mrbmode = getintcfg("mrbmode",0);
        ulamode=getintcfg("ulamode",0);
        enable_jim = getintcfg("enable_jim",0);

        drawmode=getintcfg("filter",0);
        
        s=getstringcfg("discname_0");
        if (s)
        {
                strcpy(discname,s);
                loaddisc(0,discname);
        }
        else   discname[0]=0;
        s=getstringcfg("discname_1");
        if (s)
        {
                strcpy(discname2,s);
                loaddisc(1,discname2);
        }
        else   discname2[0]=0;

        sndint=getintcfg("sound_internal",1);
        sndex=getintcfg("sound_exp",0);
        sndddnoise=getintcfg("sound_ddnoise",1);
        ddvol=getintcfg("sound_ddvol",2);
        ddtype=getintcfg("sound_ddtype",1);
        sndtape=getintcfg("sound_tape",0);
        
        videoresize=getintcfg("win_resize",0);
        
        firstbyte=getintcfg("joy_firstbyte",0);
        
        joffset=getintcfg("joy_offset",0);

        // Just use default keyboard for now for allegro5.
        #ifdef HAL_ALLEGRO_4
        for (c=0;c<128;c++)
        {
                sprintf(s2,"key_define_%03i",c);
                keylookup[c]=getintcfg(s2,c);
        }
        #endif

        /* Cartridge expansions */
        enable_mgc = getintcfg("enable_mgc", 0);
        enable_db_flash_cartridge = getintcfg("enable_db_flash_cartridge", 0);

        fclose(cfgfile);
}

void saveconfig()
{
        int c;
        char s[20];
        char fn[MAX_PATH_FILENAME_BUFFER_SIZE + strlen(elk_cfg_filename)];
        sprintf(fn,"%s%s",exedir, elk_cfg_filename);
        cfgfile=fopen(fn,"wt");
        writeintcfg("tapespeed",elkConfig.tape.speed);
        writeintcfg("plus1",elkConfig.expansion.plus1);
        writeintcfg("plus3",elkConfig.expansion.plus3);
        writeintcfg("dfsena", elkConfig.expansion.dfsena);
        writeintcfg("adfsena",elkConfig.expansion.adfsena);
        writeintcfg("defaultwriteprotect",elkConfig.expansion.defaultwriteprot);
        
        writestringcfg("discname_0",discname);
        writestringcfg("discname_1",discname2);

        writeintcfg("turbo",  elkConfig.expansion.turbo);
        writeintcfg("mrb",    elkConfig.expansion.mrb);
        writeintcfg("mrbmode",elkConfig.expansion.mrbmode);
        writeintcfg("ulamode",ulamode);
        writeintcfg("enable_jim", enable_jim);
        
        writeintcfg("filter",drawmode);
        
        writeintcfg("sound_internal",sndint);
        writeintcfg("sound_exp",sndex);

        writeintcfg("sound_ddnoise",sndddnoise);
        writeintcfg("sound_ddvol",ddvol);
        writeintcfg("sound_ddtype",ddtype);
        
        writeintcfg("sound_tape",sndtape);

        writeintcfg("win_resize",videoresize);
        
        writeintcfg("joy_firstbyte",firstbyte);
        
        writeintcfg("joy_offset",joffset);
        
        // Use default keyboard for now.
        #ifdef HAL_ALLEGRO_4
        for (c=0;c<128;c++)
        {
                sprintf(s,"key_define_%03i",c);
                writeintcfg(s,keylookup[c]);
        }
        #endif

        /* Cartridge expansions */
        writeintcfg("enable_mgc", enable_mgc);
        writeintcfg("enable_db_flash_cartridge", enable_db_flash_cartridge);

        fclose(cfgfile);
}
