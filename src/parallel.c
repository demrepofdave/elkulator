/*
  Plus 1 parallel output / Plus 1 emulation.
  Copyright (C) 2019, David Boddie
  Part of Elkulator by Sarah Walker.
*/

#include <string.h>
#include <unistd.h>
#include "elk.h"

int parallel_fd = -1;

void resetparallel()
{
    extern char parallelname[512];

    if (strlen(parallelname))
        parallel_fd = socket_open(parallelname);
    else
        parallel_fd = STDOUT_FILENO;
}

void writeparallel(uint8_t val)
{
    switch (parallel_fd)
    {
        case STDOUT_FILENO:
        fputc(val, stdout);
        fflush(stdout);
        break;

        case -1:
        break;

        default:
        write(parallel_fd, &val, 1);
        break;
    }
}
