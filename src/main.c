#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "event_loop.h"

#include "connection.h"
#include "event_handler.h"

int main(void)
{
  int server_fd = server_create(8080);

  if (server_fd == -1)
  {
    return EXIT_FAILURE;
  }

  printf("Celestial listening on :8080\n");

  int kq = event_loop_create();

  if (kq == -1)
  {
    perror("kqueue");
    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("kqueue created: %d\n", kq);

  if (event_loop_add_read(kq, server_fd) == -1)
  {
    perror("event_loop_add_read");
    close(kq);
    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("Server socket registered with kqueue\n");

  struct kevent events[16];
  struct connection connections[1024] = {0};

  // main event loop
  while (1)
  {
    int event_count = event_loop_wait(
        kq,
        events,
        16);

    if (event_count == -1)
    {
      perror("kevent");
      break;
    }

    for (int i = 0; i < event_count; i++)
    {
      struct kevent *event = &events[i];

      if (event->filter == EVFILT_READ)
      {
        handle_read_event(kq, server_fd, event, connections);
      }
      else if (event->filter == EVFILT_WRITE)
      {
        handle_write_event(kq, event, connections);
      }
    }
  }

  close(kq);
  close(server_fd);

  return EXIT_SUCCESS;
}