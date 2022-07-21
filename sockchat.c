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
    fputs("help:\n"
          "    -nick nickname\n"
          "        sets your nickname to 'nickname'\n\n"
          "    -host example.com\n"
          "        sets target host to 'example.com'\n\n"
          "    -port 7890\n"
          "        sets target port to 7890\n",
          stderr);
    die("");
}

int main(int argc, char* argv[])
{
    /* configuration */
    char nick[32] = "nickname";
    char* host = "example.com";
    /* needs to be int if using sockaddr_in to connect */
    char* port = "1337";

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-nick") == 0)
            strncpy(nick, argv[++i], 32);
        else if (strcmp(argv[i], "-host") == 0)
            host = argv[++i];
        else if (strcmp(argv[i], "-port") == 0)
            port = argv[++i];
        else if (strcmp(argv[i], "-h") == 0)
            help_and_die();
    }

    printf("configuration:\n"
           "    nick: %s\n"
           "    host: %s\n"
           "    port: %s\n",
           nick,
           host,
           port);

    /* get socket file descriptor */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1)
        die("failed to open socket file descriptor!\n");

    struct addrinfo hints;
    /* AF_INET == ipv4 */
    hints.ai_family = AF_INET;
    /* SOCK_STREAM == tcp */
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
        die("failed to connect! (is the server up?)\n");
    }

    /* free the linked list */
    freeaddrinfo(server_info);

    /*
     * without getaddrinfo
     * note: gethostbyname is depricated and does not work well with ipv6.
     *       getaddrinfo is not supported by normal c standards like c89, c99, etc afaik.
     */
    /*
    struct sockaddr_in socket_data;
    struct hostent* server = gethostbyname(host);

    memset(&socket_data, 0, sizeof(socket_data));
    socket_data.sin_family = AF_INET;
    socket_data.sin_port = htons(port);
    memcpy(&socket_data.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    if (connect(socket_fd, (struct sockaddr*)&socket_data, sizeof(socket_data)) == -1) {
        close(socket_fd);
        die("failed to connect! (is the server up?)\n");
    }
    */

    pre_setup(&socket_fd, nick);

    pthread_t thread_chat_recv;
    pthread_t thread_chat_send;
    pthread_create(&thread_chat_recv, NULL, &chat_recv, NULL);
    pthread_create(&thread_chat_send, NULL, &chat_send, NULL);

    /* wait until chat threads terminate */
    pthread_join(thread_chat_recv, NULL);
    pthread_join(thread_chat_send, NULL);
    /* close socket file descriptor */
    close(socket_fd);
    printf("sucessfully shut down.\n");
    return 0;
}
