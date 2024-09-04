# Elkulator v1.0

Introduction
============

Elkulator is an Acorn Electron emulator for Windows and Linux.

Elkulator is licensed under the GPL, see COPYING for more details.


# Compiling
## Linux

You will need the following libraries:

Allegro 4.x
OpenAL
ALut
Zlib

On a Debian system you should be able to install these by invoking the
following command in a terminal window:

  sudo apt-get install automake liballegro4-dev zlib1g-dev libalut-dev libopenal-dev aclocal

To configure and build Elkulator, open a terminal window, navigate to the
Elkulator directory then enter

  aclocal -I m4
  automake -a
  autoconf

This should have produced a configure script that can be used to configure
the build process. Then type

  ./configure
  make

If this is successful, typing

  ./elkulator

will run the emulator.

Elkulator has been tested on x86-32 and x86-64 machines. No other architecture
is guaranteed to work, and big-endian machines (eg PowerPC) almost certainly
won't work.

## Windows

NOTE: The following instructions are provided as a guide, but as of September
      2024 I have not been able to successfully build and link elkulator on 
      Windows.

TBD


### Linux specifics

The menu is not available all the time. Press the Menu or F11 keys to open it,
then Menu or F11 to close it again. (Where available, the Menu key is
typically found between the space bar and right Ctrl key, depicting an
application menu on the keycap. It is not a Windows key.)

The debugger is only available via the command line.

Hardware line doubling mode is not available on Linux.

Fullscreen mode doesn't appear to work correctly, at least on my machine.
Elkulator takes over the screen, but the resolution never changes.

Video performance is noticeably slower than on Windows. This is largely due to
the lack of hardware acceleration support in Elkulator.


Features
========

- Emulates basic 32k Electron
- Emulation of all modes + line-by-line effects
- Turbo board and Master RAM Board emulation
- Tape emulation
- Plus 3 emulation
- ROM cartridges
- Emulates sound including sample playback
- CSS Sound Expansion emulation
- Super 2xSaI, Super Eagle, Scale2x and PAL filters


Status
======

6502   - Enough to run all games AFAIK. Cycle accurate.
ULA    - Cycle accurate graphics emulation.
Sound  - Plays back samples correctly.
Tape   - Works with UEF, HQ-UEF and CSW files. Loading is (optionally) accelerated. Read only.
Plus 1 - ADC (joysticks), parallel and some serial support.
Plus 3 - Two drives, ADFS + DFS, Read/Write. FDI support (though no images exist yet).
CSS    - Using SN emulator from B-em.


Menu
====

The menu options are:

File -
    Reset         - hard-resets the Electron
    Exit          - exit to Windows
Tape -
    Load tape   - loads a new UEF tape image file.
    Rewind tape - rewinds tape to beginning.
    Tape speed - choose between normal, fast, and really fast (really fast can break compatibilty)
Disc -
    Load disc 0/2          - load a disc image into drives 0 and 2.
    Load disc 1/3          - load a disc image into drives 1 and 3.
    Eject disc 0/2         - removes disc image from drives 0 and 2.
    Eject disc 1/3         - removes disc image from drives 1 and 3.
    New disc 0/2   	   - creates a new DFS/ADFS disc and loads it into drives 0 and 2.
    New disc 1/3           - creates a new DFS/ADFS disc and loads it into drives 1 and 3.
    Write protect disc 0/2 - toggles write protection on drives 0 and 2.
    Write protect disc 1/3 - toggles write protection on drives 1 and 3.
    Default write protect  - determines whether loaded discs are write protected by default
ROM - 
    Load ROM cartridge 1 - Loads the first ROM cartridge
    Load ROM cartridge 2 - Loads the second ROM cartridge. Some stuff (Starship Command) comes on 2
                           ROMs and both must be loaded in order to work (these would both be in the
			   same cartridge, but have been imaged seperately).
    Unload ROM cartridges - Unload both ROM cartridges
