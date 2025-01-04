/*
 * Elkulator - An electron emulator originally written 
 *             by Sarah Walker
 *
 * mem.c - Memory handling
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "1770.h"
#include "6502.h"
#include "config_vars.h"
#include "debugger.h"
#include "elk.h"
#include "host_abstraction_layer/keyutils.h"
#include "keyboard.h"
#include "mem.h"
#include "ula.h"




static const char * roms = "roms";   // Name of directory containing rom files

int FASTLOW=0;      // 0 = Normal RAM use with sync / 1 = Fast RAM2 use (turbo and mrb?).
int FASTHIGH2=0;    // 0 = Normal, 1 = Fast? mrb & mrbmode==2.
//#define FASTLOW (elkConfig.expansion.turbo || (elkConfig.expansion.mrb && elkConfig.expansion.mrbmode && mrbmapped))
#define FASTHIGH (FASTHIGH2 && ((pc&0xE000)!=0xC000))

int mrbmapped=0;

// // ROM area - All 16 banks of 16k ROM available.
uint8_t rombanks[16][SIZE_16K];
uint8_t rombank_enabled[16];

// Main Ram area, either normal RAM, and master ram board ram (if configured)
uint8_t ram[SIZE_32K],ram2[SIZE_32K];

// Operating System Area, either normal OS 1.00 or Master Ram board OS 3.00
uint8_t os[SIZE_16K],mrbos[SIZE_16K];

/* Use pointers to refer to banks in the general rombanks array. */
#define DFS_BANK 3
#define PLUS1_BANK 12
#define SOUND_BANK 13
#define ADFS_BANK 15
uint8_t *basic;
uint8_t *adfs;
uint8_t *dfs;
uint8_t *sndrom;
uint8_t *plus1rom;
uint8_t sndlatch;
int snden=0;
int usedrom6=0;
uint8_t ram6[SIZE_16K];
/* Banked cartridge support providing space for multiple ROMS, added for the
   Mega Games Cartridge */
#define NUM_BANKS 256
uint8_t cart0[NUM_BANKS * SIZE_16K],cart1[NUM_BANKS * SIZE_16K];
uint8_t banks[2];
/* Banks accessible via the JIM page, controlled by the paging register at
   &fcff. */
#define JIM_BANKS 128
uint8_t jim_ram[JIM_BANKS][256];
int jim_page = 0;

/* Loads a rom file into a 16k Buffer*/
void loadrom(uint8_t dest[SIZE_16K], char *name)
{
    FILE *f = fopen(name, "rb");
    if (f == NULL) {
        fprintf(stderr, "Failed to load ROM file '%s'.\n", name);
        exit(1);
    }
    fread(dest, SIZE_16K, 1, f);
    fclose(f);
}

/* Loads a rom file into a particular rom bank 16k Buffer */
void loadrom_n(int bank, char *name)
{
    loadrom(rombanks[bank], name);
    rombank_enabled[bank] = 1;
}

/* Main routine to load all ROMs and correctly setup rom pointers */
void update_rom_config(void)
{
    rombank_enabled[PLUS1_BANK] = elkConfig.expansion.plus1;
    rombank_enabled[SOUND_BANK] = elkConfig.sound.sndex;
    rombank_enabled[ADFS_BANK]  = (elkConfig.expansion.plus3 && elkConfig.expansion.adfsena);
    rombank_enabled[DFS_BANK]   = (elkConfig.expansion.plus3 && elkConfig.expansion.dfsena);
}

