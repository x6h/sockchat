/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "helpers.h"

void die(const char* text)
{
    fputs(text, stderr);
    exit(1);
}

void pre_setup(int* socket_fd, const char* nick)
{
    if (send(*socket_fd, nick, 32, 0) == -1)
        fputs("failed to run pre_setup!\n", stderr);
}
