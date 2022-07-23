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

    printf("configuration:\n"
        "    nick: %s\n"
        "    host: %s\n"
        "    port: %s\n\n",
        nick,
        host,
        port);

    /* null terminate the end of the nickname string */
    nick[MAX_NICK_LENGTH] = '\0';

    /* criteria for socket address structures returned from getaddrinfo */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    /* ipv4 */
    hints.ai_family = AF_INET;
    /* tcp */
    hints.ai_socktype = SOCK_STREAM;

    /* variable to hold server information */
    struct addrinfo* server_info;

    /* get hostname/service information; stored in a pointer to a addrinfo struct */
    if (getaddrinfo(host, port, &hints, &server_info) != 0) {
        close(socket_fd);
        die("failed to get server information! (incorrect host?)\n");
    }

    /* loop through server information linked list and connect to the first valid socket */
    for (struct addrinfo* iterator = server_info; iterator != NULL; iterator = iterator->ai_next) {
        /* get socket file descriptor */
        socket_fd = socket(iterator->ai_family, iterator->ai_socktype, 0);

        if (socket_fd == -1)
            continue;

        if (connect(socket_fd, iterator->ai_addr, iterator->ai_addrlen) != -1) {
            printf("successful socket creation!\n");
            break;
        }
    }

    if (socket_fd == -1) {
        close(socket_fd);
        die("failed to create socket! (is the server up?)\n");
    }

    /* free the linked list */
    freeaddrinfo(server_info);

    /* not using this at the moment */
    /*
    if (pre_setup(&socket_fd, nick, &slot))
        printf("pre setup returned with nick '%s' and slot %i\n", nick, slot);
    else
        fprintf(stderr, "pre setup failed!\n");
    */

    pthread_t thread_chat_recv;
    pthread_t thread_chat_send;
    /* create threads for simultaneously sending and recieving messages */
    pthread_create(&thread_chat_recv, NULL, &chat_recv, NULL);
    pthread_create(&thread_chat_send, NULL, &chat_send, NULL);

    /* constantly check if the client should shut down */
    while (1) {
        if (should_quit) {
            pthread_cancel(thread_chat_recv);
            pthread_cancel(thread_chat_send);
            break;
        }

        sleep(1);
    }

    /* wait until chat threads terminate */
    pthread_join(thread_chat_recv, NULL);
    pthread_join(thread_chat_send, NULL);
    /* close socket file descriptor */
    close(socket_fd);
    return 0;
}
