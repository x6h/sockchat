/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "chat.h"
#include "globals.h"

void* chat_recv(void* unused)
{
    char output[MAX_RECV_LENGTH] = "";

    while (1) {
        if (recv(socket_fd, output, MAX_RECV_LENGTH, 0) != -1) {
            /* check if server sent back a message to verify a message was sent and recieved */
            /* note: server specific message */
            if (output[0] == 'r' && output[1] == 'd' && output[2] == '\0') {
                expecting_reply = 0;
                continue;
            } else if (expecting_reply) {
                fputs("server did not reply back!\n", stderr);
            }

            printf("%s", output);
        }
    }

    return NULL;
}

void* chat_send(void* unused)
{
    char input[MAX_SEND_LENGTH] = "";

    while (1) {
        fgets(input, MAX_SEND_LENGTH, stdin);

        /* quit command */
        if (input[0] == '/' && input[1] == 'q') {
            strcpy(input, "(client) i am leaving!");
            should_quit = 1;
            break;
        }

        if (send(socket_fd, input, strlen(input), 0) == -1)
            fputs("failed to write input to socket!\n", stderr);
        
        expecting_reply = 1;
    }

    return NULL;
}
