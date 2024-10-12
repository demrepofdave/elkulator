/*Elkulator v1.0 by Sarah Walker
Savestate handling*/
#include <stdio.h>
#include "elk.h"
#include "6502.h"
#include "mem.h"
#include "ula.h"
#include "config_vars.h"

void savestate()
{
        wantsavestate=1;
}

void loadstate()
{
        wantloadstate=1;
}

void dosavestate(const char * filename)
{
        FILE *f=fopen(filename,"wb");
        putc('E',f); putc('L',f); putc('K',f); putc('S',f);
        putc('N',f); putc('A',f); putc('P',f); putc('1',f);

        putc(elkConfig.expansion.turbo,f);
        putc(elkConfig.expansion.mrb,f);
        putc(elkConfig.expansion.mrbmode,f);
        putc(usedrom6,f);
        
        putc(elkConfig.expansion.plus3,f);
        putc(elkConfig.expansion.adfsena,f);
        putc(elkConfig.expansion.dfsena,f);
        putc(0,f);
        
        putc(elkConfig.sound.sndex,f);
        putc(elkConfig.expansion.plus1,f);
        putc(elkConfig.expansion.firstbyte,f); putc(0,f);
        
        save6502state(f);
        saveulastate(f);
        savememstate(f);
        
        fclose(f);
        
        wantsavestate=0;
}
        
void doloadstate(const char * filename)
{
        int c;
        FILE *f=fopen(filename,"rb");
        for (c=0;c<8;c++) getc(f);
        
        elkConfig.expansion.turbo   = getc(f);
        elkConfig.expansion.mrb     = getc(f);
        elkConfig.expansion.mrbmode = getc(f);
        usedrom6=getc(f);
        
        elkConfig.expansion.plus3   = getc(f);
        elkConfig.expansion.adfsena = getc(f);
        elkConfig.expansion.dfsena  = getc(f);
        getc(f);
        
        elkConfig.sound.sndex=getc(f);
        elkConfig.expansion.plus1=getc(f);
        elkConfig.expansion.firstbyte=getc(f);
        getc(f);
        
        load6502state(f);
        loadulastate(f);
        loadmemstate(f);
        
        fclose(f);
        
        wantloadstate=0;
}
