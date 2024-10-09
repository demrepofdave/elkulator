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

/******************************************************************************
* Typedefs
*******************************************************************************/

typedef struct 
{
    uint8_t plus1;
    uint8_t plus3;
    uint8_t dfsena;
    uint8_t adfsena;
    uint8_t defaultwriteprot;
    // Turbo and Shadow board configuration
    uint8_t mrb;
    uint8_t mrbmode;
    uint8_t turbo; 
} expansion_config_t;


typedef struct
{
    uint8_t speed;
} tape_config_t;


typedef struct 
{
    tape_config_t      tape;
    expansion_config_t expansion;

} elk_config_t;

extern elk_config_t elkConfig;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

#endif // CONFIG_VARS_H