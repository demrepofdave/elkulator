/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * keyutils.h
 * 
 * A collection of keyboard utilities common to both the 
 * hardware abstraction layer and the main electron code.
 *
 * Helps with things like string to legacy key id 
 * conversion.
 * 
 */

#ifndef _KEYUTILS_H
#define _KEYUTILS_H

/******************************************************************************
* Include files
*******************************************************************************/
#include "keyboard.h"

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

host_key_t keyutils_get_hostkey_from_legacy_keyid(uint8_t old_key_id);
void       keyutils_refresh_elkkeys();  // Allegro 4 only.

#endif // _KEYUTILS_H