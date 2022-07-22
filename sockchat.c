/* See LICENSE file for copyright and license details. */

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "chat.h"
#include "globals.h"
#include "helpers.h"

void help_and_die()
{
    fprintf(stderr,
        "help:\n"
        "    -nick nickname\n"
        "        sets your nickname to 'nickname'\n\n"
        "    -host example.com\n"
        "        sets target host to 'example.com'\n\n"
        "    -port 7890\n"
        "        sets target port to 7890\n");
    die("");
}

int main(int argc, char* argv[])
{
    /* configuration */
    char* host = "example.com";
    char* port = "1337";
    char nick[MAX_NICK_LENGTH + 1] = "nickname";
    unsigned short slot = 0;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-host") == 0)
            host = argv[++i];
        else if (strcmp(argv[i], "-port") == 0)
            port = argv[++i];
        else if (strcmp(argv[i], "-nick") == 0)
            strncpy(nick, argv[++i], MAX_NICK_LENGTH);
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            help_and_die();
    }

    /* null terminate the end of the nickname string */
    nick[MAX_NICK_LENGTH] = '\0';

    printf("configuration:\n"
           "    nick: %s\n"
           "    host: %s\n"
           "    port: %s\n\n",
           nick,
           host,
           port);

    /* get socket file descriptor */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1)
        die("failed to open socket file descriptor!\n");

    struct addrinfo hints;
    /* ipv4 */
    hints.ai_family = AF_INET;
    /* tcp */
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* server_info;

    /* get hostname/service information; stored in a pointer to a addrinfo struct */
    if (getaddrinfo(host, port, &hints, &server_info) != 0) {
        close(socket_fd);
        die("failed to get server info! (incorrect host?)\n");
    }

    /* connect the socket (file descriptor) to the server */
    if (connect(socket_fd, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        close(socket_fd);
        die("connection failed! (is the server up?)\n");
    }

    /* free the linked list */
    freeaddrinfo(server_info);

    if (pre_setup(&socket_fd, nick, &slot))
        printf("pre setup returned with nick '%s' and slot %i\n", nick, slot);
    else
        fprintf(stderr, "pre setup failed!\n");

    pthread_t thread_chat_recv;
    pthread_t thread_chat_send;
    pthread_create(&thread_chat_recv, NULL, &chat_recv, NULL);
    pthread_create(&thread_chat_send, NULL, &chat_send, NULL);

    /* constantly check if the client should shut down */
    while (1) {
        if (should_quit) {
            pthread_cancel(thread_chat_recv);
            pthread_cancel(thread_chat_send);
            break;
        }

        usleep(1000 * 1000);
    }

    /* wait until chat threads terminate */
    pthread_join(thread_chat_recv, NULL);
    pthread_join(thread_chat_send, NULL);
    /* close socket file descriptor */
    close(socket_fd);
    return 0;
}
