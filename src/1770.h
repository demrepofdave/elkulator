/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * 1770.h - 1700 FDC Emulation
 * 
 */

#ifndef _1700_H
#define _1700_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>

/******************************************************************************
* Variable externs
*******************************************************************************/
extern int motoron;
extern int fdctime;
extern int disctime;
extern int motorspin;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void    reset1770();
uint8_t read1770 (uint16_t addr);
void    write1770(uint16_t addr, uint8_t val);


#endif // _1700_H