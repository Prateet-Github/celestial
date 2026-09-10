#ifndef CELESTIAL_EVENT_LOOP_H
#define CELESTIAL_EVENT_LOOP_H

int event_loop_create(void);
int event_loop_add_read(int kq, int fd);

#endif