/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * elk.h - Main header file
 * 
 * Contains a number of global variable and function called by the
 * electron code.
 * 
 */


#ifndef _ELK_H
#define _ELK_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* Include files
*******************************************************************************/

#include <stdint.h>
#include <stdio.h>

#define printf rpclog

void rpclog(char *format, ...);

extern int rombank,intrombank;
extern int extrom;
#ifdef HAL_ALLEGRO_4
extern int keylookup[128];
#endif

void polltape();
void polluef();

void openuef(const char *fn);
void closeuef();

extern int resetit;

extern char tapename[512];

void resetsound();

extern int mrbmapped;

void error(const char *format, ...);
//void rpclog(const char *format, ...);

void ssd_reset();
void ssd_load(int drive, char *fn);
void ssd_close(int drive);
void dsd_load(int drive, char *fn);
void ssd_seek(int drive, int track);
void ssd_readsector(int drive, int sector, int track, int side, int density);
void ssd_writesector(int drive, int sector, int track, int side, int density);
void ssd_readaddress(int drive, int sector, int side, int density);
void ssd_format(int drive, int sector, int side, int density);
void ssd_poll();

void setejecttext(int drive, char *fn);

extern int writeprot[2],fwriteprot[2];

extern uint8_t plus1stat;
extern int adctime;

extern uint8_t readc[65536],writec[65536],fetchc[65536];

extern char moviename[260];
extern uint8_t sndstreambuf[626];
extern int sndstreamptr;

extern int discspd;

// Maximum file name buffer size (previously this was defined as magic number
// in the code at 512, however it is possible that a file path on modern OS's
// may have file paths + filename combinations that exceed 512 characters, 
// hence bumping the value up here).
#define MAX_PATH_FILENAME_BUFFER_SIZE 768

extern char exedir[MAX_PATH_FILENAME_BUFFER_SIZE];

void redefinekeys();

int break_pressed();
int menu_pressed();
void update_break_keys();
void update_menu_keys();

void resetserial();
uint8_t readserial(uint16_t addr);
void writeserial(uint16_t addr, uint8_t val);
void pollserial(int cycles);

void loadtape(const char *fn);
void handle_rewind_tape(void);
void handle_eject_tape(void);
void reallyfasttapepoll();

void initsound();
void writesound(uint8_t data);
void logvols();

void loaddiscsamps();
void mixddnoise();
void closeddnoise();
void ddnoise_seek(int len);

void maketapenoise();
void adddatnoise(uint8_t dat);
void addhightone();
void mixtapenoise(int16_t *tapebuffer);

void loadstate();
void savestate();
void doloadstate(const char *filename);
void dosavestate(const char *filename);

uint8_t readfirstbyte();

uint8_t readadc();
void writeadc(uint8_t val);
uint8_t getplus1stat();

void setquit();

void entergui();

/* Plus 1 parallel port */
void resetparallel();
void writeparallel(uint8_t val);

/* Socket utilities. */
int socket_input(int socket_fd, char *buffer, size_t count);
int socket_open(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // _ELK_H