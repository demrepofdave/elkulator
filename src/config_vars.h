/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * config_var.h
 * 
 * Contains configuration structure which is used to
 * run the electron and emulator.  Most of this information
 * is loaded and stored in elk.cfg file.
 */

#ifndef _CONFIG_VARS_H
#define _CONFIG_VARS_H

/******************************************************************************
* Include files
*******************************************************************************/

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/
#define TAPE_NORMAL       0
#define TAPE_FAST         1
#define TAPE_REALLY_FAST  2

#define DISCNAME_CHARS_MAX 260

/******************************************************************************
* Typedefs
*******************************************************************************/

typedef struct 
{
    uint8_t plus1;
    uint8_t plus3;
    uint8_t firstbyte;
    int     joffset; // TODO: leaving as int for now as unsure of the range for this item.
    uint8_t dfsena;
    uint8_t adfsena;
    // Turbo and Shadow board configuration
    uint8_t mrb;
    uint8_t mrbmode;
    uint8_t turbo;
    // Enhancements
    uint8_t ulamode;
    uint8_t enable_jim;
    // Cartridge expansions
    uint8_t enable_mgc;
    uint8_t enable_db_flash_cartridge;

} expansion_config_t;


typedef struct
{
    uint8_t speed;
} tape_config_t;

typedef struct
{
    uint8_t defaultwriteprot;
    char discname[DISCNAME_CHARS_MAX];
    char discname2[DISCNAME_CHARS_MAX];
} disc_config_t;

typedef struct 
{
    uint8_t sndint;
    uint8_t sndex;
    uint8_t sndddnoise;
    uint8_t ddvol;
    uint8_t ddtype;
    uint8_t sndtape;
} sound_config_t;

typedef struct
{
    uint8_t drawmode;
    uint8_t videoresize;
    uint8_t maintain_aspect_ratio;
} display_config_t;

typedef struct 
{
    display_config_t   display;
    expansion_config_t expansion;
    sound_config_t     sound;
    tape_config_t      tape;
    disc_config_t      disc;
} elk_config_t;

extern elk_config_t elkConfig;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

#endif // CONFIG_VARS_H