void loadroms()
{
        for (int i = 0; i < 16; i++) {
            memset(rombanks[i], 0, SIZE_16K);
            rombank_enabled[i] = 0;
        }

        /* Clear paged RAM. */
        for (int i = 0; i < JIM_BANKS; i++) {
            memset(jim_ram[i], 0, 256);
        }

        dfs = rombanks[DFS_BANK];
        basic = rombanks[0xa];
        plus1rom = rombanks[PLUS1_BANK];
        sndrom = rombanks[SOUND_BANK];
        adfs = rombanks[ADFS_BANK];

        char path[MAX_PATH_FILENAME_BUFFER_SIZE + sizeof(roms)];
        char p2[MAX_PATH_FILENAME_BUFFER_SIZE];
        getcwd(p2,MAX_PATH_FILENAME_BUFFER_SIZE - 1);
        sprintf(path,"%s%s",exedir, roms);
        printf("path now %s\n",path);
        chdir(path);
        loadrom(os, "os");
        loadrom(mrbos, "os300.rom");
        loadrom(basic, "basic.rom");
        memcpy(rombanks[0xb], basic, SIZE_16K);
        loadrom(adfs, "adfs.rom");
        loadrom(dfs, "dfs.rom");
        loadrom(sndrom, "sndrom");
        loadrom(plus1rom, "plus1.rom");
        chdir(p2);
}

void loadcart(const char *filename)
{
        FILE *f=fopen(filename,"rb");
        if (!f) return;
        fread(cart0,NUM_BANKS * SIZE_16K,1,f);
        fclose(f);
}

void loadcart2(const char *filename)
{
        FILE *f=fopen(filename,"rb");
        if (!f) return;
        fread(cart1,NUM_BANKS * SIZE_16K,1,f);
        fclose(f);
}

void unloadcart()
{
        memset(cart0,0,NUM_BANKS * SIZE_16K);
        memset(cart1,0,NUM_BANKS * SIZE_16K);
}

void dumpram()
{
        FILE *f=fopen("ram.dmp","wb");
        fwrite(ram,SIZE_32K,1,f);
        fclose(f);
}

void resetmem()
{
        update_rom_config();

        FASTLOW = elkConfig.expansion.turbo || (elkConfig.expansion.mrb && elkConfig.expansion.mrbmode);
        FASTHIGH2 = (elkConfig.expansion.mrb && elkConfig.expansion.mrbmode==2);
        banks[0] = 0;
        banks[1] = 0;
        if (elkConfig.expansion.enable_mgc) 
        {
            fprintf(stderr, "ROM slot 0 bank = %i\n", banks[0]);
        }

        /* Initialise the current RAM bank paged into page FD (JIM). */
        jim_page = 0;
}

uint8_t readmem(uint16_t addr)
{
        if (debugon) debugread(addr);
        if (addr==pc) fetchc[addr]=31;
        else          readc[addr]=31;
        if (addr<0x2000)
        {
                if (FASTLOW) return ram2[addr];
                waitforramsync();
                return ram[addr];
        }
        if (addr<0x8000)
        {
                if (FASTHIGH) return ram2[addr];
                waitforramsync();
                return ram[addr];
        }
        if (addr<0xC000)
        {
                if (!extrom)
                {
                        if (intrombank&2) return basic[addr&0x3FFF];
                        return readkeys(addr);
                }
                /* Treat cartridges specially for now. */
                if (rombank==0) return cart0[(banks[0] * SIZE_16K) + (addr&0x3FFF)];
                if (rombank==1) return cart1[(banks[1] * SIZE_16K) + (addr&0x3FFF)];

                /* Handle other ROMs. */
                if (rombank_enabled[rombank])
                    return rombanks[rombank][addr & 0x3fff];

                if (rombank==0x6) return ram6[addr&0x3FFF];

                return addr>>8;
        }
        switch (addr&0xFF00) {
        case 0xFC00:
        {
            if ((addr&0xFFF8)==0xFCC0 && elkConfig.expansion.plus3) return read1770(addr);
            if (addr==0xFCC0 && elkConfig.expansion.firstbyte) return readfirstbyte();
            if (elkConfig.expansion.plus1)
            {
                    if (addr==0xFC70) return readadc();
                    if (addr==0xFC72) return getplus1stat();
            }
            #ifndef WIN32
                if (addr>=0xFC60 && addr<=0xFC6F && elkConfig.expansion.plus1) return readserial(addr);
            #endif

            /* Allow the JIM paging register to be read if enabled directly or
               indirectly. */
            if (elkConfig.expansion.enable_jim && (addr == 0xfcff))
                return jim_page;

            return addr>>8;
        }
        case 0xFD00: /* Paged RAM exposed in page FD */
        {
            if (elkConfig.expansion.enable_jim) {
                //fprintf(stdout, "FD: (%02x) %04x %02x\n", jim_page, addr, jim_ram[jim_page][addr & 0xff]);
                return jim_ram[jim_page & 0x7f][addr & 0xff];
            }else
                return addr>>8;
        }
        case 0xFE00:
            return readula(addr);
        default:
            break;
        }
        if (elkConfig.expansion.mrb) return mrbos[addr&0x3FFF];
        return os[addr&0x3FFF];
}

