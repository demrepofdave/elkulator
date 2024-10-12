/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * ula.c - ULA and video emulation
 * 
 */

#ifndef _6502_H
#define _6502_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>
#include <stdio.h>


/******************************************************************************
* Typedefs
*******************************************************************************/
typedef struct
{
        int c,z,i,d,v,n;
} CPUStatus;


/******************************************************************************
* Variable externs
*******************************************************************************/
extern uint8_t a,x,y,s;
extern uint16_t pc;
extern CPUStatus p;

extern uint8_t opcode;

extern int nmi;
extern int irq;

extern int cycles;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void reset6502();
void reset6502e();
void exec6502();
void load6502state(FILE *f);
void save6502state(FILE *f);

#endif // _6502_H