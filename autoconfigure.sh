#/bin/sh

# Run automake/autoconf tools + create automake links.
aclocal -I m4
automake -a
autoconf

# Run the project configuration script produced by autoconf.
./configure