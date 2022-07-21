/* See LICENSE file for copyright and license details. */

#pragma once

/* throw a fatal error */
void die(const char* text);
/* run a set of code before initialising send/recv threads */
void pre_setup(int* socket_fd, const char* nick);
