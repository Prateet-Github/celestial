#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "event_loop.h"

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

  int event_count = event_loop_wait(
      kq,
      events,
      16);

  if (event_count == -1)
  {
    perror("kevent");
    close(kq);
    close(server_fd);
    return EXIT_FAILURE;
  }

  for (int i = 0; i < event_count; i++)
  {
    printf(
        "Event: fd=%lu filter=%d\n",
        events[i].ident,
        events[i].filter);
  }

  printf("Received %d event(s)\n", event_count);

  int client_fd = server_accept(server_fd);

  if (client_fd == -1)
  {
    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("Client connected!\n");

  close(client_fd);
  close(server_fd);

  return EXIT_SUCCESS;
}