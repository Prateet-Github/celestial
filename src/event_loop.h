#ifndef CELESTIAL_EVENT_LOOP_H
#define CELESTIAL_EVENT_LOOP_H

#include <sys/event.h>

int event_loop_create(void);

int event_loop_add_read(int kq, int fd);

int event_loop_wait(
    int kq,
    struct kevent *events,
    int max_events);

#endif