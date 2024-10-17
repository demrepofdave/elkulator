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
* Variable externs
*******************************************************************************/
extern uint8_t ram[32768];


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
void loadmemstate(FILE *f);
void savememstate(FILE *f);

#endif // _MEM_H