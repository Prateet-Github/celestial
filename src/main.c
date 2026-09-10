#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(void)
{
  // create a socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1)
  {
    perror("socket");
    return EXIT_FAILURE;
  }

  int flags = fcntl(server_fd, F_GETFL, 0);

  if (flags == -1)
  {
    perror("fcntl F_GETFL");
    close(server_fd);
    return EXIT_FAILURE;
  }

  // set the socket to non blocking mode
  if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
  {
    perror("fcntl F_SETFL");
    close(server_fd);
    return EXIT_FAILURE;
  }

  // set up the server address
  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(8080),
      .sin_addr.s_addr = htonl(INADDR_ANY)};

  // bind
  int bind_result = bind(
      server_fd,
      (struct sockaddr *)&server_addr,
      sizeof(server_addr));

  if (bind_result == -1)
  {
    perror("bind");
    close(server_fd);
    return EXIT_FAILURE;
  }

  // listen
  int listen_result = listen(server_fd, 128);

  if (listen_result == -1)
  {
    perror("listen");
    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("Celestial listening on :8080\n");

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  // accept
  int client_fd = accept(
      server_fd,
      (struct sockaddr *)&client_addr,
      &client_len);

  if (client_fd == -1)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
      printf("No connection available\n");
    }
    else
    {
      perror("accept");
    }

    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("Client connected!\n");

  // buffer to hold incoming data
  char buffer[4096];

  // read
  ssize_t bytes_read = read(
      client_fd,
      buffer,
      sizeof(buffer) - 1);

  if (bytes_read == -1)
  {
    perror("read");
    close(client_fd);
    close(server_fd);
    return EXIT_FAILURE;
  }

  // null terminate the buffer
  buffer[bytes_read] = '\0';

  printf("Received %zd bytes:\n", bytes_read);
  printf("%s\n", buffer);

  // send a response to the client
  const char *response =
      "HTTP/1.1 200 OK\r\n"
      "Content-Length: 17\r\n"
      "Connection: close\r\n"
      "\r\n"
      "Hello, Celestial!";

  size_t response_length = strlen(response);

  // write
  ssize_t bytes_written = write(
      client_fd,
      response,
      response_length);

  if (bytes_written == -1)
  {
    perror("write");
    close(client_fd);
    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("Sent %zd bytes\n", bytes_written);

  // close the sockets
  close(client_fd);
  close(server_fd);

  return EXIT_SUCCESS;
}
