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

    printf("Received %d event(s)\n", event_count);

    for (int i = 0; i < event_count; i++)
    {
      struct kevent *event = &events[i];

      printf(
          "Event: fd=%lu filter=%d\n",
          event->ident,
          event->filter);

      if (event->filter != EVFILT_READ)
      {
        continue;
      }

      if ((int)event->ident == server_fd)
      {
        int client_fd = server_accept(server_fd);

        if (client_fd == -1)
        {
          continue;
        }

        printf("Client connected: fd=%d\n", client_fd);

        if (event_loop_add_read(kq, client_fd) == -1)
        {
          perror("event_loop_add_read client");
          close(client_fd);
          continue;
        }

        printf(
            "Client fd=%d registered with kqueue\n",
            client_fd);
      }

      /*
       * Client socket is readable:
       * data has arrived from the client.
       */
      else
      {
        int client_fd = (int)event->ident;

        printf(
            "Client fd=%d is readable\n",
            client_fd);

        char buffer[4096];

        ssize_t bytes_read = read(
            client_fd,
            buffer,
            sizeof(buffer) - 1);

        if (bytes_read > 0)
        {
          buffer[bytes_read] = '\0';

          printf(
              "Received %zd bytes:\n%s\n",
              bytes_read,
              buffer);
        }
      }
    }
  }

  close(kq);
  close(server_fd);

  return EXIT_SUCCESS;
}