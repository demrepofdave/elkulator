/*Elkulator v1.0 by Sarah Walker
  Header file for memory management*/

#ifndef __INC_MEM_H
#define __INC_MEM_H

#include <stdint.h>

// RAM / ROM Size definitons

#define SIZE_16K  16384
#define SIZE_32K  32768

// External variables
extern uint8_t ram[32768];  // Used in ula.c main electron ram bank (0x0000-0x7FFF)
extern int plus1;           // Configuration Item - 0 = Plus 1 disabled, 1 = Plus 1 enabled

// Global functions - memory handling
uint8_t readmem(uint16_t addr);
void writemem(uint16_t addr, uint8_t val);
void resetmem();
void dumpram();
void loadmemstate(FILE *f);
void savememstate(FILE *f);

// Global functions - Rom handling
void loadroms();
void loadrom_n(int, char *fn);
void loadcart(char *fn);
void loadcart2(char *fn);
void unloadcart();

#endif // __INC_MEM_H