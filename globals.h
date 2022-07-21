/* See LICENSE file for copyright and license details. */

#pragma once

#define MAX_RECV_LENGTH 1024
#define MAX_SEND_LENGTH 1024

/* socket file descriptor */
extern int socket_fd;
/* should the client expect a reply from the server */
extern int expecting_reply;
/* should the client shut down */
extern int should_quit;
