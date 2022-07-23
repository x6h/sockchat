/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "chat.h"
#include "globals.h"

/* should the client expect a reply from the server */
static int expecting_reply = 0;

void* chat_recv(void* unused)
{
    while (1) {
        char output[MAX_RECV_LENGTH] = "";

        if (recv(socket_fd, output, MAX_RECV_LENGTH, 0) != -1) {
            /*
             * check if server sent back a message to verify the sent message was recieved
             * note: reply must be set up serverside
             */
            if (expecting_reply) {
                if (output[0] == '\x04' && output[1] == '\0') {
                    expecting_reply = 0;
                    continue;
                } else {
                    fprintf(stderr, "server did not reply back!\n");
                }
            }

            /* print the received message */
            printf("%s", output);
        }
    }

    return NULL;
}

void* chat_send(void* unused)
{
    while (1) {
        char input[MAX_SEND_LENGTH] = "";
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
                should_quit = 1;
                break;
            default:
                printf("command not found! (do '/h' for help)\n");
            }
        }

        if (send(socket_fd, input, strlen(input), 0) == -1)
            fprintf(stderr, "failed to write input to socket!\n");
        
        expecting_reply = 1;
    }

    return NULL;
}
