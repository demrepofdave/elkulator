#!/usr/bin/env python

"""
Serial console for use with Elkulator serial port support.

Copyright (C) 2022 Paul Boddie <paul@boddie.org.uk>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from os import read, remove, O_NONBLOCK
from os.path import exists, isfile
from socket import socket, AF_UNIX, SOCK_STREAM
import select, tempfile
import sys

# Conveniences.

class Reader:

    "A simple file-like object for direct stream access."

    def __init__(self, fd):
        self.fd = fd

    def read(self, num):
        return read(self.fd, num)

# Line ending conversion functions.

def cr_to_lf(s):

    """
    Convert output from the Electron having optional line feeds to data
    containing line feeds, replacing carriage returns.
    """

    return s.replace(b"\n", b"").replace(b"\r", b"\n")

def lf_to_cr(s):

    """
    Handle input to the Electron having optional carriage returns to data
    containing carriage returns, replacing line feeds.
    """

    return s.replace(b"\r", b"").replace(b"\n", b"\r")

# Communications functions.

def session(poller, channels):

    "Use 'poller' to monitor the given 'channels'."

    while 1:
        fds = poller.poll()
        for fd, status in fds:
            if status & (select.POLLHUP | select.POLLNVAL | select.POLLERR):
                print("Connection closed.", file=sys.stderr)
                return
            elif status & select.POLLIN:
                reader, writer, converter = channels[fd]
                s = converter(reader.read(1))
                writer.write(s.decode('utf-8'))

def main():

    "Initialise a server socket and open a session."

    # Obtain the communications socket filename and remove any previous socket file.

    try:
        filename = sys.argv[1]
        temporary_file = False
    except IndexError:
        filename = tempfile.mktemp()
        temporary_file = True

    if exists(filename) and not isfile(filename):
        remove(filename)

    # Obtain a socket and bind it to the given filename.

    s = socket(AF_UNIX, SOCK_STREAM)
    s.bind(filename)
    s.listen(0)

    # Accept a connection.

    print("Waiting for connection at:", filename, file=sys.stderr)

    c, addr = s.accept()

    print("Connection accepted.", file=sys.stderr)

    # Employ file-like objects for reading and writing.

    reader = c.makefile("rb", 0)
    writer = c.makefile("wb", 0)

    # Employ polling on the socket input and on standard input.

    poller = select.poll()
    poller.register(reader.fileno(), select.POLLIN | select.POLLHUP | select.POLLNVAL | select.POLLERR)
    poller.register(sys.stdin.fileno(), select.POLLIN | select.POLLHUP | select.POLLNVAL | select.POLLERR)

    # Map input descriptors to input streams, output streams and conversion
    # functions.

    channels = {
        reader.fileno() : (reader, sys.stdout, cr_to_lf),
        sys.stdin.fileno() : (Reader(sys.stdin.fileno()), writer, lf_to_cr)
        }

    # Initiate a session.

    try:
        session(poller, channels)

    # Remove any temporary file.

    finally:
        if temporary_file:
            remove(filename)

# Main program.

if __name__ == "__main__":
    main()

# vim: tabstop=4 expandtab shiftwidth=4
