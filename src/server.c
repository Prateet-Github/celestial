#include "server.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int server_create(uint16_t port)
{
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1)
  {
    perror("socket");
    return -1;
  }

  int flags = fcntl(server_fd, F_GETFL, 0);

  if (flags == -1)
  {
    perror("fcntl F_GETFL");
    close(server_fd);
    return -1;
  }

  // Set the listening socket to non-blocking mode.
  if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
  {
    perror("fcntl F_SETFL");
    close(server_fd);
    return -1;
  }

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr.s_addr = htonl(INADDR_ANY)};

  if (bind(
          server_fd,
          (struct sockaddr *)&server_addr,
          sizeof(server_addr)) == -1)
  {
    perror("bind");
    close(server_fd);
    return -1;
  }

  if (listen(server_fd, 128) == -1)
  {
    perror("listen");
    close(server_fd);
    return -1;
  }

  return server_fd;
}

int server_accept(int server_fd)
{
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  int client_fd = accept(
      server_fd,
      (struct sockaddr *)&client_addr,
      &client_len);

  if (client_fd == -1)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
      return -1;
    }

    perror("accept");
    return -1;
  }

  return client_fd;
}