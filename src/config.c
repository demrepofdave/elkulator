/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * config.c - Handles loading and saving of configuration from elk.cfg
 * 
 */

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "elk.h"
#include "config_vars.h"
#include "config.h"
#include "logger.h"
#include "disc.h"
#include "host_abstraction_layer/keyutils.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/

static const char * elk_cfg_filename = "/elk.cfg"; // Filename for elkulator config file.

/******************************************************************************
* Private Variable Definitions
*******************************************************************************/

FILE *cfgfile;
char cfgbuffer[1024];

/******************************************************************************
* Public Variable Definitions
*******************************************************************************/

elk_config_t elkConfig;


/******************************************************************************
* Private Function Definitions
*******************************************************************************/

char *getstringcfg(const char *name)
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

bool getboolcfg(char *name, bool def)
{
        int c = getintcfg(name, (int)def);
        return(c!=0?true:false);
}

void getrgbcfg(cfg_rgb_t *rgb, char *name, cfg_rgb_t def)
{
   char *s=getstringcfg(name);
   int red;
   int green;
   int blue;
   int retval;
   // Set to default in case we cannot decode
   rgb->red   = def.red;
   rgb->green = def.green;
   rgb->blue  = def.blue;

   if(s)
   {
        retval = sscanf(s,"%i,%i,%i", &red, &green, &blue);

        if(retval ==3 && red <= 255 && green <= 255 && blue <= 255)
        {
                rgb->red   = red;
                rgb->green = green;
                rgb->blue  = blue;
        }

   }
}

void writecommentcfg(const char *comment)
{
    fprintf(cfgfile,"# %s\n",comment);    
}

void writestringcfg(const char *name, const char *s)
{
    if (s[0]) fprintf(cfgfile,"%s = %s\n",name,s);
}

void writeintcfg(char *name, int i)
{
    fprintf(cfgfile,"%s = %i\n",name,i);
}

void writeboolcfg(char *name, bool b)
{
    writeintcfg(name, (b!=true?0:1));
}

