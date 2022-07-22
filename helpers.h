/* See LICENSE file for copyright and license details. */

#pragma once

/* throw a fatal error */
void die(const char* text);
/* a block of code meant to run before send/recv thread initialisation */
int pre_setup(int* socket_fd, char* nick, unsigned short* slot);
