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

  printf("celestial bound to port 8080");

  close(server_fd);

  return EXIT_SUCCESS;
}