void writergbcfg(char *name, cfg_rgb_t rgb)
{
    fprintf(cfgfile,"%s=%d,%d,%d\n", name, rgb.red, rgb.green, rgb.blue);
}


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void loadconfig()
{
        char *s;
        char fn[MAX_PATH_FILENAME_BUFFER_SIZE + strlen(elk_cfg_filename)];
        cfg_rgb_t border_rgb_default;

        // Default border colour is black.
        border_rgb_default.red = 0;
        border_rgb_default.green = 0;
        border_rgb_default.blue = 0;

        sprintf(fn,"%s%s",exedir, elk_cfg_filename);
        cfgfile=fopen(fn,"rt");
        printf("config file handle %s %d\n", fn, cfgfile);
        fflush(stdout);

        // Read and set log level first.
        elkConfig.stats.log_level = (log_level_t) getintcfg("log_level", (int) LOG_ERROR);
        log_set_level(elkConfig.stats.log_level);
        
        int api_version = getintcfg("api_version", 1);  // Assume old config file unless apiversion is present (2 is latest).

        log_info("Config file Api version = %d", api_version);

        elkConfig.tape.speed            = getintcfg("tapespeed",0);

        elkConfig.expansion.plus1       = getboolcfg("plus1",   false);
        elkConfig.expansion.plus3       = getboolcfg("plus3",   false);
        elkConfig.expansion.dfsena      = getboolcfg("dfsena",  false);
        elkConfig.expansion.adfsena     = getboolcfg("adfsena", false);
        elkConfig.disc.defaultwriteprot = getboolcfg("defaultwriteprotect",true);
        
        elkConfig.expansion.turbo      = getboolcfg("turbo", false);
        elkConfig.expansion.mrb        = getboolcfg("mrb", false);
        elkConfig.expansion.mrbmode    = getintcfg ("mrbmode",0);
        elkConfig.expansion.ulamode    = getintcfg ("ulamode",0);
        elkConfig.expansion.enable_jim = getboolcfg("enable_jim", false);

        elkConfig.display.drawmode=getintcfg("filter",0);

        elkConfig.display.fullscreen = 0; // Currently this is not saved in the elk.cfg file.
        
        s=getstringcfg("discname_0");
        if (s)
        {
                strcpy(elkConfig.disc.discname,s);
                loaddisc(0,elkConfig.disc.discname);
        }
        else
        {
                elkConfig.disc.discname[0]=0;
        }
        s=getstringcfg("discname_1");
        if (s)
        {
                strcpy(elkConfig.disc.discname2,s);
                loaddisc(1,elkConfig.disc.discname2);
        }
        else
        {
                elkConfig.disc.discname2[0]=0;
        }
        
        elkConfig.sound.sndint     = getintcfg("sound_internal",1);
        elkConfig.sound.sndex      = getintcfg("sound_exp",0);
        elkConfig.sound.sndddnoise = getintcfg("sound_ddnoise",1);
        // TODO: Temporarily disabled due to crash bug.
        elkConfig.sound.sndddnoise = 0;
        elkConfig.sound.ddvol      = getintcfg("sound_ddvol",2);
        elkConfig.sound.ddtype     = getintcfg("sound_ddtype",1);
        elkConfig.sound.sndtape    = getintcfg("sound_tape",0);
        // TODO: Temporarily disabled due to tape sound issues.
        elkConfig.sound.sndtape = 0;
        
        elkConfig.display.videoresize = getintcfg("win_resize",0);
        elkConfig.display.maintain_aspect_ratio = getintcfg("win_aspectratio", 1);
        elkConfig.display.maintain_pixel_ratio  = getintcfg("win_pixelratio", 0);
        elkConfig.display.native_window_width   = getintcfg("win_width", 640);
        elkConfig.display.native_window_height  = getintcfg("win_height", 512);
        getrgbcfg(&elkConfig.display.border, "border_col", border_rgb_default);
        
        elkConfig.expansion.firstbyte = getintcfg("joy_firstbyte",0);
        elkConfig.expansion.joffset   = getintcfg("joy_offset",0);

        /* New keyboard handling */
        for(int host_key = 0; host_key < HOST_KEY_MAX; host_key++)
        {
            s=getstringcfg(keyboard_get_hostkey_config_string(host_key));
            if(s)
            {
                elkConfig.keyboard.host_key_mapping[host_key] = keyboard_config_string_to_elk_key_id(s);
            }
            else
            {
                elkConfig.keyboard.host_key_mapping[host_key] = keyboard_get_default_elk_key_from_host_key(host_key);
            }
        }

        if(api_version == 1)
        {
                /* Convert old keyboard config to new keyboard config */
                /* TODO: Very hacky code but it is working - refactor */
                int key_id, key_value;
                host_key_t old_host_key;
                host_key_t old_host_assigned_key;
                char s2[20];
                // Keys 001 to xxx and xxx to 127 can be converted.
                // Other keys are undefined for allegro4.
                log_debug("Upgrading elkulator keyboard config definitions");

                for (key_id=0; key_id<128; key_id++)
                {
                        // Can key be defined (not part of the undefined set in allegro4)
                        old_host_key = keyutils_get_hostkey_from_legacy_keyid(key_id);
                        if(old_host_key != HOST_KEY_NONE)
                        {
                                sprintf(s2,"key_define_%03i", key_id);
                                key_value=getintcfg(s2,key_id);
                                // Only store if values are different.
                                if(key_id != key_value)
                                {
                                        old_host_assigned_key = keyutils_get_hostkey_from_legacy_keyid(key_value);
                                        log_debug("Key value %d = %d", key_value, key_id);
                                        log_debug("Key value %s = %s", keyboard_get_hostkey_config_string(old_host_assigned_key), keyboard_get_hostkey_config_string(old_host_key));
                                        int elk_key = keyboard_get_default_elk_key_from_host_key(old_host_assigned_key);
                                        if(old_host_assigned_key != HOST_KEY_NONE && elk_key != ELK_KEY_NONE)
                                        {
                                                elkConfig.keyboard.host_key_mapping[old_host_assigned_key] = elk_key;
                                                log_debug("%s=%s", keyboard_get_hostkey_config_string(old_host_assigned_key), keyboard_get_elkkey_config_string(elk_key));
                                        }
                                }
                        }
                }
        }

        /* Cartridge expansions */
        elkConfig.expansion.enable_mgc                = getboolcfg("enable_mgc", false);
        elkConfig.expansion.enable_db_flash_cartridge = getboolcfg("enable_db_flash_cartridge", false);

        /* Elkulator specific performance statistics */
        elkConfig.stats.titlebar_performance_stats = getboolcfg("enable_titlebar_stats", true);
        elkConfig.stats.blitting_performance_stats = getboolcfg("enable_blitting_stats", false);

        fclose(cfgfile);
}

