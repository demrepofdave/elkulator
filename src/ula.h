/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * ula.c - ULA and video emulation
 * 
 */

#ifndef _ULA_H
#define _ULA_H

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdint.h>
#include <stdio.h>


/******************************************************************************
* Typedefs and Enums
*******************************************************************************/
enum
{
        ULA_CONVENTIONAL = 0,
        ULA_RAM_8BIT = 2,
        ULA_RAM_8BIT_DUAL_ACCESS = 2,
        ULA_RAM_8BIT_SINGLE_ACCESS = 3
};


/******************************************************************************
* Variable externs
*******************************************************************************/
extern int ulacycles;
extern int tapeon;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/

void initula();
void resetula();
uint8_t readula(uint16_t addr);
void writeula(uint16_t addr, uint8_t val);

void yield();
void waitforramsync();
void intula(uint8_t num);
void receive(uint8_t val);

void enterfullscreen();
void leavefullscreen();

void savescrshot(const char * filename);
void loadulastate(FILE *f);
void saveulastate(FILE *f);

void startmovie();
void stopmovie();

#endif // _ULA_H