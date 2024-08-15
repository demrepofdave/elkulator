# SUBDIRS = src src/hal/hal_allegro_4

# Makefile.am for Elkulator

bin_PROGRAMS = elkulator
noinst_SCRIPTS = ../elkulator
CLEANFILES = $(noinst_SCRIPTS)

# ../elkulator: elkulator

amrefresh:	


elkulator_CFLAGS = $(allegro_CFLAGS) -I ./src/

# workaround for Win32 Allegro, which has `allegro-config' missing
if OS_WIN
elkulator_LDADD = -lalleg -lz -lopenal -lalut
else
elkulator_LDADD = $(allegro_LIBS) -lz -lopenal -lalut -lz
endif

elkulator_SOURCES = src/1770.c \
                    src/6502.c \
                    src/adc.c  \
                    src/adf.c  \
                    src/config.c  \
                    src/csw.c  \
                    src/ddnoise.c \
                    src/debugger.c \
                    src/disc.c \
                    src/fdi2raw.c \
                    src/fdi.c \
                    src/firstbyte.c \
                    src/linux.c \
                    src/linux-gui.c \
                    src/linux-keydefine.c \
                    src/main.c \
                    src/mem.c \
                    src/parallel.c \
                    src/savestate.c \
                    src/serial.c \
                    src/socket.c \
                    src/sound.c \
                    src/soundopenal.c \
                    src/ssd.c \
                    src/tapenoise.c \
                    src/uef.c \
                    src/ula.c \
                    src/allegro_icon.c \
                    src/hal/hal_allegro_4/2xsai.c \
                    src/hal/hal_allegro_4/palfilt.c \
                    src/hal/hal_allegro_4/scale2x.c \
                    src/hal/hal_allegro_4/hal.c \
                    src/hal/hal_allegro_4/hal_sound.c