void saveconfig()
{
        char fn[MAX_PATH_FILENAME_BUFFER_SIZE + strlen(elk_cfg_filename)];
        sprintf(fn,"%s%s",exedir, elk_cfg_filename);

        cfgfile=fopen(fn,"wt");

        writecommentcfg("Api version (1 = elkulator v1.0, 2 = elkulator v2.0+");
        writeintcfg ("api_version", 2);   // Version 2 of the config file api (1 = old allegro4, 2 = elkulator version 2)
        writeintcfg ("tapespeed", elkConfig.tape.speed);
        writeboolcfg("plus1",     elkConfig.expansion.plus1);
        writeboolcfg("plus3",     elkConfig.expansion.plus3);
        writeboolcfg("dfsena",    elkConfig.expansion.dfsena);
        writeboolcfg("adfsena",   elkConfig.expansion.adfsena);

        writeboolcfg("defaultwriteprotect",elkConfig.disc.defaultwriteprot);
        
        writestringcfg("discname_0",elkConfig.disc.discname);
        writestringcfg("discname_1",elkConfig.disc.discname2);

        writeboolcfg("turbo",     elkConfig.expansion.turbo);
        writeboolcfg("mrb",        elkConfig.expansion.mrb);
        writeintcfg ("mrbmode",    elkConfig.expansion.mrbmode);
        writeintcfg ("ulamode"    ,elkConfig.expansion.ulamode);
        writeboolcfg("enable_jim", elkConfig.expansion.enable_jim);
        
        writeintcfg("filter",     elkConfig.display.drawmode);
        
        writeintcfg("sound_internal", elkConfig.sound.sndint);
        writeintcfg("sound_exp",      elkConfig.sound.sndex);

        writeintcfg("sound_ddnoise",  elkConfig.sound.sndddnoise);
        writeintcfg("sound_ddvol",    elkConfig.sound.ddvol);
        writeintcfg("sound_ddtype",   elkConfig.sound.ddtype);
        
        writeintcfg("sound_tape", elkConfig.sound.sndtape);

        writeintcfg("win_resize",elkConfig.display.videoresize);
        writeintcfg("win_aspectratio", elkConfig.display.maintain_aspect_ratio);
        writeintcfg("win_pixelratio", elkConfig.display.maintain_pixel_ratio);
        writeintcfg("win_width", elkConfig.display.native_window_width);
        writeintcfg("win_height", elkConfig.display.native_window_height);
        writergbcfg("border_col", elkConfig.display.border);
        
        writeintcfg("joy_firstbyte", elkConfig.expansion.firstbyte);
        writeintcfg("joy_offset",    elkConfig.expansion.joffset);
        
        /* New keyboard handling */
        writecommentcfg("Key redefinitions (note only differences from the default keys are stored)");
        for(int host_key = 0; host_key < HOST_KEY_MAX; host_key++)
        {
            if(elkConfig.keyboard.host_key_mapping[host_key] != ELK_KEY_NONE &&
               elkConfig.keyboard.host_key_mapping[host_key] != keyboard_get_default_elk_key_from_host_key(host_key)) // Not the default, so save it.
            {
                writestringcfg(keyboard_get_hostkey_config_string(host_key), keyboard_get_elkkey_config_string(elkConfig.keyboard.host_key_mapping[host_key]));
            }
        }

        /* Cartridge expansions */
        writecommentcfg("Cartridge expansions config");
        writeboolcfg("enable_mgc",                elkConfig.expansion.enable_mgc);
        writeboolcfg("enable_db_flash_cartridge", elkConfig.expansion.enable_db_flash_cartridge);

        /* Elkulator specific performance statistics */
        writeboolcfg("enable_titlebar_stats", elkConfig.stats.titlebar_performance_stats);
        writeboolcfg("enable_blitting_stats", elkConfig.stats.blitting_performance_stats);
        writecommentcfg("Logging level (0 = ERRORS - default, 1 = WARNINGS, 2 = INFO, 3 = DEBUG)");
        writeintcfg("log_level", (int) elkConfig.stats.log_level);

        fclose(cfgfile);
}

