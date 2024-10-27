/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * 6502.h - Emulation of the Electron 6502 CPU.
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

extern int output;

/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void reset6502();
void reset6502e();
void exec6502();
void load6502state(FILE *f);
void save6502state(FILE *f);

void dumpregs();

#endif // _6502_H