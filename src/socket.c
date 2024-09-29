/*
 * Socket handling routines for communication with other programs.
 *
 * Copyright (C) 2022 Paul Boddie <paul@boddie.org.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// TODO: Convert code to win32 compatible code.
#ifndef WIN32

#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>



/* A convenience function testing a socket for an input condition and reading
   any input into a buffer of the indicated size. A true value is returned if
   the buffer was filled. Otherwise, a false value is returned. */

int socket_input(int socket_fd, char *buffer, size_t count)
{
        struct pollfd fds[] = {{.fd = socket_fd, .events = POLLIN}};

        if (poll(fds, 1, 0) == -1)
                return 0;

        if (read(socket_fd, buffer, count) == count)
                return 1;

	return 0;
}

/* Open a socket for data exchange. */

int socket_open(const char *filename)
{
        struct sockaddr_un addr;
        int flags;
	int socket_fd;

        if (!strlen(filename))
                return -1;

        socket_fd = socket(AF_UNIX, SOCK_STREAM, PF_UNIX);

        if (socket_fd == -1)
        {
                perror("Failed to open communications socket");
                return -1;
        }

        /* Make the socket non-blocking to be able to receive characters
           concurrently. */

        flags = fcntl(socket_fd, F_GETFL, 0);
        fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

        /* Connect to a named UNIX address. */

        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, filename, sizeof(addr.sun_path) - 1);

        if (connect(socket_fd, (const struct sockaddr *) &addr,
                    sizeof(struct sockaddr_un)) == -1)
        {
                perror("Failed to connect to communications socket");
                return -1;
        }

	return socket_fd;
}

#endif // WIN32