extern uint16_t pc;
void writemem(uint16_t addr, uint8_t val)
{
        if (debugon) debugwrite(addr,val);
        writec[addr]=31;
//        if (addr==0x5820) rpclog("Write 5820\n");
//        if (addr==0x5B10) rpclog("Write 5B10\n");
//        if (addr==0x5990) rpclog("Write 5990\n");
//        if (addr==0x5D70) rpclog("Write 5D70\n");
//        if (addr==0x6798) rpclog("Write 6798\n");
//        if (addr==0x5C00) rpclog("Write 5C00\n");
//        if (addr==0x6710) rpclog("Write 6710\n");
//        if (addr==0x5FC0) rpclog("Write 5FC0\n");
//        if (addr==0x6490) rpclog("Write 6490\n");
//        if (addr==0x5820) rpclog("Write 5820\n");
//        if (addr>=0x5800 && addr<0x8000) rpclog("Write %04X %02X %04X %i %i\n",addr,val,pc,FASTHIGH,FASTHIGH2);
//if (addr>0xFC00) rpclog("Write %04X %02X\n",addr,val);
        if (addr<0x2000)
        {
                if (FASTLOW) ram2[addr]=val;
                else
                {
                        waitforramsync();
                        ram[addr]=val;
                }
                return;
        }
        if (addr<0x8000)
        {
                if (FASTHIGH)
                {
                        ram2[addr]=val;
                        return;
                }
                waitforramsync();
                ram[addr]=val;
                return;
        }
        if (addr<0xC000)
        {
                if (extrom && rombank==SOUND_BANK && (addr&0x2000)) sndrom[addr&0x3FFF]=val;
                if (extrom && rombank==DFS_BANK && elkConfig.expansion.plus3 && elkConfig.expansion.dfsena) dfs[addr&0x3FFF]=val;
                if (extrom && rombank==0x6) { ram6[addr&0x3FFF]=val; usedrom6=1; }
        }
        switch (addr & 0xFF00) {
        case 0xFE00:
            writeula(addr,val);
            break;
        case 0xFD00:    /* Paged RAM exposed in page FD */
            //fprintf(stdout, "FD: (%02x) %04x %02x %02x\n", jim_page, addr, jim_ram[jim_page][addr & 0xff], val);
            if (elkConfig.expansion.enable_jim)
                jim_ram[jim_page & 0x7f][addr & 0xff] = val;
            break;
        default:
            break;
        }
        if ((addr&0xFFF8)==0xFCC0 && elkConfig.expansion.plus3) write1770(addr,val);

        if (addr==0xFC98)
        {
//                rpclog("FC98 write %02X\n",val);
                sndlatch=val;
        }
        if (addr==0xFC99)
        {
//                rpclog("FC99 write %02X\n",val);
                if ((val&1) && !snden)
                {
//                        rpclog("Writesound! %02X\n",sndlatch);
                        writesound(sndlatch);
                }
                snden=val&1;
        }

        /* Master RAM board - RAM control */
        if (addr==0xFC7F && elkConfig.expansion.mrb)
        {
                mrbmapped=!(val&0x80);
                FASTLOW=(elkConfig.expansion.turbo || (elkConfig.expansion.mrb && elkConfig.expansion.mrbmode && mrbmapped));
                FASTHIGH2=(elkConfig.expansion.mrb && elkConfig.expansion.mrbmode==2 && mrbmapped);
                
//                rpclog("Write MRB %02X %i %i %04X\n",val,FASTLOW,FASTHIGH2,pc);
//                if (!val) output=1;
        }

        /* ADC expansion - Only available if plus1 is enabled*/
        if (addr==0xFC70 && elkConfig.expansion.plus1) writeadc(val);
        #ifndef WIN32
                /* 6850 ACIA Serial port - Only available if plus1 is enabled*/
                if (addr>=0xFC60 && addr<=0xFC6F && elkConfig.expansion.plus1) return writeserial(addr, val);
                /* Centronics Parallel Interface - Only available if plus1 is enabled*/
                if (addr==0xFC71 && elkConfig.expansion.plus1) writeparallel(val);
        #endif // WIN32
        /* The Mega Games Cartridge uses FC00 to select pairs of 16K banks in
           the two sets of ROMs. */
        if (elkConfig.expansion.enable_mgc && (addr == 0xfc00)) {
            fprintf(stderr, "bank = %i\n", val); banks[0] = val; banks[1] = val;
        }
        /* DB: My cartridge uses FC73 to select 32K regions in a flash ROM.
           For convenience we use the same paired 16K ROM arrangement as for
           the MGC. */
        if (elkConfig.expansion.enable_db_flash_cartridge && (addr == 0xfc73)) {
            fprintf(stderr, "bank = %i\n", val); banks[0] = val; banks[1] = val;
        }
        /* Support the JIM paging register when enabled directly or indirectly. */
        if (elkConfig.expansion.enable_jim && (addr == 0xfcff)) {
            jim_page = val;
            //fprintf(stdout, "JIM: %02x pc=%04x (&f4)=%02x\n", jim_page, pc, ram[0xf4]);
        }
}

