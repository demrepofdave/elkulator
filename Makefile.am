#SUBDIRS = src

# SUBDIRS = src src/allegro_4

# Makefile.am for Elkulator

bin_PROGRAMS = elkulator
noinst_SCRIPTS = elkulator
CLEANFILES = $(noinst_SCRIPTS)

# ../elkulator: elkulator

amrefresh:	


elkulator_CFLAGS = $(allegro_CFLAGS) -I ./src/ -Wno-unused-result

# workaround for Win32 Allegro, which has `allegro-config' missing
if OS_WIN
elkulator_LDADD = -lalleg -lz -lopenal -lalut
else
elkulator_LDADD = $(allegro_LIBS) -lz -lopenal -lalut -lz
endif

if ENABLE_ALLEGRO4
elkulator_hal_src = src/allegro_4/2xsai.c \
                    src/allegro_4/adc.c  \
                    src/allegro_4/palfilt.c \
                    src/allegro_4/scale2x.c \
                    src/allegro_4/soundopenal.c \
                    src/allegro_4/firstbyte.c \
                    src/allegro_4/linux-gui.c \
                    src/allegro_4/linux-keydefine.c \
                    src/allegro_4/video.c \
                    src/allegro_4/keyboard.c \
                    src/allegro_4/samples.c \
                    src/allegro_4/fileutils.c \
                    src/allegro_4/allegro_icon.c
else
elkulator_hal_src = src/allegro_5/adc.c  \
                    src/allegro_5/event_handler.c \
                    src/allegro_5/firstbyte.c \
                    src/allegro_5/fileutils.c \
                    src/allegro_5/keyboard.c \
                    src/allegro_5/linux-keydefine.c \
                    src/allegro_5/menus.c \
                    src/allegro_5/menu_file.c \
                    src/allegro_5/menu_tape.c \
                    src/allegro_5/menu_disc.c \
                    src/allegro_5/menu_roms.c \                    
                    src/allegro_5/menu_settings.c \
                    src/allegro_5/menu_misc.c \                    
                    src/allegro_5/palfilt.c \
                    src/allegro_5/samples.c \
                    src/allegro_5/soundopenal.c \
                    src/allegro_5/video.c
endif

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
                    $(elkulator_hal_src) 


