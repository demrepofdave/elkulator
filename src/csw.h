/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * csw.h - CSW handling
 * 
 */

#ifndef _CSW_H
#define _CSW_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>


/******************************************************************************
* Variable externs
*******************************************************************************/
extern int cswena;


/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void opencsw (const char *fn);
void closecsw();
void pollcsw ();

#endif // _CSW_H