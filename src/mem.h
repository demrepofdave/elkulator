/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * mem.h - Memory handling
 * 
 */

#ifndef _MEM_H
#define _MEM_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>
#include <stdio.h>

/******************************************************************************
* Proprocessor macros
*******************************************************************************/

// RAM / ROM Size definitons
#define SIZE_16K  16384
#define SIZE_32K  32768

/******************************************************************************
* Variable externs
*******************************************************************************/
extern int usedrom6;
extern int mrbmapped;
extern uint8_t ram[SIZE_32K];  // Used in ula.c main electron ram bank (0x0000-0x7FFF)


/******************************************************************************
* Public Function Definitions
*******************************************************************************/
uint8_t readmem(uint16_t addr);
void writemem(uint16_t addr, uint8_t val);

void loadroms();
void loadrom_n(int, char *fn);
void resetmem();
void dumpram();
void loadcart(const char *filename);
void loadcart2(const char *filename);
void unloadcart();
void makekeyl();
uint8_t readkeys(uint16_t addr);
void loadmemstate(FILE *f);
void savememstate(FILE *f);

#endif // _MEM_H