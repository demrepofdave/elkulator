/*Elkulator v1.0 by Sarah Walker
  6502 emulation*/

#include <stdio.h>
#include <stdlib.h>
#include "elk.h"
#include "logger.h"
#include "config_vars.h"

int timetolive;
int ins=0;
int ulacycles;
uint8_t a,x,y,s;
uint16_t pc;
CPUStatus p;

int cycles;
int cyccount=1;
int cpureset=0;
int otherstuffcount=0;

void reset6502()
{
        a=x=y=s=0;
        pc=readmem(0xFFFC)|(readmem(0xFFFD)<<8);
        p.c=p.z=p.d=p.v=p.n=0;
        p.i=1;
        cycles=0;
        mrbmapped=0;
        resetmem();
}

void reset6502e()
{
        cpureset=1;
}

void dumpregs()
{
        dumpram();
        printf("6502 registers :\n");
        printf("A=%02X X=%02X Y=%02X S=%02X PC=%04X\n",a,x,y,s,pc);
        printf("%c%c%c%c%c%c\n",(p.c)?'C':'.',
                                (p.z)?'Z':'.',
                                (p.i)?'I':'.',
                                (p.d)?'D':'.',
                                (p.v)?'V':'.',
                                (p.n)?'N':'.');
        printf("%i ins\n",ins);
}

uint8_t opcode;

void setzn(uint8_t v)
{
        p.z=!v;
        p.n=v&0x80;
}
void setadc(uint8_t v)
{
        uint16_t tempw=a+v+((p.c)?1:0);
        p.z=!(tempw&0xFF);
        p.n=tempw&0x80;
        p.c=tempw&0x100;
        p.v=(!((a^v)&0x80)&&((a^tempw)&0x80));
}
void setsbc(uint8_t temp)
{
        uint16_t tempw=a-(temp+(p.c?0:1));
        int tempv=(short)a-(short)(temp+(p.c?0:1));
        p.v=((a^(temp+(p.c?0:1)))&(a^(uint8_t)tempv)&0x80);
        p.c=tempv>=0;
        setzn(tempw&0xFF);
}

int realnmi,realirq;
void doints()
{
        yield();
        if (nmi) realnmi=1;
        realirq=irq;
        nmi=0;
}

void ADCBCD(uint8_t temp, int tempc)
{
        printf("BCD ADC!\n");
        dumpregs();
        exit(-1);
}

void SBCBCD(uint8_t temp, int tempc)
{
        printf("BCD SBC!\n");
        dumpregs();
        exit(-1);
}

/*ADC/SBC temp variables*/
uint16_t tempw;
int tempv,hc,al,ah;
uint8_t tempb;

#define ADC(temp)       if (!p.d)                            \
                        {                                  \
                                tempw=(a+temp+(p.c?1:0));        \
                                p.v=(!((a^temp)&0x80)&&((a^tempw)&0x80));  \
                                a=tempw&0xFF;                  \
                                p.c=tempw&0x100;                  \
                                setzn(a);                  \
                        }                                  \
                        else                               \
                        {                                  \
                                ah=0;        \
                                tempb=a+temp+(p.c?1:0);                            \
                                if (!tempb)                                      \
                                   p.z=1;                                          \
                                al=(a&0xF)+(temp&0xF)+(p.c?1:0);                            \
                                if (al>9)                                        \
                                {                                                \
                                        al-=10;                                  \
                                        al&=0xF;                                 \
                                        ah=1;                                    \
                                }                                                \
                                ah+=((a>>4)+(temp>>4));                             \
                                if (ah&8) p.n=1;                                   \
                                p.v=(((ah << 4) ^ a) & 128) && !((a ^ temp) & 128);   \
                                p.c=0;                                             \
                                if (ah>9)                                        \
                                {                                                \
                                        p.c=1;                                     \
                                        ah-=10;                                  \
                                        ah&=0xF;                                 \
                                }                                                \
                                a=(al&0xF)|(ah<<4);                              \
                        }

#define SBC(temp)       if (!p.d)                            \
                        {                                  \
                                tempw=a-(temp+(p.c?0:1));    \
                                tempv=(short)a-(short)(temp+(p.c?0:1));            \
                                p.v=((a^(temp+(p.c?0:1)))&(a^(uint8_t)tempv)&0x80); \
                                p.c=tempv>=0;\
                                a=tempw&0xFF;              \
                                setzn(a);                  \
                        }                                  \
                        else                               \
                        {                                  \
                                hc=0;                               \
                                p.z=p.n=0;                            \
                                if (!((a-temp)-((p.c)?0:1)))            \
                                   p.z=1;                             \
                                al=(a&15)-(temp&15)-((p.c)?0:1);      \
                                if (al&16)                           \
                                {                                   \
                                        al-=6;                      \
                                        al&=0xF;                    \
                                        hc=1;                       \
                                }                                   \
                                ah=(a>>4)-(temp>>4);                \
                                if (hc) ah--;                       \
                                if ((a-(temp+((p.c)?0:1)))&0x80)        \
                                   p.n=1;                             \
                                p.v=(((a-(temp+((p.c)?0:1)))^temp)&128)&&((a^temp)&128); \
                                p.c=1; \
                                if (ah&16)                           \
                                {                                   \
                                        p.c=0; \
                                        ah-=6;                      \
                                        ah&=0xF;                    \
                                }                                   \
                                a=(al&0xF)|((ah&0xF)<<4);                 \
                        }

