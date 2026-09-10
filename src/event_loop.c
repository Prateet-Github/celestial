#include "event_loop.h"

#include <sys/event.h>
#include <unistd.h>

int event_loop_create(void)
{
  return kqueue();
}

int event_loop_add_read(int kq, int fd)
{
  struct kevent event;

  // here EV_SET tells kernel add this fd to the kqueue and notify when it is ready to read
  EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

  return kevent(kq, &event, 1, NULL, 0, NULL);
}

int event_loop_wait(int kq, struct kevent *events, int max_events)
{
  // wait for events to occur on the kqueue and populate the events array with the ready events
  return kevent(kq, NULL, 0, events, max_events, NULL);
}