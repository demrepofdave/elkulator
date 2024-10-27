/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * ssd.h - SSD/DSD disc handling
 * 
 */

#ifndef _SSD_H
#define _SSD_H

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

void ssd_reset();
void ssd_load (int drive, char *fn);
void ssd_close(int drive);
void dsd_load (int drive, char *fn);
void ssd_seek (int drive, int track);

void ssd_readsector (int drive, int sector, int track, int side, int density);
void ssd_writesector(int drive, int sector, int track, int side, int density);
void ssd_readaddress(int drive, int sector, int side, int density);
void ssd_format     (int drive, int sector, int side, int density);

void ssd_poll();

#endif // _SSD_H