void log_config_vars()
{
    log_info("Config vars");
    log_info("===========");
    log_info("- display:");
    log_info("  - drawmode    : %d", elkConfig.display.drawmode);
    log_info("  - videoresize : %d", elkConfig.display.videoresize);
    log_info("  - aspectratio : %d", elkConfig.display.maintain_aspect_ratio);
    log_info("  - pixelratio  : %d", elkConfig.display.maintain_pixel_ratio);
    log_info("  - win width   : %d", elkConfig.display.native_window_width);
    log_info("  - win height  : %d", elkConfig.display.native_window_height);

    log_info("- expansion:");
    log_info("  - plus1                : %d", elkConfig.expansion.plus1);
    log_info("  - plus3                : %d", elkConfig.expansion.plus3);
    log_info("  - firstbyte            : %d", elkConfig.expansion.firstbyte);
    log_info("  - joffset              : %d", elkConfig.expansion.joffset);
    log_info("  - dfsena               : %d", elkConfig.expansion.dfsena);
    log_info("  - adfsena              : %d", elkConfig.expansion.adfsena);
    log_info("  - mrb                  : %d", elkConfig.expansion.mrb);
    log_info("  - mrbmode              : %d", elkConfig.expansion.mrbmode);
    log_info("  - turbo                : %d", elkConfig.expansion.turbo);
    log_info("  - ulamode              : %d", elkConfig.expansion.ulamode);
    log_info("  - enable_jim           : %d", elkConfig.expansion.enable_jim);
    log_info("  - enable_mgc           : %d", elkConfig.expansion.enable_mgc);
    log_info("  - enable_db_flash_cart : %d", elkConfig.expansion.enable_db_flash_cartridge);
    log_info("- sound:");
    log_info("  - sndint     : %d", elkConfig.sound.sndint);
    log_info("  - sndex      : %d", elkConfig.sound.sndex);
    log_info("  - sndddnoise : %d", elkConfig.sound.sndddnoise);
    log_info("  - ddvol      : %d", elkConfig.sound.ddvol);
    log_info("  - ddtype     : %d", elkConfig.sound.ddtype);
    log_info("  - sndtape    : %d", elkConfig.sound.sndtape);
    log_info("  - sndex      : %d", elkConfig.sound.sndex);
    log_info("- keyboard:");

    for(int host_key = 0; host_key < HOST_KEY_MAX; host_key++)
    {
        if(elkConfig.keyboard.host_key_mapping[host_key] != keyboard_get_default_elk_key_from_host_key(host_key))
        {
            log_info(" - %s       : %s", keyboard_get_hostkey_config_string(host_key), keyboard_get_elkkey_config_string(elkConfig.keyboard.host_key_mapping[host_key]));
        }
    }
    log_info("- tape:");
    log_info("  - speed      : %d", elkConfig.tape.speed);
    log_info("- disc:");
    log_info("  - defaultwriteprot : %d", elkConfig.disc.defaultwriteprot);
    log_info("  - discname         : %s", elkConfig.disc.discname);
    log_info("  - discname2        : %s", elkConfig.disc.discname2);

    log_info("- stats:");
    log_info("  - titlebar_performance_stats : %d", elkConfig.stats.titlebar_performance_stats);
    log_info("  - blitting_performance_stats : %d", elkConfig.stats.blitting_performance_stats);
    log_info("  - Log level                  : %d", (int)elkConfig.stats.log_level);

 }
