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
#include "elk.h"
#include "mem.h"
#include "6502.h"
#include "ula.h"
#include "config_vars.h"
#include "common/keyboard.h"

static const char * roms = "roms";   // Name of directory containing rom files

int FASTLOW=0;
int FASTHIGH2=0;
//#define FASTLOW (elkConfig.expansion.turbo || (elkConfig.expansion.mrb && elkConfig.expansion.mrbmode && mrbmapped))
#define FASTHIGH (FASTHIGH2 && ((pc&0xE000)!=0xC000))

extern int output;
int mrbmapped=0;
uint8_t rombanks[16][16384];
uint8_t rombank_enabled[16];

uint8_t ram[32768],ram2[32768];
uint8_t os[16384],mrbos[16384];
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
uint8_t ram6[16384];
/* Banked cartridge support providing space for multiple ROMS, added for the
   Mega Games Cartridge */
#define NUM_BANKS 256
uint8_t cart0[NUM_BANKS * 16384],cart1[NUM_BANKS * 16384];
uint8_t banks[2];
/* Banks accessible via the JIM page, controlled by the paging register at
   &fcff. */
#define JIM_BANKS 128
uint8_t jim_ram[JIM_BANKS][256];
int jim_page = 0;

void loadrom(uint8_t dest[16384], char *name)
{
    FILE *f = fopen(name, "rb");
    if (f == NULL) {
        fprintf(stderr, "Failed to load ROM file '%s'.\n", name);
        exit(1);
    }
    fread(dest, 16384, 1, f);
    fclose(f);
}

void loadrom_n(int bank, char *name)
{
    loadrom(rombanks[bank], name);
    rombank_enabled[bank] = 1;
}

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
            memset(rombanks[i], 0, 16384);
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
        memcpy(rombanks[0xb], basic, 16384);
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
        fread(cart0,NUM_BANKS * 16384,1,f);
        fclose(f);
}

void loadcart2(const char *filename)
{
        FILE *f=fopen(filename,"rb");
        if (!f) return;
        fread(cart1,NUM_BANKS * 16384,1,f);
        fclose(f);
}

void unloadcart()
{
        memset(cart0,0,NUM_BANKS * 16384);
        memset(cart1,0,NUM_BANKS * 16384);
}

void dumpram()
{
        FILE *f=fopen("ram.dmp","wb");
        fwrite(ram,32768,1,f);
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
                        return keyboard_read(addr);
                }
                /* Treat cartridges specially for now. */
                if (rombank==0) return cart0[(banks[0] * 16384) + (addr&0x3FFF)];
                if (rombank==1) return cart1[(banks[1] * 16384) + (addr&0x3FFF)];

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
        if (addr==0xFC7F && elkConfig.expansion.mrb)
        {
                mrbmapped=!(val&0x80);
                FASTLOW=(elkConfig.expansion.turbo || (elkConfig.expansion.mrb && elkConfig.expansion.mrbmode && mrbmapped));
                FASTHIGH2=(elkConfig.expansion.mrb && elkConfig.expansion.mrbmode==2 && mrbmapped);
                
//                rpclog("Write MRB %02X %i %i %04X\n",val,FASTLOW,FASTHIGH2,pc);
//                if (!val) output=1;
        }
        if (addr==0xFC70 && elkConfig.expansion.plus1) writeadc(val);
        #ifndef WIN32
                if (addr>=0xFC60 && addr<=0xFC6F && elkConfig.expansion.plus1) return writeserial(addr, val);
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

void savememstate(FILE *f)
{
        fwrite(ram,32768,1,f);
        if (elkConfig.expansion.mrb) fwrite(ram2,32768,1,f);
        if (elkConfig.expansion.plus3 && elkConfig.expansion.dfsena) fwrite(dfs,16384,1,f);
        if (elkConfig.sound.sndex)  fwrite(sndrom,16384,1,f);
        if (usedrom6) fwrite(ram6,16384,1,f);
}

void loadmemstate(FILE *f)
{
        fread(ram,32768,1,f);
        if (elkConfig.expansion.mrb) fread(ram2,32768,1,f);
        if (elkConfig.expansion.plus3 && elkConfig.expansion.dfsena) fread(dfs,16384,1,f);
        if (elkConfig.sound.sndex)  fread(sndrom,16384,1,f);
        if (usedrom6) fread(ram6,16384,1,f);
}
