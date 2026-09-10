#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"

int main(void)
{
  int server_fd = server_create(8080);

  if (server_fd == -1)
  {
    return EXIT_FAILURE;
  }

  printf("Celestial listening on :8080\n");

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