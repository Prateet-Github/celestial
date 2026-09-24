#ifndef CELESTIAL_EVENT_HANDLER_H
#define CELESTIAL_EVENT_HANDLER_H

#include <sys/event.h>

#include "connection.h"

int handle_read_event(
    int kq,
    int server_fd,
    struct kevent *event,
    struct connection *connections
);

int handle_write_event(
    int kq,
    struct kevent *event,
    struct connection *connections
);

#endif