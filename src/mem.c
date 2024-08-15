/*Elkulator v1.0 by Sarah Walker
  Memory handling*/
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elk.h"

static const char * roms = "roms";   // Name of directory containing rom files

int FASTLOW=0;
int FASTHIGH2=0;
//#define FASTLOW (turbo || (mrb && mrbmode && mrbmapped))
#define FASTHIGH (FASTHIGH2 && ((pc&0xE000)!=0xC000))

extern int output;
int mrbmapped=0;
int plus1=0;
uint8_t readkeys(uint16_t addr);
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
    rombank_enabled[PLUS1_BANK] = plus1;
    rombank_enabled[SOUND_BANK] = sndex;
    rombank_enabled[ADFS_BANK] = (plus3 && adfsena);
    rombank_enabled[DFS_BANK] = (plus3 && dfsena);
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

void loadcart(char *fn)
{
        FILE *f=fopen(fn,"rb");
        if (!f) return;
        fread(cart0,NUM_BANKS * 16384,1,f);
        fclose(f);
}

void loadcart2(char *fn)
{
        FILE *f=fopen(fn,"rb");
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

        FASTLOW=turbo || (mrb && mrbmode);
        FASTHIGH2=(mrb && mrbmode==2);
        banks[0] = 0;
        banks[1] = 0;
        if (enable_mgc) {
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
            if ((addr&0xFFF8)==0xFCC0 && plus3) return read1770(addr);
            if (addr==0xFCC0 && firstbyte) return readfirstbyte();
            if (plus1)
            {
                    if (addr==0xFC70) return readadc();
                    if (addr==0xFC72) return getplus1stat();
            }
            if (addr>=0xFC60 && addr<=0xFC6F && plus1) return readserial(addr);
//          if ((addr&~0x1F)==0xFC20) return readsid(addr);

            /* Allow the JIM paging register to be read if enabled directly or
               indirectly. */
            if (enable_jim && (addr == 0xfcff))
                return jim_page;

            return addr>>8;
        }
        case 0xFD00: /* Paged RAM exposed in page FD */
        {
            if (enable_jim) {
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
        if (mrb) return mrbos[addr&0x3FFF];
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
                if (extrom && rombank==DFS_BANK && plus3 && dfsena) dfs[addr&0x3FFF]=val;
                if (extrom && rombank==0x6) { ram6[addr&0x3FFF]=val; usedrom6=1; }
        }
        switch (addr & 0xFF00) {
        case 0xFE00:
            writeula(addr,val);
            break;
        case 0xFD00:    /* Paged RAM exposed in page FD */
            //fprintf(stdout, "FD: (%02x) %04x %02x %02x\n", jim_page, addr, jim_ram[jim_page][addr & 0xff], val);
            if (enable_jim)
                jim_ram[jim_page & 0x7f][addr & 0xff] = val;
            break;
        default:
            break;
        }
        if ((addr&0xFFF8)==0xFCC0 && plus3) write1770(addr,val);
//        if ((addr&~0x1F)==0xFC20) writesid(addr,val);

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
        if (addr==0xFC7F && mrb)
        {
                mrbmapped=!(val&0x80);
                FASTLOW=(turbo || (mrb && mrbmode && mrbmapped));
                FASTHIGH2=(mrb && mrbmode==2 && mrbmapped);
                
//                rpclog("Write MRB %02X %i %i %04X\n",val,FASTLOW,FASTHIGH2,pc);
//                if (!val) output=1;
        }
        if (addr==0xFC70 && plus1) writeadc(val);
        if (addr>=0xFC60 && addr<=0xFC6F && plus1) return writeserial(addr, val);
        if (addr==0xFC71 && plus1) writeparallel(val);
        /* The Mega Games Cartridge uses FC00 to select pairs of 16K banks in
           the two sets of ROMs. */
        if (enable_mgc && (addr == 0xfc00)) {
            fprintf(stderr, "bank = %i\n", val); banks[0] = val; banks[1] = val;
        }
        /* DB: My cartridge uses FC73 to select 32K regions in a flash ROM.
           For convenience we use the same paired 16K ROM arrangement as for
           the MGC. */
        if (enable_db_flash_cartridge && (addr == 0xfc73)) {
            fprintf(stderr, "bank = %i\n", val); banks[0] = val; banks[1] = val;
        }
        /* Support the JIM paging register when enabled directly or indirectly. */
        if (enable_jim && (addr == 0xfcff)) {
            jim_page = val;
            //fprintf(stdout, "JIM: %02x pc=%04x (&f4)=%02x\n", jim_page, pc, ram[0xf4]);
        }
}

int keys[2][14][4]=
{
        {
                {KEY_RIGHT,KEY_END,0,KEY_SPACE},
                {KEY_LEFT,KEY_DOWN,KEY_ENTER,KEY_DEL},
                {KEY_MINUS,KEY_UP,KEY_QUOTE,0},
                {KEY_0,KEY_P,KEY_COLON,KEY_SLASH},
                {KEY_9,KEY_O,KEY_L,KEY_STOP},
                {KEY_8,KEY_I,KEY_K,KEY_COMMA},
                {KEY_7,KEY_U,KEY_J,KEY_M},
                {KEY_6,KEY_Y,KEY_H,KEY_N},
                {KEY_5,KEY_T,KEY_G,KEY_B},
                {KEY_4,KEY_R,KEY_F,KEY_V},
                {KEY_3,KEY_E,KEY_D,KEY_C},
                {KEY_2,KEY_W,KEY_S,KEY_X},
                {KEY_1,KEY_Q,KEY_A,KEY_Z},
                {KEY_ESC,KEY_ALT,KEY_LCONTROL,KEY_LSHIFT}
        },
        {
                {KEY_RIGHT,KEY_END,0,KEY_SPACE},
                {KEY_LEFT,KEY_DOWN,KEY_ENTER,KEY_BACKSPACE},
                {KEY_MINUS,KEY_UP,KEY_QUOTE,0},
                {KEY_0,KEY_P,KEY_SEMICOLON,KEY_SLASH},
                {KEY_9,KEY_O,KEY_L,KEY_STOP},
                {KEY_8,KEY_I,KEY_K,KEY_COMMA},
                {KEY_7,KEY_U,KEY_J,KEY_M},
                {KEY_6,KEY_Y,KEY_H,KEY_N},
                {KEY_5,KEY_T,KEY_G,KEY_B},
                {KEY_4,KEY_R,KEY_F,KEY_V},
                {KEY_3,KEY_E,KEY_D,KEY_C},
                {KEY_2,KEY_W,KEY_S,KEY_X},
                {KEY_1,KEY_Q,KEY_A,KEY_Z},
                {KEY_ESC,KEY_TAB,KEY_RCONTROL,KEY_RSHIFT}
        }
};

int keyl[128];

void makekeyl()
{
        int c,d,e;
        memset(keyl,0,sizeof(keyl));

        /* Establish a mapping from emulated key presses to keyboard matrix values. */

        for (c=0;c<14;c++)
        {
                for (d=0;d<4;d++)
                {
                        for (e=0;e<2;e++)
                        {
                                keyl[keys[e][c][d]]=c|(d<<4)|0x80;
                        }
                }
        }

        /* Record the Break keys separately, along with configurable menu keys. */

        update_break_keys();
        update_menu_keys();
}

uint8_t readkeys(uint16_t addr)
{
        int d;
        uint8_t temp=0;
        for (d=0;d<128;d++)
        {
                if (key[d] && keyl[keylookup[d]]&0x80 && !(addr&(1<<(keyl[keylookup[d]]&15)))) temp|=1<<((keyl[keylookup[d]]&0x30)>>4);
//                if (autoboot && (d==KEY_LSHIFT || d==KEY_RSHIFT) && !(addr&(1<<(keyl[d]&15)))) temp|=1<<((keyl[keylookup[d]]&0x30)>>4);
        }
  //      if (autoboot && !(addr&(1<<(keyl[KEY_LSHIFT]&15)))) temp|=1<<((keyl[keylookup[KEY_LSHIFT]]&0x30)>>4);
//        rpclog("Readkey %i %04X %02X %02X %04X\n",autoboot,addr,temp,keyl[KEY_LSHIFT],1<<(keyl[KEY_LSHIFT]&15));
/*        for (c=0;c<14;c++)
        {
                if (!(addr&(1<<c)))
                {
                        if (key[keylookup[keys[0][c][0]]]) temp|=1;
                        if (key[keylookup[keys[0][c][1]]]) temp|=2;
                        if (key[keylookup[keys[0][c][2]]]) temp|=4;
                        if (key[keylookup[keys[0][c][3]]]) temp|=8;
                        if (key[keylookup[keys[1][c][0]]]) temp|=1;
                        if (key[keylookup[keys[1][c][1]]]) temp|=2;
                        if (key[keylookup[keys[1][c][2]]]) temp|=4;
                        if (key[keylookup[keys[1][c][3]]]) temp|=8;
                }
        }*/
        return temp;
}

void savememstate(FILE *f)
{
        fwrite(ram,32768,1,f);
        if (mrb) fwrite(ram2,32768,1,f);
        if (plus3 && dfsena) fwrite(dfs,16384,1,f);
        if (sndex)  fwrite(sndrom,16384,1,f);
        if (usedrom6) fwrite(ram6,16384,1,f);
}

void loadmemstate(FILE *f)
{
        fread(ram,32768,1,f);
        if (mrb) fread(ram2,32768,1,f);
        if (plus3 && dfsena) fread(dfs,16384,1,f);
        if (sndex)  fread(sndrom,16384,1,f);
        if (usedrom6) fread(ram6,16384,1,f);
}
