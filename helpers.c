/* See LICENSE file for copyright and license details. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "globals.h"
#include "helpers.h"

#define SERVER_RESPONSE_SIZE 34

void die(const char* text)
{
    fprintf(stderr, "%s", text);
    exit(1);
}

int pre_setup(int* socket_fd, char* nick, unsigned short* slot)
{
    /* send the server our nickname */
    if (send(*socket_fd, nick, MAX_NICK_LENGTH, 0) == -1)
        fprintf(stderr, "failed to send pre setup information!\n");

    /* array big enough for the server response */
    char server_response[SERVER_RESPONSE_SIZE];

    /* recieve and store the server response */
    if (recv(*socket_fd, server_response, 34, 0) != -1) {
        /* store the first 32 bytes (nickname requested, if successful) */
        strncpy(nick, server_response, MAX_NICK_LENGTH);
        /* null terminate last character of nickname string */
        nick[MAX_NICK_LENGTH] = '\0';
        /* store the last 2 bytes (slot number) that the server returns */
        memcpy(slot, server_response + 32, 2);
        return 1;
    } else {
        fprintf(stderr, "failed to receive pre setup information response!\n");
        return 0;
    }
}
