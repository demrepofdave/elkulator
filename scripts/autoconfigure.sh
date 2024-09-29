#/bin/sh

# Run automake/autoconf tools + create automake links.
echo "Running aclocal"
echo "---------------"

aclocal -I m4

echo
echo "Running automake"
echo "----------------"

automake -a

echo
echo "Running autoconf"
echo "----------------"

autoconf

# Run the project configuration script produced by autoconf.
echo "Running configure ${1}"
echo "-----------------"

./configure $1

# End of file.