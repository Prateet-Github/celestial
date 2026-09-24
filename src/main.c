#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "event_loop.h"

#include "http.h"
#include "connection.h"
#include <errno.h>

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

      // if (event->filter != EVFILT_READ)
      // {
      //   continue;
      // }

      if (event->filter == EVFILT_READ)
      {
        if ((int)event->ident == server_fd)
        {
          int client_fd = server_accept(server_fd);

          if (client_fd == -1)
          {
            continue;
          }

          printf("Client connected: fd=%d\n", client_fd);

          // struct connection *connection = &connections[client_fd];

          // connection->fd = client_fd;
          // connection->read_length = 0;
          // connection->write_buffer = NULL;
          // connection->write_length = 0;
          // connection->write_offset = 0;

          struct connection *connection = &connections[client_fd];

          connection_init(connection, client_fd);

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
        else
        {
          int client_fd = (int)event->ident;

          printf(
              "Client fd=%d is readable\n",
              client_fd);

          struct connection *connection =
              &connections[client_fd];

          ssize_t bytes_read = connection_read(connection);

          if (bytes_read > 0)
          {
            printf("Received %zd bytes:\n%s\n", bytes_read, connection->read_buffer);

            size_t response_length;

            connection->write_buffer =
                http_response(&response_length);

            connection->write_length = response_length;
            connection->write_offset = 0;

            if (event_loop_add_write(kq, client_fd) == -1)
            {
              perror("event_loop_add_write");
              close(client_fd);
              continue;
            }

            printf(
                "Client fd=%d registered for write\n",
                client_fd);
          }
          else if (bytes_read == 0)
          {
            printf(
                "Client fd=%d disconnected\n",
                client_fd);

            close(client_fd);
          }
          else
          {
            perror("read");
            close(client_fd);
          }
        }
      }
      else if (event->filter == EVFILT_WRITE)
      {
        int client_fd = (int)event->ident;

        struct connection *connection =
            &connections[client_fd];

        printf(
            "Client fd=%d is writable\n",
            client_fd);

        ssize_t bytes_written =
            connection_write(connection);

        if (bytes_written == -1)
        {
          if (errno == EAGAIN ||
              errno == EWOULDBLOCK)
          {
            printf(
                "Client fd=%d not writable yet\n",
                client_fd);

            continue;
          }

          perror("write");
          close(client_fd);
          continue;
        }

        if (bytes_written == 0)
        {
          continue;
        }

        printf(
            "Sent %zd bytes to client fd=%d\n",
            bytes_written,
            client_fd);

        if (connection->write_offset ==
            connection->write_length)
        {
          printf(
              "Response completely sent to fd=%d\n",
              client_fd);

          if (event_loop_remove_write(kq, client_fd) == -1)
          {
            perror("event_loop_remove_write");
            close(client_fd);
            continue;
          }

          printf(
              "Write events disabled for fd=%d\n",
              client_fd);

          close(client_fd);
        }
      }
    }
  }

  close(kq);
  close(server_fd);

  return EXIT_SUCCESS;
}