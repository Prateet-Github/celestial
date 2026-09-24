#include "event_handler.h"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "server.h"
#include "event_loop.h"
#include "http.h"
#include "http_parser.h"
#include "request_handler.h"

int handle_read_event(
    int kq,
    int server_fd,
    struct kevent *event,
    struct connection *connections)
{
  int fd = (int)event->ident;

  if (fd == server_fd)
  {
    int client_fd = server_accept(server_fd);

    if (client_fd == -1)
    {
      return 0;
    }

    struct connection *connection =
        &connections[client_fd];

    connection_init(connection, client_fd);

    if (event_loop_add_read(kq, client_fd) == -1)
    {
      perror("event_loop_add_read");
      close(client_fd);
      return -1;
    }

    return 0;
  }

  struct connection *connection =
      &connections[fd];

  ssize_t bytes_read =
      connection_read(connection);

  if (bytes_read == 0)
  {
    close(fd);
    return 0;
  }

  if (bytes_read == -1)
  {
    if (errno == EAGAIN ||
        errno == EWOULDBLOCK)
    {
      return 0;
    }

    perror("read");
    close(fd);
    return -1;
  }

  struct http_request request;

  if (http_parse_request(
          connection->read_buffer,
          connection->read_length,
          &request) == -1)
  {
    printf("Invalid HTTP request\n");
    close(fd);
    return -1;
  }

  request_handle(&request);

  size_t response_length;

  connection->write_buffer =
      http_response(&response_length);

  connection->write_length =
      response_length;

  connection->write_offset = 0;

  if (event_loop_add_write(kq, fd) == -1)
  {
    perror("event_loop_add_write");
    close(fd);
    return -1;
  }

  return 0;
}

int handle_write_event(
    int kq,
    struct kevent *event,
    struct connection *connections)
{
  int fd = (int)event->ident;

  struct connection *connection =
      &connections[fd];

  ssize_t bytes_written =
      connection_write(connection);

  if (bytes_written == -1)
  {
    if (errno == EAGAIN ||
        errno == EWOULDBLOCK)
    {
      return 0;
    }

    perror("write");
    close(fd);
    return -1;
  }

  if (connection->write_offset ==
      connection->write_length)
  {
    if (event_loop_remove_write(kq, fd) == -1)
    {
      perror("event_loop_remove_write");
      close(fd);
      return -1;
    }

    close(fd);
  }

  return 0;
}