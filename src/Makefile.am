# Makefile.am for Elkulator

bin_PROGRAMS = elkulator
noinst_SCRIPTS = ../elkulator
CLEANFILES = $(noinst_SCRIPTS)

../elkulator: elkulator
	cp elkulator ..

amrefresh:	


elkulator_CFLAGS = $(allegro_CFLAGS) -Wno-unused-result

# workaround for Win32 Allegro, which has `allegro-config' missing
if OS_WIN
elkulator_LDADD = -lalleg -lz -lopenal -lalut
else
elkulator_LDADD = $(allegro_LIBS) -lz -lopenal -lalut -lz
endif

elkulator_SOURCES = 2xsai.c 1770.c 6502.c adc.c adf.c config.c csw.c ddnoise.c \
debugger.c disc.c fdi2raw.c fdi.c firstbyte.c linux.c linux-gui.c \
linux-keydefine.c main.c mem.c palfilt.c parallel.c savestate.c scale2x.c \
serial.c socket.c sound.c soundopenal.c ssd.c tapenoise.c uef.c ula.c \
allegro_icon.c