Hardware - 
    Video -
        Display type - Scanlines
		       Line doubling
		       2xSaI
		       Scale2x
		       Super Eagle
		       PAL filter - choose the output mode. Select whichever you prefer.
	Fullscreen - enables fullscreen mode. ALT+ENTER to leave.
	Resizeable window - lets you resize the window at will. Forces Line Doubling on.
    Disc - Plus 3 enable - toggles Plus 3 emulation.
	   ADFS enable   - toggles the presence of the ADFS ROM
	   DFS enable    - toggles the presence of the DFS ROM
    Memory -
	   Elektuur/Slogger turbo board - Emulate a turbo board (see below)
	   Slogger/Jafa Master RAM board - Emulate a master RAM board (see below)
	   Master RAM board mode - Select the mode of operation for the master RAM board (see below)
    Sound -
	   Internal sound output - Enables output of the internal Electron speaker
	   CSS Sound Expansion - Emulate a CSS Sound Expansion cartridge
	   Disc drive noise - Enables authentic disc drive sounds
	   Tape noise       - Enables less authentic tape sounds
	   Disc drive type - Select between 5.25" and 3.5" sounds
	   Disc drive volume - Select volume of disc drive sounds
    Joystick -
	   Plus 1 interface - Enables Plus 1 emulation. Disables First Byte
	   First Byte interface - Enables First Byte emulation. Disables Plus 1 and Plus 3 (due to
 				  conflicts)
    Keyboard -
	   Redefine keyboard - lets you redefine the keyboard
           Restore default layout - restores default keyboard layout
Misc -
    Save screenshot - Saves a screenshot in .BMP format
    Debugger - Enters the debugger
    Break - Breaks into debugger (when debugger enabled)


Turbo boards
============

Elkulator can emulate an Elektuur/Slogger turbo board. This board replaces the lower
8k of RAM in the system with the faster RAM on the board. This has the effect of speeding
up most of the system, as most OS workspace is in this lower 8k, along with the stack.
However, it can have some slight compatibility problems - the Electron ULA can't display 
anything from this area, and some timing sensitive games (eg Firetrack) will mess up.

Elkulator can also emulate a Slogger/Jafa Master RAM board. This is a far more ambitious
afair, attempting to replace all the Electron memory. It has three modes :

Off - Disabled. Same as a normal Electron.

Turbo - Same as the above turbo board - only replaces 8k.

Shadow - As above, but also shadows the screen memory. The screen is left in normal memory,
         and BASIC works with the 'shadow memory' on the board. The computer boasts this as
         '64k mode'. This means that you can use up to 28k in BASIC in all modes, and it runs
         noticeably faster as well. This mode is incompatible with almost all games though.


FAQ
===

Q : How do I run a game?
A : Load the UEF file through the tape menu. Then, at the > prompt, type
    CHAIN"" (for most games. A few will need *RUN).

Q : Why is the 2xSaI filter so slow?
A : The code itself isn't terribly fast to start with, but due to my lazyness,
    the screen rendering code is still in 8-bit, so Elkulator has to perform
    an 8-bit to 16-bit conversion every frame.

Q : Why is the PAL filter even slower?
A : The PAL filter is actually doing a lot of work - processing 3 16mhz data streams.
    It's made worse by my inability to optimise it properly.


Thanks to :
===========

Dave Moore for the website and for testing

Peter Edwards for testing and for feature suggestions

Gary Forrest for hosting

Bob Austin,Astec,Harry Barman,Paul Bond,Allen Boothroyd,Ben Bridgewater,Cambridge,
John Cox,Chris Curry,6502 designers,Jeremy Dion,Tim Dobson,Joe Dunn,Ferranti,
Steve Furber,David Gale,Andrew Gordon,Martyn Gilbert,Lawrence Hardwick,Hermann Hauser,
John Herbert,Hitachi,Andy Hopper,Paul Jephcot,Brian Jones,Chris Jordan,
Computer Laboratory,Tony Mann,Peter Miller,Trevor Morris,Steve Parsons,Robin Pain,
Glyn Phillips,Brian Robertson,Peter Robinson,David Seal,Kim Spence-Jones,Graham Tebby,
Jon Thackray,Topexpress,Chris Turner,Hugo Tyson,John Umney,Alex van Someren,
Geoff Vincent,Adrian Warner,Robin Williamson and Roger Wilson for contributing to the 
development of the Acorn Electron (among others too numerous to mention)


Sarah Walker

b-em@bbcmicro.com