// Represents the internal mapping of the keyboard to memory locations
// 0x9FFF to 0xBFFE.
//
// See page 218 of the Advanced User Guide for the Acorn Electron
// for full details.
//
// Note the mappings are repeated twice as some Allegro distinguishes
// between keys more than the electron.  For example Left Shift and Right shift
// are mapped to the Electron shift key.
//
int keys[14][4]=
{
    // TODO: Check below the advanced user manual
    { ELK_KEY_RIGHT,  ELK_KEY_COPY, 0,              ELK_KEY_SPACE    },
    { ELK_KEY_LEFT,   ELK_KEY_DOWN, ELK_KEY_RETURN, ELK_KEY_DEL      },
    { ELK_KEY_EQUALS, ELK_KEY_UP,   ELK_KEY_SEMICOLON,  0            },
    { ELK_KEY_0,      ELK_KEY_P,    ELK_KEY_COLON,  ELK_KEY_SLASH    },  // TODO: Key COLON2 may not be correct.
    { ELK_KEY_9,      ELK_KEY_O,    ELK_KEY_L,      ELK_KEY_FULLSTOP },
    { ELK_KEY_8,      ELK_KEY_I,    ELK_KEY_K,      ELK_KEY_COMMA    },
    { ELK_KEY_7,      ELK_KEY_U,    ELK_KEY_J,      ELK_KEY_M        },
    { ELK_KEY_6,      ELK_KEY_Y,    ELK_KEY_H,      ELK_KEY_N        },
    { ELK_KEY_5,      ELK_KEY_T,    ELK_KEY_G,      ELK_KEY_B        },
    { ELK_KEY_4,      ELK_KEY_R,    ELK_KEY_F,      ELK_KEY_V        },
    { ELK_KEY_3,      ELK_KEY_E,    ELK_KEY_D,      ELK_KEY_C        },
    { ELK_KEY_2,      ELK_KEY_W,    ELK_KEY_S,      ELK_KEY_X        },
    { ELK_KEY_1,      ELK_KEY_Q,    ELK_KEY_A,      ELK_KEY_Z        },
    { ELK_KEY_ESCAPE, ELK_KEY_FUNCTION,  ELK_KEY_CONTROL,  ELK_KEY_SHIFT   }
};

