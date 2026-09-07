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

  printf("socket created: %d\n", server_fd);
  close(server_fd);

  return EXIT_SUCCESS;
}