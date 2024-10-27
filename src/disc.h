/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * disc.h - Disc support (also some tape)
 * 
 */

#ifndef _DISC_H
#define _DISC_H

/******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>


/******************************************************************************
* Typedefs
*******************************************************************************/
struct drives
{
        void (*seek)(int drive, int track);
        void (*readsector)(int drive, int sector, int track, int side, int density);
        void (*writesector)(int drive, int sector, int track, int side, int density);
        void (*readaddress)(int drive, int track, int side, int density);
        void (*format)(int drive, int track, int side, int density);
        void (*poll)();
};

/******************************************************************************
* Variable externs
*******************************************************************************/
extern struct drives drives[2];
extern int discchanged[2];
extern int curdrive;


/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void load_disc_0_2(const char *filename);
void load_disc_1_3(const char *filename);

void loaddisc (int drive, char *fn);
void newdisc  (int drive, char *fn);
void closedisc(int drive);

void disc_reset();
void disc_poll();

void disc_seek       (int drive, int track);
void disc_readsector (int drive, int sector, int track, int side, int density);
void disc_writesector(int drive, int sector, int track, int side, int density);
void disc_readaddress(int drive, int track, int side, int density);
void disc_format     (int drive, int track, int side, int density);

extern void (*fdccallback)();
extern void (*fdcdata)(uint8_t dat);
extern void (*fdcspindown)();
extern void (*fdcfinishread)();
extern void (*fdcnotfound)();
extern void (*fdcdatacrcerror)();
extern void (*fdcheadercrcerror)();
extern void (*fdcwriteprotect)();
extern int  (*fdcgetdata)(int last);

#endif // _DISC_H