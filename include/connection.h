#ifndef CELESTIAL_CONNECTION_H
#define CELESTIAL_CONNECTION_H

#include <stddef.h>
#include <sys/types.h>

#define CONNECTION_BUFFER_SIZE 4096

struct connection
{
  int fd;

  char read_buffer[CONNECTION_BUFFER_SIZE];
  size_t read_length;

  const char *write_buffer;
  size_t write_length;
  size_t write_offset;
};

void connection_init(
    struct connection *connection,
    int fd);

ssize_t connection_read(
    struct connection *connection);

ssize_t connection_write(
    struct connection *connection);

#endif