int output=0;
uint16_t oldpc2,oldpc;
void exec6502()
{
        uint16_t addr,addr2;
        uint8_t temp;
        int tempc;
        int oldcycs;
        while (cycles<128)
        {
                oldpc2=oldpc;
                oldpc=pc;
                oldcycs=cycles;
//                if (pc==0x1000) output=1;
//                if (pc>0xE00 && pc<0x5000) output=1;
//                if (pc==0xFFEE && a==65) output=0;
                if (output)
                {
                        rpclog("PC=%04X A=%02X X=%02X Y=%02X S=%02X %i %i %i\n",pc,a,x,y,s,p.i,extrom,rombank);
//                        dumpram();
//                        fflush(stdout);
                }
                opcode=readmem(pc);
                if (debugon) dodebugger();
                pc++;
                cycles+=cyccount;
                switch (opcode)
                {
                        case 0x00: /*BRK*/
                        if (output)
                        {
                                rpclog("BRK!\n");
                                dumpregs();
                        }
                        temp =(p.c)?1:0;    temp|=(p.z)?2:0;
                        temp|=(p.i)?4:0;    temp|=(p.d)?8:0;
                        temp|=(p.v)?0x40:0; temp|=(p.n)?0x80:0;
                        temp|=0x30;
                        pc++;
                        writemem(0x100+s,pc>>8);   s--; cycles+=cyccount;
                        writemem(0x100+s,pc&0xFF); s--; cycles+=cyccount;
                        writemem(0x100+s,temp);    s--; cycles+=cyccount;
                        pc=readmem(0xFFFE);             cycles+=cyccount;
                        pc|=(readmem(0xFFFF)<<8);       cycles+=cyccount;
                        p.i=1;
                        cycles+=cyccount;
                        break;
                        case 0x08: /*PHP*/
                        temp =(p.c)?1:0;    temp|=(p.z)?2:0;
                        temp|=(p.i)?4:0;    temp|=(p.d)?8:0;
                        temp|=(p.v)?0x40:0; temp|=(p.n)?0x80:0;
                        temp|=0x30;
                        readmem(pc);    cycles+=cyccount;
                        doints();
                        writemem(0x100+s,temp); s--; cycles+=cyccount;
//                        printf("It's the PHP\n");
                        break;
                        case 0x0A: /*ASL A*/
                        doints();
                        readmem(pc); cycles+=cyccount;
                        p.c=a&0x80;
                        a<<=1;
                        setzn(a);
                        break;
                        case 0x18: /*CLC*/
                        doints();
                        readmem(pc);
                        p.c=0;
                        cycles+=cyccount;
                        break;
                        case 0x28: /*PLP*/
                        readmem(pc);           cycles+=cyccount;
                        readmem(0x100+s); s++; cycles+=cyccount;
                        doints();
                        temp=readmem(0x100+s);    cycles+=cyccount;
                        p.c=temp&1; p.z=temp&2; p.i=temp&4; p.d=temp&8;
                        p.v=temp&0x40; p.n=temp&0x80;
                        break;
                        case 0x2A: /*ROL A*/
                        doints();
                        readmem(pc);
                        tempc=(p.c)?1:0;
                        p.c=a&0x80;
                        a<<=1;
                        a|=tempc;
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x38: /*SEC*/
                        doints();
                        readmem(pc);
                        p.c=1;
                        cycles+=cyccount;
                        break;
                        case 0x48: /*PHA*/
                        readmem(pc);    cycles+=cyccount;
                        doints();
                        writemem(0x100+s,a); s--; cycles+=cyccount;
                        break;
                        case 0x4A: /*LSR A*/
                        doints();
                        readmem(pc); cycles+=cyccount;
                        p.c=a&1;
                        a>>=1;
                        setzn(a);
                        break;
                        case 0x58: /*CLI*/
                        doints();
                        readmem(pc);
                        p.i=0;
                        cycles+=cyccount;
                        break;
                        case 0x68: /*PLA*/
                        readmem(pc);           cycles+=cyccount;
                        readmem(0x100+s); s++; cycles+=cyccount;
                        doints();
                        a=readmem(0x100+s);    cycles+=cyccount;
                        setzn(a);
                        break;
                        case 0x6A: /*ROR A*/
                        doints();
                        readmem(pc);
                        tempc=(p.c)?0x80:0;
                        p.c=a&1;
                        a>>=1;
                        a|=tempc;
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x78: /*SEI*/
                        doints();
                        readmem(pc);
                        p.i=1;
                        cycles+=cyccount;
                        break;
                        case 0x88: /*DEY*/
                        doints();
                        readmem(pc);
                        y--;
                        setzn(y);
                        cycles+=cyccount;
                        break;
                        case 0x8A: /*TXA*/
                        doints();
                        readmem(pc);
                        a=x;
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x98: /*TYA*/
                        doints();
                        readmem(pc);
                        a=y;
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x9A: /*TXS*/
                        doints();
                        readmem(pc);
                        s=x;
                        cycles+=cyccount;
                        break;
                        case 0xA8: /*TAY*/
                        doints();
                        readmem(pc);
                        y=a;
                        setzn(y);
                        cycles+=cyccount;
                        break;
                        case 0xAA: /*TAX*/
                        doints();
                        readmem(pc);
                        x=a;
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0xB8: /*CLV*/
                        doints();
                        readmem(pc);
                        p.v=0;
                        cycles+=cyccount;
                        break;
                        case 0xBA: /*TSX*/
                        doints();
                        readmem(pc);
                        x=s;
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0xC8: /*INY*/
                        doints();
                        readmem(pc);
                        y++;
                        setzn(y);
                        cycles+=cyccount;
                        break;
                        case 0xCA: /*DEX*/
                        doints();
                        readmem(pc);
                        x--;
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0xD8: /*CLD*/
                        doints();
                        readmem(pc);
                        p.d=0;
                        cycles+=cyccount;
                        break;
                        case 0xE8: /*INX*/
                        doints();
                        readmem(pc);
                        x++;
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0x1A: case 0x3A: case 0x5A: case 0x7A: /*NOP*/
                        case 0xDA: case 0xEA: case 0xFA:
                        doints();
                        readmem(pc);
                        cycles+=cyccount;
                        break;
                        case 0xF8: /*SED*/
                        doints();
                        readmem(pc);
                        p.d=1;
                        cycles+=cyccount;
                        break;

                        case 0x09: /*ORA #*/
                        doints();
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        a|=temp; setzn(a);
                        break;
                        case 0x29: /*AND #*/
                        doints();
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        a&=temp; setzn(a);
                        break;
                        case 0x49: /*EOR #*/
                        doints();
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        a^=temp; setzn(a);
                        break;
                        case 0x69: /*ADC #*/
                        doints();
                        tempc=(p.c)?1:0;
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        ADC(temp);
                        break;
                        case 0x89: /*NOP #*/
                        doints();
                        readmem(pc); pc++; cycles+=cyccount;
                        break;
                        case 0xA0: /*LDY #*/
                        doints();
                        y=readmem(pc); pc++; cycles+=cyccount;
                        setzn(y);
                        break;
                        case 0xA2: /*LDX #*/
                        doints();
                        x=readmem(pc); pc++; cycles+=cyccount;
                        setzn(x);
                        break;
                        case 0xA9: /*LDA #*/
                        doints();
                        a=readmem(pc); pc++; cycles+=cyccount;
                        setzn(a);
                        break;
                        case 0xC0: /*CPY #*/
                        doints();
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        setzn(y-temp);
                        p.c=(y>=temp);
                        break;
                        case 0xC9: /*CMP #*/
                        doints();
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        setzn(a-temp);
                        p.c=(a>=temp);
                        break;
                        case 0xE0: /*CPX #*/
                        doints();
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        setzn(x-temp);
                        p.c=(x>=temp);
                        break;
                        case 0xE9: /*SBC #*/
                        doints();
                        tempc=(p.c)?0:1;
                        temp=readmem(pc); pc++; cycles+=cyccount;
                        SBC(temp);
                        break;

                        case 0x04: case 0x44: case 0x64: /*NOP zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        readmem(addr);
                        cycles+=cyccount;
                        break;
                        case 0x05: /*ORA zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        a|=temp;
                        setzn(a);
                        break;
                        case 0x06: /*ASL zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); p.c=temp&0x80; temp<<=1; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x24: /*BIT zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr);
                        p.z=!(temp&a);
                        p.v=temp&0x40;
                        p.n=temp&0x80;
                        cycles+=cyccount;
                        break;
                        case 0x25: /*AND zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        a&=temp;
                        setzn(a);
                        break;
                        case 0x26: /*ROL zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp);  cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        p.c=temp&0x80;
                        temp<<=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x45: /*EOR zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        a^=temp;
                        setzn(a);
                        break;
                        case 0x46: /*LSR zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); p.c=temp&1; temp>>=1; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x65: /*ADC zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        ADC(temp);
                        break;
                        case 0x66: /*ROR zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        tempc=(p.c)?0x80:0;
                        p.c=temp&1;
                        temp>>=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x84: /*STY zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        writemem(addr,y);
                        cycles+=cyccount;
                        break;
                        case 0x85: /*STA zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        writemem(addr,a);
                        cycles+=cyccount;
                        break;
                        case 0x86: /*STX zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        writemem(addr,x);
                        cycles+=cyccount;
                        break;
                        case 0xA4: /*LDY zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        y=readmem(addr);
                        setzn(y);
                        cycles+=cyccount;
                        break;
                        case 0xA5: /*LDA zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        a=readmem(addr);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0xA6: /*LDX zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        x=readmem(addr);
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0xC4: /*CPY zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        setzn(y-temp);
                        p.c=(y>=temp);
                        break;
                        case 0xC5: /*CMP zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        setzn(a-temp);
                        p.c=(a>=temp);
                        break;
                        case 0xC6: /*DEC zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); temp--; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0xE4: /*CPX zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        setzn(x-temp);
                        p.c=(x>=temp);
                        break;
                        case 0xE5: /*SBC zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        tempc=(p.c)?0:1;
                        SBC(temp);
                        break;
                        case 0xE6: /*INC zp*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); temp++; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;

                        case 0x14: case 0x34: case 0x54: case 0x74: /*NOP zp,x*/
                        case 0xD4: case 0xF4:
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        readmem((addr+x)&0xFF);
                        cycles+=cyccount;
                        break;
                        case 0x15: /*ORA zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        temp=readmem((addr+x)&0xFF); cycles+=cyccount;
                        a|=temp;
                        setzn(a);
                        break;
                        case 0x16: /*ASL zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); p.c=temp&0x80; temp<<=1; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x35: /*AND zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        temp=readmem((addr+x)&0xFF); cycles+=cyccount;
                        a&=temp;
                        setzn(a);
                        break;
                        case 0x36: /*ROL zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        p.c=temp&0x80;
                        temp<<=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x55: /*EOR zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        temp=readmem((addr+x)&0xFF); cycles+=cyccount;
                        a^=temp;
                        setzn(a);
                        break;
                        case 0x56: /*LSR zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); p.c=temp&1; temp>>=1; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x75: /*ADC zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        ADC(temp);
                        break;
                        case 0x76: /*ROR zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        tempc=(p.c)?0x80:0;
                        p.c=temp&1;
                        temp>>=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x94: /*STY zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        writemem((addr+x)&0xFF,y);
                        cycles+=cyccount;
                        break;
                        case 0x95: /*STA zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        writemem((addr+x)&0xFF,a);
                        cycles+=cyccount;
                        break;
                        case 0x96: /*STX zp,y*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        writemem((addr+y)&0xFF,x);
                        cycles+=cyccount;
                        break;
                        case 0xB4: /*LDY zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        y=readmem((addr+x)&0xFF);
                        setzn(y);
                        cycles+=cyccount;
                        break;
                        case 0xB5: /*LDA zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        a=readmem((addr+x)&0xFF);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0xB6: /*LDX zp,y*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount;
                        doints();
                        x=readmem((addr+y)&0xFF);
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0xD5: /*CMP zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        setzn(a-temp);
                        p.c=(a>=temp);
                        break;
                        case 0xD6: /*DEC zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); temp--; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0xF5: /*SBC zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        tempc=(p.c)?0:1;
                        SBC(temp);
                        break;
                        case 0xF6: /*INC zp,x*/
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        readmem(addr); cycles+=cyccount; addr=(addr+x)&0xFF;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); temp++; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;

                        case 0x0C: /*NOP absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        readmem(addr);
                        cycles+=cyccount;
                        break;
                        case 0x0D: /*ORA absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        a|=readmem(addr);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x0E: /*ASL absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        temp=readmem(addr);  cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        p.c=temp&0x80;
                        temp<<=1;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x2C: /*BIT absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr);
                        p.z=!(temp&a);
                        p.v=temp&0x40;
                        p.n=temp&0x80;
                        cycles+=cyccount;
                        break;
                        case 0x2D: /*AND absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        a&=readmem(addr);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x2E: /*ROL absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        temp=readmem(addr);  cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        p.c=temp&0x80;
                        temp<<=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x2F: /*RLA absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        temp=readmem(addr);  cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        p.c=temp&0x80;
                        temp<<=1;
                        temp|=tempc;
                        a&=temp;
                        setzn(a);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x4D: /*EOR absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        a^=readmem(addr);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x4E: /*LSR absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        temp=readmem(addr);  cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        p.c=temp&1;
                        temp>>=1;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x6D: /*ADC abs*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        ADC(temp);
                        break;
                        case 0x6E: /*ROR absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        temp=readmem(addr);  cycles+=cyccount;
                        writemem(addr,temp); cycles+=cyccount;
                        tempc=(p.c)?0x80:0;
                        p.c=temp&1;
                        temp>>=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0x8C: /*STY absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        writemem(addr,y);
                        cycles+=cyccount;
                        break;
                        case 0x8D: /*STA absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        writemem(addr,a);
                        cycles+=cyccount;
                        break;
                        case 0x8E: /*STX absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        writemem(addr,x);
                        cycles+=cyccount;
                        break;
                        case 0xAC: /*LDY absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        y=readmem(addr);
                        setzn(y);
                        cycles+=cyccount;
                        break;
                        case 0xAD: /*LDA absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        a=readmem(addr);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0xAE: /*LDX absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        x=readmem(addr);
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0xCC: /*CPY absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr);
                        setzn(y-temp);
                        p.c=(y>=temp);
                        cycles+=cyccount;
                        break;
                        case 0xCD: /*CMP absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr);
                        setzn(a-temp);
                        p.c=(a>=temp);
                        cycles+=cyccount;
                        break;
                        case 0xCE: /*DEC absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); temp--; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;
                        case 0xEC: /*CPX absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr);
                        setzn(x-temp);
                        p.c=(x>=temp);
                        cycles+=cyccount;
                        break;
                        case 0xED: /*SBC abs*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        doints();
                        temp=readmem(addr); cycles+=cyccount;
                        tempc=(p.c)?0:1;
                        SBC(temp);
                        break;
                        case 0xEE: /*INC absolute*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        temp=readmem(addr); cycles+=cyccount;
                        writemem(addr,temp); temp++; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr,temp); cycles+=cyccount;
                        break;

                        case 0x19: /*ORA absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a|=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x1D: /*ORA absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a|=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x1E: /*ASL absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount; addr2=addr+x;
                        readmem((addr&0xFF00)|(addr2&0xFF)); cycles+=cyccount;
                        temp=readmem(addr2);  cycles+=cyccount;
                        writemem(addr2,temp); cycles+=cyccount;
                        p.c=temp&0x80;
                        temp<<=1;
                        setzn(temp);
                        doints();
                        writemem(addr2,temp); cycles+=cyccount;
                        break;
                        case 0x39: /*AND absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a&=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x3D: /*AND absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a&=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x3E: /*ROL absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount; addr2=addr+x;
                        readmem((addr&0xFF00)|(addr2&0xFF)); cycles+=cyccount;
                        temp=readmem(addr2);  cycles+=cyccount;
                        writemem(addr2,temp); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        p.c=temp&0x80;
                        temp<<=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr2,temp); cycles+=cyccount;
                        break;
                        case 0x59: /*EOR absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a^=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x5D: /*EOR absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a^=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0x5E: /*LSR absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount; addr2=addr+x;
                        readmem((addr&0xFF00)|(addr2&0xFF)); cycles+=cyccount;
                        temp=readmem(addr2);  cycles+=cyccount;
                        writemem(addr2,temp); cycles+=cyccount;
                        p.c=temp&1;
                        temp>>=1;
                        setzn(temp);
                        doints();
                        writemem(addr2,temp); cycles+=cyccount;
                        break;
                        case 0x79: /*ADC abs,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        temp=readmem(addr2); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        ADC(temp);
                        break;
                        case 0x7D: /*ADC abs,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        temp=readmem(addr2); cycles+=cyccount;
                        tempc=(p.c)?1:0;
                        ADC(temp);
                        break;
                        case 0x7E: /*ROR absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount; addr2=addr+x;
                        readmem((addr&0xFF00)|(addr2&0xFF)); cycles+=cyccount;
                        temp=readmem(addr2);  cycles+=cyccount;
                        writemem(addr2,temp); cycles+=cyccount;
                        tempc=(p.c)?0x80:0;
                        p.c=temp&1;
                        temp>>=1;
                        temp|=tempc;
                        setzn(temp);
                        doints();
                        writemem(addr2,temp); cycles+=cyccount;
                        break;
                        case 0x99: /*STA absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        doints();
                        writemem(addr2,a);
                        cycles+=cyccount;
                        break;
                        case 0x9D: /*STA absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        doints();
                        writemem(addr2,a);
                        cycles+=cyccount;
                        break;
                        case 0xB9: /*LDA absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0xBC: /*LDY absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        y=readmem(addr2);
                        setzn(y);
                        cycles+=cyccount;
                        break;
                        case 0xBD: /*LDA absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        a=readmem(addr2);
                        setzn(a);
                        cycles+=cyccount;
                        break;
                        case 0xBE: /*LDX absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        x=readmem(addr2);
                        setzn(x);
                        cycles+=cyccount;
                        break;
                        case 0xD9: /*CMP absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        temp=readmem(addr2);
                        setzn(a-temp);
                        p.c=(a>=temp);
                        cycles+=cyccount;
                        break;
                        case 0xDD: /*CMP absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        temp=readmem(addr2);
                        setzn(a-temp);
                        p.c=(a>=temp);
                        cycles+=cyccount;
                        break;
                        case 0xDE: /*DEC absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount; addr2=addr+x;
                        temp=readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        temp=readmem(addr2); cycles+=cyccount;
                        writemem(addr2,temp); temp--; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr2,temp); cycles+=cyccount;
                        break;
                        case 0xF9: /*SBC absolute,y*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+y;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        temp=readmem(addr2);
                        tempc=(p.c)?0:1;
                        SBC(temp);
                        cycles+=cyccount;
                        break;
                        case 0xFD: /*SBC abs,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        temp=readmem(addr2); cycles+=cyccount;
                        SBC(temp);
                        break;
                        case 0xFE: /*INC absolute,x*/
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount; addr2=addr+x;
                        temp=readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        temp=readmem(addr2); cycles+=cyccount;
                        writemem(addr2,temp); temp++; setzn(temp); cycles+=cyccount;
                        doints();
                        writemem(addr2,temp); cycles+=cyccount;
                        break;
                        case 0x1C: case 0x3C: case 0x5C: case 0x7C: /*NOP absolute,x*/
                        case 0xDC: case 0xFC:
                        addr=readmem(pc);       pc++; cycles+=cyccount;
                        addr|=(readmem(pc)<<8); pc++; cycles+=cyccount;
                        addr2=addr+x;
                        if ((addr^addr2)&0xFF00)
                        {
                                readmem((addr2&0xFF)|(addr&0xFF00)); cycles+=cyccount;
                        }
                        doints();
                        readmem(addr2);
                        cycles+=cyccount;
                        break;


                        case 0x01: /*ORA (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        a|=readmem(addr2); setzn(a); cycles+=cyccount;
                        break;
                        case 0x21: /*AND (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        a&=readmem(addr2); setzn(a); cycles+=cyccount;
                        break;
                        case 0x41: /*EOR (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        a^=readmem(addr2); setzn(a); cycles+=cyccount;
                        break;
                        case 0x61: /*ADC (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        temp=readmem(addr2);
                        tempc=(p.c)?1:0;
                        ADC(temp);
                        break;
                        case 0x81: /*STA (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        writemem(addr2,a); cycles+=cyccount;
                        break;
                        case 0x83: /*SAX (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        writemem(addr2,a&x); cycles+=cyccount;
                        break;
                        case 0xA1: /*LDA (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        a=readmem(addr2); setzn(a); cycles+=cyccount;
                        break;
                        case 0xC1: /*CMP (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        temp=readmem(addr2); setzn(a-temp); p.c=(a>=temp); cycles+=cyccount;
                        break;
                        case 0xE1: /*SBC (,x)*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        readmem(addr);               cycles+=cyccount; addr=(addr+x)&0xFF;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        doints();
                        temp=readmem(addr2);
                        tempc=(p.c)?0:1;
                        SBC(temp);
                        cycles+=cyccount;
                        break;

                        case 0x11: /*ORA (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        a|=readmem(addr2+y); setzn(a); cycles+=cyccount;
                        break;
                        case 0x31: /*AND (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        a&=readmem(addr2+y); setzn(a); cycles+=cyccount;
                        break;
                        case 0x51: /*EOR (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        a^=readmem(addr2+y); setzn(a); cycles+=cyccount;
                        break;
                        case 0x71: /*ADC (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        temp=readmem(addr2+y);
                        tempc=(p.c)?1:0;
                        ADC(temp);
                        break;
                        case 0x91: /*STA (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount;
                        doints();
                        writemem(addr2+y,a); cycles+=cyccount;
                        break;
                        case 0xB1: /*LDA (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        a=readmem(addr2+y); setzn(a); cycles+=cyccount;
                        break;
                        case 0xD1: /*CMP (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        temp=readmem(addr2+y); cycles+=cyccount;
                        setzn(a-temp);
                        p.c=(a>=temp);
                        break;
                        case 0xD3: /*DCP (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        temp=readmem(addr2+y); cycles+=cyccount;
                        writemem(addr2+y,temp); temp--; cycles+=cyccount;
                        doints();
                        writemem(addr2+y,temp); cycles+=cyccount;
                        setzn(a-temp);
                        p.c=(a>=temp);
                        break;
                        case 0xF1: /*SBC (),y*/
                        addr=readmem(pc); pc++;      cycles+=cyccount;
                        addr2=readmem(addr);         cycles+=cyccount;
                        addr2|=(readmem((addr+1)&0xFF)<<8); cycles+=cyccount;
                        if ((addr2^(addr2+y))&0xFF00) { readmem((addr2&0xFF00)+((addr2+y)&0xFF)); cycles+=cyccount; }
                        doints();
                        temp=readmem(addr2+y);
                        tempc=(p.c)?0:1;
                        SBC(temp);
                        cycles+=cyccount;
                        break;

                        case 0x10: /*BPL*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (!p.n)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;
                        case 0x30: /*BMI*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (p.n)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;
                        case 0x50: /*BVC*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (!p.v)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;
                        case 0x70: /*BVS*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (p.v)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;
                        case 0x90: /*BCC*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (!p.c)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;
                        case 0xB0: /*BCS*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (p.c)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;
                        case 0xD0: /*BNE*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (!p.z)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;
                        case 0xF0: /*BEQ*/
                        doints();
                        addr=readmem(pc); pc++; cycles+=cyccount;
                        if (addr&0x80) addr|=0xFF00;
                        if (p.z)
                        {
                                doints();
                                readmem(pc); cycles+=cyccount;
                                addr2=pc+addr;
                                if ((pc^addr2)&0xFF00)
                                {
                                        doints();
                                        readmem((addr2&0xFF)+(pc&0xFF00)); cycles+=cyccount;
                                }
                                pc=addr2;

                        }
                        break;

                        case 0x20: /*JSR*/
                        addr=readmem(pc); pc++;         cycles+=cyccount;
                        readmem(0x100+s);               cycles+=cyccount;
                        writemem(0x100+s,pc>>8);   s--; cycles+=cyccount;
                        writemem(0x100+s,pc&0xFF); s--; cycles+=cyccount;
                        doints();
                        pc=addr|(readmem(pc)<<8);       cycles+=cyccount;
                        break;
                        case 0x40: /*RTI*/
                        readmem(pc);                    cycles+=cyccount;
                        readmem(0x100+s);          s++; cycles+=cyccount;
                        temp=readmem(0x100+s);     s++; cycles+=cyccount;
                        pc=readmem(0x100+s);       s++; cycles+=cyccount;
                        doints();
                        pc|=(readmem(0x100+s)<<8);      cycles+=cyccount;
                        p.c=temp&1; p.z=temp&2; p.i=temp&4; p.d=temp&8;
                        p.v=temp&0x40; p.n=temp&0x80;
                        break;
                        case 0x4C: /*JMP*/
                        addr=readmem(pc);         pc++; cycles+=cyccount;
                        doints();
                        pc=addr|(readmem(pc)<<8);       cycles+=cyccount;
                        break;
                        case 0x60: /*RTS*/
                        readmem(pc);                    cycles+=cyccount;
                        readmem(0x100+s); s++;          cycles+=cyccount;
                        pc=readmem(0x100+s); s++;       cycles+=cyccount;
                        pc|=(readmem(0x100+s)<<8);      cycles+=cyccount;
                        doints();
                        readmem(pc); pc++;              cycles+=cyccount;
                        break;
                        case 0x6C: /*JMP ()*/
                        addr=readmem(pc); pc++;                        cycles+=cyccount;
                        addr|=(readmem(pc)<<8);                        cycles+=cyccount;
                        pc=readmem(addr);                              cycles+=cyccount;
                        doints();
                        pc|=readmem((addr&0xFF00)|((addr+1)&0xFF))<<8; cycles+=cyccount;
                        break;

                        default:
                        switch (opcode&0xF)
                        {
                                case 0xA:
                                break;
                                case 0x0:
                                case 0x2:
                                case 0x3:
                                case 0x4:
                                case 0x7:
                                case 0x9:
                                case 0xB:
                                pc++;
                                break;
                                case 0xC:
                                case 0xE:
                                case 0xF:
                                pc+=2;
                                cycles+=(cyccount*2);
                                break;
                        }
                        cycles+=(cyccount*2);
                        doints();
                        break;
/*                        pc--;
                        dumpregs();
                        rpclog("Bad 6502 opcode %02X %04X %04X %i %i\n",opcode,oldpc,oldpc2,extrom,rombank);
                        fflush(stdout);
                        exit(-1);*/
                }
//                if (output) rpclog("%04X : %02X %02X %02X %02X\n",pc,a,x,y,s);
//                if (pc==0x1000) rpclog("Loop!\n");
//                if (pc==0xFFEE)
//                {
//                        if ((a > 31 && a < 127) || a == 0x0a || a == 0x0d)
//                        {
//                                log_debug("%c",a);
//                        output=1;
//                        }
//                }
                yield();
                if (realnmi)
                {
                        realnmi=0;
                        temp =(p.c)?1:0;    temp|=(p.z)?2:0;
                        temp|=(p.i)?4:0;    temp|=(p.d)?8:0;
                        temp|=(p.v)?0x40:0; temp|=(p.n)?0x80:0;
                        temp|=0x30;
                        writemem(0x100+s,pc>>8);   s--; cycles+=cyccount;
                        writemem(0x100+s,pc&0xFF); s--; cycles+=cyccount;
                        writemem(0x100+s,temp);    s--; cycles+=cyccount;
                        pc=readmem(0xFFFA);             cycles+=cyccount;
                        pc|=(readmem(0xFFFB)<<8);       cycles+=cyccount;
                        p.i=1;
                        cycles+=cyccount; cycles+=cyccount;
                        yield();
//                        printf("Taking NMI!\n");
                }
                else if (realirq && !p.i)
                {
                        temp =(p.c)?1:0;    temp|=(p.z)?2:0;
                        temp|=(p.i)?4:0;    temp|=(p.d)?8:0;
                        temp|=(p.v)?0x40:0; temp|=(p.n)?0x80:0;
                        temp|=0x20;
                        writemem(0x100+s,pc>>8);   s--; cycles+=cyccount;
                        writemem(0x100+s,pc&0xFF); s--; cycles+=cyccount;
                        writemem(0x100+s,temp);    s--; cycles+=cyccount;
                        pc=readmem(0xFFFE);             cycles+=cyccount;
                        pc|=(readmem(0xFFFF)<<8);       cycles+=cyccount;
                        p.i=1;
                        cycles+=cyccount; cycles+=cyccount;
                        yield();
//                        printf("Taking int!\n");
                }
                ins++;
                oldcycs=cycles-oldcycs;
                if (cpureset)
                {
                        cpureset=0;
                        reset6502();
                }
                      if (motoron)
                      {
                                if (fdctime) { fdctime-=oldcycs; if (fdctime<=0) fdccallback(); }
                                disctime-=oldcycs; if (disctime<=0) { disctime+=discspd; disc_poll(); }
                      }
                otherstuffcount-=oldcycs;
                if (otherstuffcount<0)
                {
                        otherstuffcount+=128;
                        if (motorspin)
                        {
                                motorspin--;
                                if (!motorspin) fdcspindown();
                        }

                        if (elkConfig.expansion.plus1 && adctime)
                        {
                                adctime-=128;
                                if (adctime<=0)
                                {
                                        adctime=0;
                                        plus1stat&=~0x40;
                                }
                        }
                        #ifndef WIN32
                                if (elkConfig.expansion.plus1) pollserial(oldcycs);
                        #endif // WIN32
                }
                        if (elkConfig.tape.speed == TAPE_REALLY_FAST)
                        {
                                if (pc==0xF699 || pc==0xF7E5 || (pc==0xfa51 && p.z))
                                {
                                        reallyfasttapepoll();
//                                        justrealfasttaped=1;
                                }
                        }
//                        else
//                        {
//                if (tapewrite)
//                {
                        tapewrite-=oldcycs;
                        if (tapewrite<=0)
                        {
//                                tapewrite+=1628;
                                //tapenextbyte();
                                polltape();
                        }
//                        }
//                }
//                if (pc==0xF905) printf("F905 from %04X %04X %i\n",oldpc,oldpc2,p.i);
                if (timetolive)
                {
                        timetolive--;
                        if (!timetolive) output=0;
                }
        }
        cycles-=128;
        ulacycles-=128;
        #if 0
                if (tapeon)
                {
                        if (tapelcount<=0)
                        {
/*                                if (justrealfasttaped)
                                {
                                        tapelcount=tapellatch;
                                        justrealfasttaped=0;
                                }
                                else
                                {*/
                                        polltape();
                                        tapelcount+=tapellatch;
//                                }
                        }
                        else
                           tapelcount-=96;
                }
        #endif
}

void save6502state(FILE *f)
{
        uint8_t temp;
        putc(a,f); putc(x,f); putc(y,f);
        temp =(p.c)?1:0;    temp|=(p.z)?2:0;
        temp|=(p.i)?4:0;    temp|=(p.d)?8:0;
        temp|=(p.v)?0x40:0; temp|=(p.n)?0x80:0;
        temp|=0x30;
        putc(temp,f);
        putc(s,f); putc(pc&0xFF,f); putc(pc>>8,f);
        putc(nmi,f); putc(irq,f);
        putc(cycles,f); putc(cycles>>8,f); putc(cycles>>16,f); putc(cycles>>24,f);
}

void load6502state(FILE *f)
{
        uint8_t temp;
        a=getc(f); x=getc(f); y=getc(f);
        temp=getc(f);
        p.c=temp&0x01; p.z=temp&0x02;
        p.i=temp&0x04; p.d=temp&0x08;
        p.v=temp&0x40; p.n=temp&0x80;
        s=getc(f); pc=getc(f); pc|=(getc(f)<<8);
        nmi=getc(f); irq=getc(f);
        cycles=getc(f); cycles|=(getc(f)<<8); cycles|=(getc(f)<<16); cycles|=(getc(f)<<24);
}
