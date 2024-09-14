# SUBDIRS = src src/hal/hal_allegro_4

# Makefile.am for Elkulator

bin_PROGRAMS = elkulator
noinst_SCRIPTS = elkulator
CLEANFILES = $(noinst_SCRIPTS)

# ../elkulator: elkulator

amrefresh:	


elkulator_CFLAGS = $(allegro_CFLAGS) -I ./src/ -Wno-unused-result

# workaround for Win32 Allegro, which has `allegro-config' missing
if OS_WIN
elkulator_LDADD = -lallegro_audio -lallegro_acodec -lallegro_primitives -lallegro_dialog -lallegro_image -lallegro_font -lallegro -lz -lm
else
elkulator_LDADD = -lallegro_audio -lallegro_acodec -lallegro_primitives -lallegro_dialog -lallegro_image -lallegro_font -lallegro_main -lallegro -lz -lm -lpthread
endif

elkulator_hal_src = src/hal/hal_allegro_5/adc.c  \
                    src/hal/hal_allegro_5/soundopenal.c \
                    src/hal/hal_allegro_5/firstbyte.c \
                    src/hal/hal_allegro_5/hal.c \
                    src/hal/hal_allegro_5/hal_sound.c \
                    src/allegro5/vidalleg.c

elkulator_SOURCES = src/1770.c \
                    src/6502.c \
                    src/adf.c  \
                    src/config.c  \
                    src/csw.c  \
                    src/ddnoise.c \
                    src/debugger.c \
                    src/disc.c \
                    src/fdi2raw.c \
                    src/fdi.c \
                    src/linux.c \
                    src/main.c \
                    src/mem.c \
                    src/parallel.c \
                    src/savestate.c \
                    src/serial.c \
                    src/socket.c \
                    src/sound.c \
                    src/ssd.c \
                    src/tapenoise.c \
                    src/uef.c \
                    src/ula.c \
                    src/allegro_icon.c \
                    src/gui-allegro5.c \
                    $(elkulator_hal_src) 