// Allegro allows 128 keys to be used.
//
// This array contains a mapping for each and every one of them.
//
// Binary format for each entry is: edddcccc
//    e = 1 - The allegro key is mapped to an electron key.
//        0 - The allegro key is not mapped to an electron key (not in use)
//    d = bit number for mapping the key to the electron key colum bitmap.
//          - 000 = Mapped to bit 0
//          - 001 = Mapped to bit 1
//          - 010 = Mapped to bit 2
//          - 011 = Mapped to bit 3
//    c = The address bitmap mask for the key (tells us which Column address this key belongs to)
//          - 0000 = Address bitmap shifted by 0 = 00000001 = When applied if matches to address for column 0 = BFFE
//          - 0001 = Address bitmap shifted by 1 = 00000010 = When applied if matches to address for column 0 = BFFD
//          - 0010 = Address bitmap shifted by 2 = 00000100 = When applied if matches to address for column 0 = BFFB
//          - 0011 = Address bitmap shifted by 3 = 00001000 = When applied if matches to address for column 0 = BFF7
//          - 0100 = Address bitmap shifted by 4 = 00010000 = When applied if matches to address for column 0 = BFEF
//          ...
//
int keyl[ELK_KEY_MAX];

void makekeyl()
{
        int c,d;
        memset(keyl,0,sizeof(keyl));

        /* Establish a mapping from emulated key presses to keyboard matrix values. */

        for (c=0;c<14;c++)
        {
                for (d=0;d<4;d++)
                {
                        // This creates a bitmap of 1dddcccc.
                        // Top bit set means this entry is defined.
                        // c = the column number which is used to create the address mask
                        // d is bit number, used to correctly set the bit in the electrons memory address (if key is pressed)
                        // For examples see above.
                        keyl[keys[c][d]]=c|(d<<4)|0x80;
                }
        }

        // Rest of keyboard intialisation.
        keyboard_makelayout();
        //keyboard_debug_dump();
}

uint8_t readkeys(uint16_t addr)
{
        int d;
        uint8_t temp=0;

        // Allegro4 has a array called key which indicates which allegro keys are being pressed.
        // We need to convert that to pressed elk keys prior to "reading" the elk keyboard.
        keyutils_refresh_elkkeys();

        for (d=0;d<ELK_KEY_MAX;d++)
        {
            if (keyboard_elk_key_state(d) && keyl[d]&0x80 && !(addr&(1<<(keyl[d]&15)))) temp|=1<<((keyl[d]&0x30)>>4);
        }

        return temp;
}

/* Appends memory to a specified file (based on configuration) */
void savememstate(FILE *f)
{
        fwrite(ram,SIZE_32K,1,f);
        if (elkConfig.expansion.mrb) fwrite(ram2,SIZE_32K,1,f);
        if (elkConfig.expansion.plus3 && elkConfig.expansion.dfsena) fwrite(dfs,SIZE_16K,1,f);
        if (elkConfig.sound.sndex)  fwrite(sndrom,SIZE_16K,1,f);
        if (usedrom6) fwrite(ram6,SIZE_16K,1,f);
}

/* Reads memory from a specified file (based on already read configuration) */
void loadmemstate(FILE *f)
{
        fread(ram,SIZE_32K,1,f);
        if (elkConfig.expansion.mrb) fread(ram2,SIZE_32K,1,f);
        if (elkConfig.expansion.plus3 && elkConfig.expansion.dfsena) fread(dfs,SIZE_16K,1,f);
        if (elkConfig.sound.sndex)  fread(sndrom,SIZE_16K,1,f);
        if (usedrom6) fread(ram6,SIZE_16K,1,f);
}
