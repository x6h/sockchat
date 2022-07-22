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
            /* check if server sent back a message to verify the sent message was recieved */
            /* note: server specific message */
            if (output[0] == 'r' && output[1] == 'd' && output[2] == '\0') {
                expecting_reply = 0;
                continue;
            } else if (expecting_reply) {
                fprintf(stderr, "server did not reply back!\n");
            }

            /* print the received message */
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

        /* detect commands */
        if (input[0] == '/') {
            switch (input[1]) {
            case 'h':
                printf("/h = show help\n"
                    "/q = quit\n");
                break;
            case 'q':
                /* message sent to server */
                strcpy(input, "(client) i am leaving!");
                /* let main function know to terminate threads */
                should_quit = 1;
                break;
            default:
                fprintf(stderr, "command not found. try '/h'.\n");
            }
        }

        if (send(socket_fd, input, strlen(input), 0) == -1)
            fprintf(stderr, "failed to write input to socket!\n");
        
        expecting_reply = 1;
    }

    return NULL;
}
