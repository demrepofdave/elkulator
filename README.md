# Elkulator v2.0 for Windows and Linux.

Introduction
============

Elkulator is an Acorn Electron emulator for Windows and Linux.  It was originally
written by Sarah Walker and has many community contirbutions since (see below).

Elkulator is licensed under the GPL, see COPYING for more details.

# Compiling

With version 2, Elkulator now compiles against the current Allegro5 libraries.  This
replaces the much older allegro4 and allows easier multi platform support.

## Windows

To build windows you will first need to install MSYS2 (TODO: Instructions for MSYS2 install).

Then open a MSYS2 command window and you will need to install the following:

```
pacman -S git mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-allegro make libtool automake autoconf mingw-w64-ucrt-x86_64-freealut
```
To configure and build Elkulator, open a terminal window, navigate to the
Elkulator directory then enter

```
  ./scripts/autoconfigure.sh
```

To build enter: 

```
  make
```

If this is successful then you should be able to run the emulator:

```
  ./elkulator.exe
```
 
## Linux

Elkulator can still be compiled against allegro4 for reference, allthough this will
be deprecated soon.

You will need the following libraries:

- Allegro 5.x
- OpenAL
- ALut
- Zlib

On a Debian system you should be able to install these by invoking the
following command in a terminal window:

```
  sudo apt-get update
  sudo apt-get install automake liballegro5-dev zlib1g-dev libalut-dev libopenal-dev 
```

To configure and build Elkulator, open a terminal window, navigate to the
Elkulator directory then enter

```
  ./scripts/autoconfigure.sh
```

To build enter: 

```
  make
```

If this is successful then you should be able to run the emulator:

```
  ./elkulator
```

## Linux (Legacy Allegro4 build)

Elkulator can still be compiled against allegro4 for reference, allthough this will
be deprecated soon.

You will need the following libraries:

- Allegro 4.x
- OpenAL
- ALut
- Zlib

On a Debian system you should be able to install these by invoking the
following command in a terminal window:

```
  sudo apt-get update
  sudo apt-get install automake liballegro4-dev zlib1g-dev libalut-dev libopenal-dev aclocal
```

To configure and build Elkulator, open a terminal window, navigate to the
Elkulator directory then enter

```
  ./scripts/autoconfigure.sk --enable-allegro4
```

To build enter: 

```
  make
```

If this is successful then you should be able to run the emulator:

```
  ./elkulator
```


Elkulator has been tested on x86-32 and x86-64 machines. No other architecture
is guaranteed to work, and big-endian machines (eg PowerPC) almost certainly
won't work.


## Linux (Legacy) Specifics

Due to the limitations of the allegro4 libraries on linus, the menu is not
available all the time. Press the Menu or F11 keys to open it,
then Menu or F11 to close it again. (Where available, the Menu key is
typically found between the space bar and right Ctrl key, depicting an
application menu on the keycap. It is not a Windows key).

The debugger is only available via the command line.

Hardware line doubling mode is not available on Linux.

Risizing of the Elkulator Window is not available, and full screen mode
does not expand the size of the actual Electron screen.

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

- 6502   - Enough to run all games AFAIK. Cycle accurate.
- ULA    - Cycle accurate graphics emulation.
- Sound  - Plays back samples correctly.
- Tape   - Works with UEF, HQ-UEF and CSW files. Loading is (optionally) accelerated. Read only.
- Plus 1 - ADC (joysticks), parallel and some serial support.
- Plus 3 - Two drives, ADFS + DFS, Read/Write. FDI support (though no images exist yet).
- CSS    - Using SN emulator from B-em.


Menu
====

The menu options are:

- File
  - Reset         - hard-resets the Electron
  - Exit          - exit to Windows
- Tape
  - Load tape   - loads a new UEF tape image file.
  - Rewind tape - rewinds tape to beginning.
  - Tape speed - choose between normal, fast, and really fast (really fast can break compatibilty)
