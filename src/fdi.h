/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
  * fdc.h - FDI disc support.
 *         Interfaces with fdi2raw.c
 * 
 */

#ifndef _FDC_H
#define _FDC_H

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



/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void fdi_reset();
void fdi_load (int drive, char *fn);
void fdi_close(int drive);
void fdi_seek (int drive, int track);

void fdi_readsector (int drive, int sector, int track, int side, int density);
void fdi_writesector(int drive, int sector, int track, int side, int density);
void fdi_readaddress(int drive, int sector, int side, int density);
void fdi_format     (int drive, int sector, int side, int density);

void fdi_poll();

#endif // _FDC_H