#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void)
{
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1)
  {
    perror("socket");
    return EXIT_FAILURE;
  }

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(8080),
      .sin_addr.s_addr = htonl(INADDR_ANY)};

  if (bind(
          server_fd,
          (struct sockaddr *)&server_addr,
          sizeof(server_addr)) == -1)
  {
    perror("bind");
    close(server_fd);
    return EXIT_FAILURE;
  }

  if (listen(server_fd, 128) == -1)
  {
    perror("listen");
    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("Celestial listening on :8080\n");

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  int client_fd = accept(
      server_fd,
      (struct sockaddr *)&client_addr,
      &client_len);

  if (client_fd == -1)
  {
    perror("accept");
    close(server_fd);
    return EXIT_FAILURE;
  }

  printf("Client connected!\n");

  close(client_fd);
  close(server_fd);

  return EXIT_SUCCESS;
}