- Disc
  - Load disc 0/2          - load a disc image into drives 0 and 2.
  - Load disc 1/3          - load a disc image into drives 1 and 3.
  - Eject disc 0/2         - removes disc image from drives 0 and 2.
  - Eject disc 1/3         - removes disc image from drives 1 and 3.
  - New disc 0/2   	       - creates a new DFS/ADFS disc and loads it into drives 0 and 2.
  - New disc 1/3           - creates a new DFS/ADFS disc and loads it into drives 1 and 3.
  - Write protect disc 0/2 - toggles write protection on drives 0 and 2.
  - Write protect disc 1/3 - toggles write protection on drives 1 and 3.
  - Default write protect  - determines whether loaded discs are write protected by default
- ROM
  - Load ROM cartridge 1 - Loads the first ROM cartridge
  - Load ROM cartridge 2 - Loads the second ROM cartridge. Some stuff (Starship Command) comes on 2
                           ROMs and both must be loaded in order to work (these would both be in the
			   same cartridge, but have been imaged seperately).
  - Unload ROM cartridges - Unload both ROM cartridges
- Hardware
  - Video
    - Display type - Scanlines
	  - Line doubling
	  - 2xSaI
	  - Scale2x
	  - Super Eagle
	  - PAL filter - choose the output mode. Select whichever you prefer.
	- Fullscreen - enables fullscreen mode. ALT+ENTER to leave.
	- Resizeable window - lets you resize the window at will. Forces Line Doubling on.
    - Disc - Plus 3 enable - toggles Plus 3 emulation.
	  - ADFS enable   - toggles the presence of the ADFS ROM
	  - DFS enable    - toggles the presence of the DFS ROM
    - Memory
	  - Elektuur/Slogger turbo board - Emulate a turbo board (see below)
	  - Slogger/Jafa Master RAM board - Emulate a master RAM board (see below)
	  - Master RAM board mode - Select the mode of operation for the master RAM board (see below)
    - Sound
	  - Internal sound output - Enables output of the internal Electron speaker
	  - CSS Sound Expansion - Emulate a CSS Sound Expansion cartridge
	  - Disc drive noise - Enables authentic disc drive sounds
	  - Tape noise       - Enables less authentic tape sounds
	  - Disc drive type - Select between 5.25" and 3.5" sounds
	  - Disc drive volume - Select volume of disc drive sounds
    - Joystick
	  - Plus 1 interface - Enables Plus 1 emulation. Disables First Byte
	  - First Byte interface - Enables First Byte emulation. Disables Plus 1 and Plus 3 (due to
 				  conflicts)
    - Keyboard
	  - Redefine keyboard - lets you redefine the keyboard
        - Restore default layout - restores default keyboard layout
- Misc
    - Save screenshot - Saves a screenshot in .BMP format
    - Debugger - Enters the debugger
    - Break - Breaks into debugger (when debugger enabled)


Turbo boards
============

Elkulator can emulate an Elektuur/Slogger turbo board. This board replaces the lower
8k of RAM in the system with the faster RAM on the board. This has the effect of speeding
up most of the system, as most OS workspace is in this lower 8k, along with the stack.
However, it can have some slight compatibility problems - the Electron ULA can't display 
anything from this area, and some timing sensitive games (eg Firetrack) will mess up.

Elkulator can also emulate a Slogger/Jafa Master RAM board. This is a far more ambitious
afair, attempting to replace all the Electron memory. It has three modes :

- Off - Disabled. Same as a normal Electron.
- Turbo - Same as the above turbo board - only replaces 8k.
- Shadow - As above, but also shadows the screen memory. The screen is left in normal memory,
         and BASIC works with the 'shadow memory' on the board. The computer boasts this as
         '64k mode'. This means that you can use up to 28k in BASIC in all modes, and it runs
         noticeably faster as well. This mode is incompatible with almost all games though.



# Credits

Sarah Walker (b-em@bbcmicro.com)
+ Many others (TODO: Add)
