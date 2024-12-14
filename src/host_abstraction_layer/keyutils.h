/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * keyutils.c
 * 
 * A collection of keyboard utilities common to both the 
 * hardware abstraction layer and the main electron code.
 *
 * Helps with things like string to key id conversion.
 * to be abstracted from the actual electron code
 * 
 */

#ifndef _KEYUTILS_H
#define _KEYUTILS_H

/******************************************************************************
* Include files
*******************************************************************************/
#include "keyboard.h"

/******************************************************************************
* Preprocessor Macros
*******************************************************************************/


/******************************************************************************
* Typedefs
*******************************************************************************/


/******************************************************************************
* Public Function Definitions
*******************************************************************************/

host_key_t   keyutils_get_hostkey_from_legacy_keyid(uint8_t old_key_id);
const char * keyutils_get_hostkey_longname(host_key_t host_key);
const char * keyutils_get_hostkey_config_string(host_key_t host_key);
const char * keyutils_get_elkkey_longname(elk_key_id_t elk_key);
const char * keyutils_get_elkkey_config_string(elk_key_id_t elk_key);

elk_key_id_t keyutils_config_string_to_elk_key_id(const char * host_config_str);

#endif // _KEYUTILS_H