/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * adf.h - ADFS disc support (really all double-density formats
 * 
 */

#ifndef _ADF_H
#define _ADF_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>


/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void adf_reset();
void adf_load  (int drive, char *fn);
void adl_loadex(int drive, char *fn, int sectors, int size, int dblstep);
void adf_close (int drive);
void adl_load  (int drive, char *fn);
void adf_seek  (int drive, int track);

void adf_readsector (int drive, int sector, int track, int side, int density);
void adf_writesector(int drive, int sector, int track, int side, int density);
void adf_readaddress(int drive, int sector, int side, int density);

void adf_format(int drive, int sector, int side, int density);

void adf_poll();

#endif // _ADF_H