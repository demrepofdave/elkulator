Elkulator for Linux
~~~~~~~~~~~~~~~~~~~

Compiling
~~~~~~~~~

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


Linux specifics
~~~~~~~~~~~~~~~

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


Sarah Walker
b-em@bbcmicro.com
