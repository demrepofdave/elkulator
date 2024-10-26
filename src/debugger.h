/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * debugger.h - Main debugger handling code
 * 
 */

#ifndef _DEBUGGER_H
#define _DEBUGGER_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>


/******************************************************************************
* Typedefs
*******************************************************************************/


/******************************************************************************
* Variable externs
*******************************************************************************/
extern int debug;
extern int debugon;


/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void dodebugger();
void debugread (uint16_t addr);
void debugwrite(uint16_t addr, uint8_t val);
void startdebug();
void enddebug  ();

#endif // _DEBUGGER_H