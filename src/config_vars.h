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
#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"
#include "logger.h"

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
    bool plus1;
    bool plus3;
    uint8_t firstbyte;
    int     joffset; // TODO: leaving as int for now as unsure of the range for this item.
    bool dfsena;
    bool adfsena;
    // Turbo and Shadow board configuration
    bool mrb;
    uint8_t mrbmode;
    bool    turbo;
    // Enhancements
    uint8_t ulamode;
    bool enable_jim;
    // Cartridge expansions
    bool enable_mgc;
    bool enable_db_flash_cartridge;

} expansion_config_t;


typedef struct
{
    uint8_t speed;
} tape_config_t;

typedef struct
{
    bool defaultwriteprot;
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
    uint8_t maintain_pixel_ratio;
    uint32_t native_window_width;           // Non-fullscreen mode window width.
    uint32_t native_window_height;          // Non-fullscreen mode window height.
    uint8_t fullscreen;
} display_config_t;

// Enable/disable various elkulator performance monitoring statistics
typedef struct 
{
    bool titlebar_performance_stats;
    bool blitting_performance_stats;
    log_level_t log_level;
} stats_config_t;


typedef struct
{
    elk_key_id_t host_key_mapping[HOST_KEY_MAX]; // Host key to elk key mapping (0 = use default)
} keyboard_config_t;

typedef struct 
{
    display_config_t   display;
    expansion_config_t expansion;
    sound_config_t     sound;
    keyboard_config_t  keyboard;
    tape_config_t      tape;
    disc_config_t      disc;
    stats_config_t     stats;
} elk_config_t;

extern elk_config_t elkConfig;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

#endif // CONFIG_VARS_H