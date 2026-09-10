#ifndef CELESTIAL_SERVER_H
#define CELESTIAL_SERVER_H

#include <stdint.h>

int server_create(uint16_t port);
int server_accept(int server_fd);

#endif