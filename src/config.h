/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * config.c - Handles loading and saving of configuration from elk.cfg
 * 
 */

#ifndef _CONFIG_H
#define _CONFIG_H

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdint.h>

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void loadconfig();
void saveconfig();


#endif // _CONFIG_H